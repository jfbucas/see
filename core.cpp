/*
 *  Le coeur du programme
 *  Gestion des demandes de l'utilisateur
 *
 */


#include <SDL.h>

#include "core.h"
#include "video.h"
#include "dir.h"


// filename peut être un répertoire ou un fichier
Core::Core(const string dir, const string filename) {

	//cout << "Init Core" << endl;

	done = false;
	reload = false;

	timer_counter = 0;
	cursor_mouse_timer = 0;

	showCommentedFilesOnly = false;
	
	emphase_mode = false;
	edit_mode = false;
	edited_comment = "";

	next_by_click = false;
	
	// startup SDL with timer
	if (SDL_Init( SDL_INIT_TIMER ) < 0) 
		cerr << "SDL_Init_Timer: " << SDL_GetError() << endl;
	
	// we cut window resize event
	//SDL_EventState(SDL_VIDEORESIZE, SDL_IGNORE);

	dirList = new Dir(dir);
	//dirList->afficher();

	desiredFilename = filename;
}

// Destructeur
Core::~Core() {
	// shutdown SDL
	SDL_Quit();
	
	// Free memory
	delete dirList;
	delete video;
	
	//cout << "Done Core." << endl;

	//cout << endl << "T'Chümps !" << endl;
}

// on entre dans le mode édition de commentaire
bool Core::enter_edit_mode() {
	edit_mode = true; 
	cursor_keyboard = false;
	video->toggleComment(true);
	SDL_EnableUNICODE(1);
	edited_comment = current->getCommentaire();
	cursor_position = edited_comment.length();
	
	// on insère un évenement pour demander le rafraichissement
	SDL_Event event;
	event.type = SDL_USEREVENT;
	event.user.code = TIMER_EVENT;
	SDL_PushEvent(&event);
	return true;
}

// on quite le mode edition
bool Core::leave_edit_mode(bool save) {
	edit_mode = false;
	SDL_EnableUNICODE(0);
	if (save) {
		current->setCommentaire(edited_comment);
		dirList->updateDescription(*current);
	}
	video->generateGLComment(current->getCommentaire());
	video->refresh();
	return true;
}

// Rotation d'une image en passant pas l'appel externe à jpegtran
// TODO Vérifier que l'image est bien JPEG !!!
bool Core::Rotate(int angle) {
	int result = 0;
	string renommage, commande, deletage;

	commande = "jpegtran -rotate ";
	switch ( angle ) {
		case  90 : commande +=  "90 "; break;
		case 180 : commande += "180 "; break;
		case 270 : commande += "270 "; break;
	}
	commande += "-outfile \"" + current->getFullNom() + "\" \"" + current->getFullNom() + ".original\"";
	renommage = "mv \"" + current->getFullNom() + "\" \"" + current->getFullNom() + ".original\"";
	deletage = "rm -f \"" + current->getFullNom() + ".original\"";

	cout << renommage << endl;
	cout << commande << endl;
	cout << deletage << endl;
	
	if ( result != -1 ) result = system(renommage.c_str());	// on renomme
	if ( result != -1 ) result = system(commande.c_str());	// on tourne
	//if ( result != -1 ) result = system(deletage.c_str());	// on supprime
	if ( result != -1 ) video->viewImage(*current);		// reload the picture

	return (result == -1);	
}

// the keyboard events are handled here
bool Core::handle_keyboard( const SDL_KeyboardEvent & event ) {
	bool result = false;

	if ( edit_mode ) { // MODE EDITION DES COMMENTAIRES
		unsigned int l = edited_comment.length();
		switch( event.keysym.sym ) { 
		case SDLK_RETURN: leave_edit_mode(true);	break;
		case SDLK_ESCAPE: leave_edit_mode(false);	break;
		case SDLK_BACKSPACE: edited_comment = edited_comment.substr(0, edited_comment.length()-1);		break;
		
		default: 
			char ch;
			//cout << "Code = " << event.keysym.unicode << endl;
			if ( (event.keysym.unicode & 0xFF00) == 0 ) {
				ch = event.keysym.unicode & 0xFF;
				if (ch != 0) edited_comment.push_back(ch);
			 }else{
			       cout << "Not  handled key " << endl;
			 }
			break;
		}
		if (l != edited_comment.length()) {
			// on insère un évenement pour demander le rafraichissement
			SDL_Event event;
			event.type = SDL_USEREVENT;
			event.user.code = TIMER_EVENT;
			SDL_PushEvent(&event);
		}
	} else 	if ( event.keysym.mod & KMOD_SHIFT ) { // une touche shiftée
		switch( event.keysym.sym ) {  	
		case SDLK_SPACE:current = dirList->getPrec(current);	break;
		case SDLK_z:		video->deZoom();break;
		case SDLK_q:		done = 1;	break;
		// Rotation
		case SDLK_LEFT : Rotate(270);	break;
		case SDLK_UP   :
		case SDLK_DOWN : Rotate(180);	break;
		case SDLK_RIGHT: Rotate( 90);	break;

		default: cout << "Not shifted handled key " << event.keysym.sym << endl;	break;
		}
		
	} else if ( event.keysym.mod & KMOD_CTRL ) { // une touche controlée
		switch( event.keysym.sym ) {  
		default: cout << "Not controled handled key " << endl;	break;
		}

	} else {
		switch( event.keysym.sym ) {  // une touche normale a été frappée
		// Next Picture
		case SDLK_RETURN:
		case SDLK_SPACE:if (showCommentedFilesOnly)
					current = dirList->getNextCommented(current);
				else
					current = dirList->getNext(current); break;
		case SDLK_HOME: if (showCommentedFilesOnly)
					current = dirList->getFirstCommented();
				else
					current = dirList->getFirst();	break;

		case SDLK_END:  current = dirList->getLast();		break;
		case SDLK_F5:	reload = true;				break;
		// Comment
		case SDLK_c:	video->toggleComment();			break;
		case SDLK_i:
		case SDLK_e:	enter_edit_mode();			break;
		case SDLK_BACKSPACE: current->setCommentaire(current->getCommentaire().substr(0, current->getCommentaire().length()-1));
				dirList->updateDescription(*current);
				video->generateGLComment(current->getCommentaire());
				video->refresh();			break;
		case SDLK_AMPERSAND:	
		case SDLK_1:	
		case SDLK_F1:	current->setCommentaire(current->getCommentaire() + "a");
				dirList->updateDescription(*current);
				video->generateGLComment(current->getCommentaire());
				video->refresh();			break;
		case 233:	// SDLK_EACUTE
		case SDLK_2:	
		case SDLK_F2:	current->setCommentaire(current->getCommentaire() + "b");
				dirList->updateDescription(*current);
				video->generateGLComment(current->getCommentaire());
				video->refresh();			break;
		case SDLK_QUOTEDBL:	
		case SDLK_3:	
		case SDLK_F3:	current->setCommentaire(current->getCommentaire() + "c");
				dirList->updateDescription(*current);
				video->generateGLComment(current->getCommentaire());
				video->refresh();			break;
		case SDLK_QUOTE:	
		case SDLK_4:	
		case SDLK_F4:	current->setCommentaire(current->getCommentaire() + "d");
				dirList->updateDescription(*current);
				video->generateGLComment(current->getCommentaire());
				video->refresh();			break;
		// Emphase mode
		case SDLK_F12:	setEmphasis(!emphase_mode);		
				video->generateGLEmphase(*current);
				video->refresh();			break;
		case SDLK_k:	current->emphaseClear(0);
				video->generateGLEmphase(*current);
				video->refresh();			break;
		// Window Manager
		case SDLK_ESCAPE:
		case SDLK_q:
		case SDLK_F10:	done = 1;				break;
		case SDLK_f:	video->toggleFullscreen();		break; 
		// Zoom 
		case SDLK_z:
		case SDLK_KP_PLUS:
		case SDLK_PLUS:		video->Zoom();	break;
		case SDLK_s:
		case SDLK_KP_MINUS:
		case SDLK_MINUS:	video->deZoom();break;
		case SDLK_a:
		case SDLK_KP_MULTIPLY:
		case SDLK_ASTERISK:	video->noZoomNCenter();	break;
		// Déplacement
		case SDLK_LEFT:	video->Vertical(1);	break;
		case SDLK_RIGHT:video->Vertical(-1);	break;
		case SDLK_UP:	video->Horizontal(-1);	break;
		case SDLK_DOWN:	video->Horizontal(1);	break;

		default: /*cout << "Not handled key " << endl;*/	break;
									
		}
	}
	return result;
}

// the mouse events are handled here
bool Core::handle_mouse( const SDL_MouseButtonEvent & event) {
	bool result = false;
	//old_current = current;
	//cout << "code bouton : " << int(event.button) << endl;
	switch (event.button) {
		case SDL_BUTTON_WHEELUP:	current = dirList->getPrec(current); break;
		case SDL_BUTTON_WHEELDOWN:	current = dirList->getNext(current); break;
		case SDL_BUTTON_MIDDLE:	break;
		default: break;
	}

	// in tablet mode
	if ( next_by_click ) {
		cout << "next by click : code bouton : " << int(event.button) << endl;
		switch (event.button) {
			case SDL_BUTTON_MIDDLE:	current = dirList->getPrec(current); break;
			case SDL_BUTTON_LEFT:	current = dirList->getNext(current); break;
		}
	}

	if ( emphase_mode ) {
		int px = video->PosPixelPicX(event.x);
		int py = video->PosPixelPicY(event.y);

		switch (event.button) {
			case SDL_BUTTON_LEFT: current->emphasePlus(px, py);     break;
			case SDL_BUTTON_RIGHT: current->emphaseMoins(px, py);   break;
		}
		video->generateGLEmphase(*current);
		video->refresh();
	}
	return result;
}

// the mouse motion events are handled here
bool Core::handle_mouse_motion( const SDL_MouseMotionEvent & event) {
	bool result = false;
	int px = 0, py = 0;
	//cout << "code status : " << int(event.state) << endl;
	//cout << "BTN 1 : " << bool(event.state & SDL_BUTTON(1)) << endl;
	//cout << "BTN 2 : " << bool(event.state & SDL_BUTTON(2)) << endl;
	//cout << "BTN 3 : " << bool(event.state & SDL_BUTTON(3)) << endl;
	
	if ( emphase_mode ) {

		px = video->PosPixelPicX(event.x);
		py = video->PosPixelPicY(event.y);
		
		//cout << " Position pic " << px << ", " << py << endl;
	
		// left click
		if ( event.state & SDL_BUTTON(1)) {
			current->emphasePlus(px, py);
		}

		// right click
		if ( event.state & SDL_BUTTON(3)) {
			current->emphaseMoins(px, py);
		}

		video->generateGLEmphase(*current);
		video->refresh();
	}	

	return result;

}

// the timer generated event handled here
bool Core::handle_userevent() {
	bool result = false;
	string temp;

	if ( edit_mode ) {
		cursor_keyboard = !cursor_keyboard;
		
		temp.append(edited_comment);
		if (cursor_keyboard) 
			temp.push_back('.');
		else
			temp.push_back(' ');

		video->generateGLComment(temp);
		video->refresh();
		
	}
	
	return result;
}

// Timer Event -- STATIC --
Uint32 Core::handle_timer(Uint32 interval, void* tptr) {
	Core * myCore = (Core *)tptr;
	
	// **** Afficher le nom de l'image pendant 1sec****

	myCore->timer_counter++;
	//cout << "Timer " << myCore->timer_counter << endl;

	// pour enlever le curseur de la souris après 4 * 1/2 seconde d'inactivité
	if ( myCore->timer_counter - myCore->cursor_mouse_timer > 3 ) {
		SDL_ShowCursor(SDL_DISABLE);
	}	
	
	// insertion d'un évenement timer toute les secondes
	SDL_Event event;
	event.type = SDL_USEREVENT;
	event.user.code = TIMER_EVENT;
	SDL_PushEvent(&event);
	
	return interval;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// 
// Main loop of the program
// 
bool Core::run() {
	list<Fichier>::iterator oldcurrent;
	bool result = false;
	SDL_Event event;

	// Look for the wanted file
	if (desiredFilename.empty() )
		if (showCommentedFilesOnly)
			current = dirList->getFirstCommented();
		else
			current = dirList->getFirst();
	else
		current = dirList->find_file(desiredFilename);


	if (current == dirList->getEnd() ) {
		cerr << "  No image for see ! (-h for help)" << endl;
		done = true;
	} else {
		video = new Video();
		//cout << "Le fichier ouvert est " << current->getNom() << " (" << current->getCommentaire() << ")" << endl;
		done = false;
	}

	// Set the Timer iteration to 1 sec
	id_timer = SDL_AddTimer(1 SECONDE / 2, &(Core::handle_timer), this);
	if (! id_timer ) cerr << "Timer install problem" << endl;
	
	// set the keyboard repeat frequency - must be here
	SDL_EnableKeyRepeat(250,20);
	
	// Put the first picture on screen	
	result = video->viewImage(*current);
	
	// on insère un évenement de rafraichissement
	//event.type = SDL_VIDEOEXPOSE;
	//SDL_PushEvent(&event);

	// ------------------------------------------------------------------
	// the event loop, redraws if needed, quits on keypress or quit event
	// 
	while (!result && !done && SDL_WaitEvent(&event)!=-1 ) {
		// we save previous seen picture to detect event
		oldcurrent = current;
		
		switch (event.type) {
		case SDL_MOUSEMOTION:	  SDL_ShowCursor(SDL_ENABLE);
					  cursor_mouse_timer = timer_counter;
					  result = handle_mouse_motion( event.motion );	break;
		case SDL_MOUSEBUTTONDOWN: result = handle_mouse( event.button );	break;
		case SDL_MOUSEBUTTONUP:						break;
		case SDL_KEYUP: 						break;
		case SDL_KEYDOWN:	result = handle_keyboard( event.key );	break;
		case SDL_QUIT:		done = true;				break;
		case SDL_VIDEOEXPOSE:	result = video->refresh();		break;
		case SDL_VIDEORESIZE:	result = video->resizeWindow(event.resize.w, event.resize.h); break;
		case SDL_SYSWMEVENT:	cout << "Window Manager" << endl;	break;
		case SDL_USEREVENT:	result = handle_userevent();		break;
		case SDL_ACTIVEEVENT:	/*cout << "Active" << endl;*/		break;
		default:		cout << "other type ! " << event.type << endl; break;
		}

		// another picture was asked
		if (((current != oldcurrent)||(reload)) && !result) {
			result = video->viewImage(*current);
			reload = false;
		}
		//cout << "un tour de boucle " << endl;
	}

	if (result) cerr << "Resultat pas cool" << endl;

	return result;
}
