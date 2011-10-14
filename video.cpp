/*
 *	Gestion de la video OpenGL et SDL
 */

#include "video.h"


// To see a picture
bool Video::viewImage(Fichier & fichier) {
	//cout << "  View Image" << endl;

	/* set the window title to the filename */
	SDL_WM_SetCaption((fichier.getNom()).c_str(), (fichier.getNom()).c_str());
	
	if ( fichier.loadImg() ) {
	
		// dimension of screen
		current_w	= fichier.getWidth();
		current_h	= fichier.getHeight();
		current_bpp	= fichier.getBitsPerPixel();
		current_fichier = &fichier;
		
		// Create Screen
		if (fullscreen) {
			resizeWindow( desktop_w, desktop_h, current_bpp );
		}else{
			resizeWindow( current_w, current_h, current_bpp );
		}
		
		// si déjà une texture en mémoire, on la libère
		if ( img_text_id ) {
			glDeleteTextures(1, &img_text_id);
			img_text_id = 0;
		}
		if ( comment_text_id[0] ) {
			glDeleteTextures(2, comment_text_id );
			comment_text_id[0] = 0;
			comment_text_id[1] = 0;
		}

		// chargement des textures
		if ( !loadGLTextures(fichier) ) {
			cerr << "can't load texture with file " << fichier.getNom() << endl;
			return true;
		}

		// dessine la scene
		drawGLScene();

		// free memory libérée-ty
		fichier.freeImg();

	}else{
		cerr << fichier.getNom() << " is not viewable" << endl;
	}
	
	return ! fichier.isVisible();
}//view_image

/* function to load in bitmap as a GL texture */
bool Video::loadGLTextures(Fichier & fichier) {

	SDL_Surface * image;
	SDL_Surface * GL_texture;
	SDL_PixelFormat *fmt;
	//SDL_PixelFormat convert;

	int n, m, cutn, cutm;
	GLint GL_internal_format	= 3;
	GLenum GL_format		= GL_RGB;
	GLenum GL_type			= GL_UNSIGNED_BYTE;

	// Chargement du fichier.
	image = fichier.getImage();
	fmt = image->format;
		
	//cout << "  " << fichier.getNom() << " : " << image->w << "x" << image->h 
	//     << " " << (int)fmt->BitsPerPixel << "bpp " <<endl;
		
	switch ((int)fmt->BitsPerPixel) {
		case 8:
			//cout << "    GL_Param - internalfmt=" << GL_internal_format << " fmt=" << GL_format << " type=" << GL_type << endl;
			/*convert.palette = NULL; //fmt->palette;
			convert.BitsPerPixel = 32; //fmt->BitsPerPixel;
			convert.BytesPerPixel = 4; //fmt->BytesPerPixel;
			convert.Amask = 0xff000000; //fmt->Amask;
			convert.Rmask = 0xff0000;//fmt->Rmask;
			convert.Gmask = 0xff00;//fmt->Gmask;
			convert.Bmask = 0xff;//fmt->Bmask;
			convert.Ashift = 24;//fmt->Ashift;
			convert.Rshift = 16;//fmt->Rshift;
			convert.Gshift = 8;//fmt->Gshift;
			convert.Gshift = 0;//fmt->Gshift;*/
			//image = SDL_ConvertSurface(fichier.getImage(), screen->format, SDL_SWSURFACE | SDL_HWSURFACE ); //SDL_SWSURFACE);
			//if ( image == NULL ) {
				cout << "8bits image not viewable" << endl;
				return false;
			//}
		break;
		case 24:
		break;
	}
	

	// Il faut réinjecter l'image dans une SDL_Surface de dimension 2^n x 2^m
	// pour transmettre en texture à OpenGL
	n = fichier.getGLWidth();
	m = fichier.getGLHeight();

	position[Z] = 1 + RACINE_DE_2;
	if ((n == image->w) && (m == image->h)) {
		// pas besoin de mettre dans une autre texture
		GL_texture = image;
	}else{
		// dimension de la partie non utilisée de la texture
		cutn = n - image->w;
		cutm = m - image->h;
		
		// on créer une texture avec les bonnes dimensions
		GL_texture = SDL_CreateRGBSurface(SDL_HWSURFACE, n, m, fmt->BitsPerPixel, 0,0,0,0);

		// on copie toute l'image dans la nouvelle texture
		// et on la place au milieu
		SDL_Rect dstR;
		dstR.x = cutn/2;
		dstR.y = cutm/2;
		dstR.w = image->w;
		dstR.h = image->h;

		SDL_BlitSurface(image, NULL, GL_texture, &dstR);

		// on corrige la position de la caméra pour tenir compte de la nouvelle texture
		// Pk se prendre la tête ?
		position[Z] *= 1 - (GLfloat)cutm/(GLfloat)m;
	}

			

	GL_format = GL_BGR;
	GL_type = GL_UNSIGNED_BYTE;
	GL_internal_format = fmt->BytesPerPixel;

	// ---- Création d'un objet de texture. ----
	//have_texture = true;
	glGenTextures (1, &img_text_id);
    	if ( img_text_id == 0 )
		cerr << " impossible d'allouer un ID texture " << endl;
	glBindTexture (GL_TEXTURE_2D, img_text_id);

	// Paramétrage de la texture.
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Linear Filtering 
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	
	// Jonction entre OpenGL et SDL.
	glTexImage2D ( GL_TEXTURE_2D, 0, GL_internal_format,
			GL_texture->w, GL_texture->h, 0, 
			GL_format, GL_type, GL_texture->pixels);

	if ( GL_texture != image )
		SDL_FreeSurface(GL_texture);
	
	
	if ( image != fichier.getImage() ){
		//cout << " converted surface freed" << endl;
		SDL_FreeSurface(image);
	}
		
	if ( !fichier.getCommentaire().empty() ) { 
		if ( font ) {
			generateGLComment( fichier.getCommentaire() );
			//cout << fichier.getNom() << " : '" << fichier.getCommentaire() << "'" << endl;
		} else {
			//cout << fichier.getNom() << " : '" << fichier.getCommentaire() << "'" << endl;
		}
	}else{
	}

	if ( fichier.emphaseValue() > 0 ){
		generateGLEmphase(fichier);
	}else{
		if (emphase_text_id) {
			glDeleteTextures(1, &emphase_text_id );
			emphase_text_id = 0;
		}
	}

	
	return fichier.isVisible();
}

// bind the emphase texture into a GL texture
bool Video::generateGLEmphase(Fichier & fichier) {
	if (emphase_text_id) {
		glDeleteTextures(1, &emphase_text_id );
		emphase_text_id = 0;
	}

	SDL_Surface * emphase_texture =  SDL_CreateRGBSurface(SDL_SWSURFACE, EMPHASE_W, EMPHASE_H, 32, 0,0,0,0);
	if ( !emphase_texture )
		cerr << "Impossible de créer la texture emphase" << endl;


	SDL_LockSurface(emphase_texture);

	Uint32 * dst = (Uint32 *)emphase_texture->pixels;
	for(int y=0 ; y < EMPHASE_H; y++) {
		for(int x=0 ; x < EMPHASE_W; x++) {
			int e = fichier.getEmphase(x, y);
			if (e > 0) {
				if (e > 7) { e = 7; }
				//(*dst) = SDL_MapRGBA(emphase_texture->format, 127 + (1 <<  e), 63 + (1 <<  e), 0, 128);
				(*dst) = SDL_MapRGBA(emphase_texture->format, 0, 127 + (1 <<  e), 0, 128);
			}else if (e < 0) {
				if (-e > 7) { e = -7; }
				//(*dst) = SDL_MapRGBA(emphase_texture->format, 127 + (1 << -e), 0, 127 + (1 << -e), 128);
				(*dst) = SDL_MapRGBA(emphase_texture->format, 127 + (1 << -e), 0, 0, 128);
			}else {
				(*dst) = SDL_MapRGBA(emphase_texture->format, 0,0,0, 255);
			}
			dst ++;
		}
	}
	
	SDL_UnlockSurface(emphase_texture);

	// ---- Création d'un objet de texture. ----
	glGenTextures (1, &emphase_text_id);
    	if ( emphase_text_id == 0 )
		cerr << " impossible d'allouer un ID emphase texture " << endl;
		
	// Texture du mask
	glBindTexture (GL_TEXTURE_2D, emphase_text_id);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGBA8, emphase_texture->w, emphase_texture->h, 0, 
				GL_BGRA, GL_UNSIGNED_BYTE, emphase_texture->pixels );


	SDL_FreeSurface(emphase_texture);

	return false;
}

// si on a un commentaire alors, il faut l'enregistrer comme texture également
bool Video::generateGLComment(const string & commentaire) {
	if (commentaire.empty()) return true;

	const char * comment = commentaire.c_str();

	//cout << "commentaire is : " << comment << endl;

	//int ttf_height = TTF_FontHeight(font);
	SDL_Surface * img_comment;
	SDL_Surface * comment_texture;
	SDL_Surface * comment_texture_mask;
	SDL_Color color;
	SDL_Rect dstR;

	// si y a un ancien commentaire, on le libère
	if ( comment_text_id[0] ) {
		glDeleteTextures(2, comment_text_id );
		comment_text_id[0] = 0;
		comment_text_id[1] = 0;
	}

		
	// Draw Black background of the font - contour noir
	color.r = 255;	color.b = 0;	color.g = 0;
	img_comment = TTF_RenderText_Blended(font, comment, color);
	if (!img_comment){
		cerr << "erreur de génération " << endl;
		return true;
	}else{

		int GLw=1; while (GLw < img_comment->w) GLw = GLw << 1;
		int GLh=1; while (GLh < img_comment->h) GLh = GLh << 1;
		comment_texture      = SDL_CreateRGBSurface(SDL_SWSURFACE, GLw, GLh, 32, 0,0,0,0);
		comment_texture_mask = SDL_CreateRGBSurface(SDL_SWSURFACE, GLw, GLh, 32, 0,0,0,0);
		if ( !comment_texture || !comment_texture_mask )
			cerr << "Impossible de créer la texture commentaire" << endl;
		
		//SDL_FillRect(comment_texture, NULL, SDL_MapRGBA(comment_texture->format, 127,127,127,0));
		//SDL_SetAlpha(comment_texture, 0, 255);
		
		dstR.x = (comment_texture->w /2) - (img_comment->w /2);
		dstR.y = (comment_texture->h /2) - (img_comment->h /2);
		dstR.w = comment_texture->w;
		dstR.h = comment_texture->h;
		
		SDL_Rect rect;
		rect.w = dstR.w;;
		rect.h = dstR.h;
		for(int x = -1; x<=1; x++) {
			for(int y = -1; y<=1; y++) {
				rect.x = x + dstR.x;
				rect.y = y + dstR.y;
		//		SDL_BlitSurface(img_comment, NULL, comment_texture, &rect);
			}
		}
		SDL_FreeSurface(img_comment);
			
		// Put colored text
		color.r = 255;	color.b = 255;	color.g = 255;
		img_comment = TTF_RenderText_Blended(font, comment, color);
		SDL_BlitSurface(img_comment, NULL, comment_texture, &dstR);
		SDL_BlitSurface(img_comment, NULL, comment_texture_mask, &dstR);
		SDL_FreeSurface(img_comment);

		// Create Mask
		//SDL_LockSurface(comment_texture);
		SDL_LockSurface(comment_texture_mask);

		Uint32 black = SDL_MapRGBA(comment_texture->format, 0,0,0,0);
		Uint32 white = SDL_MapRGBA(comment_texture->format, 255,255,255,0);
	 	//Uint32 * src = (Uint32 *)comment_texture->pixels;
	 	Uint32 * dst = (Uint32 *)comment_texture_mask->pixels;
		for(int c = 0 ; c < GLh * GLw; c++) {
			if ((*dst) == black) {
				(*dst) = white;
			}else{
				(*dst) = black;
			}
			//src ++;
			dst ++;
		}
		//SDL_UnlockSurface(comment_texture);
		SDL_UnlockSurface(comment_texture_mask);

		// ---- Création d'un objet de texture. ----
		glGenTextures (2, comment_text_id);
	    	if ( comment_text_id[0] == 0 )
			cerr << " impossible d'allouer un ID comment texture " << endl;
		
		// Texture du mask
		glBindTexture (GL_TEXTURE_2D, comment_text_id[0]);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGBA8, comment_texture_mask->w, comment_texture->h, 0, 
				GL_BGRA, GL_UNSIGNED_BYTE, comment_texture_mask->pixels );

		// Texture du texte
		glBindTexture (GL_TEXTURE_2D, comment_text_id[1]);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGBA8, comment_texture->w, comment_texture->h, 0, 
				GL_BGRA, GL_UNSIGNED_BYTE, comment_texture->pixels );

		// on sauvegarde la largeur de la texture
		comment_text_w = GLw;
		comment_text_h = GLh;
		
		SDL_FreeSurface(comment_texture);
		SDL_FreeSurface(comment_texture_mask);
	}
	
	return false;
}

/* Here goes our drawing code */
bool Video::drawGLScene( void ) {

    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    /* Clear The Screen And The Depth Buffer */
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    /* Move Into The Screen 5 Units */
    glLoadIdentity();


    GLfloat z = position[Z] - (GLfloat)(zoom * 0.05f);
    glTranslatef( position[X],position[Y], -z);

    /* Select Our Texture */
    glBlendFunc(GL_ONE, GL_ZERO);
    glBindTexture( GL_TEXTURE_2D, img_text_id );

    /* Carré de la picture */
    glBegin(GL_QUADS);
      // Bottom Left Of The Texture and Quad
      glTexCoord2f( 0.0f, 1.0f ); glVertex2f( -1.0f, -1.0f);
      // Bottom Right Of The Texture and Quad
      glTexCoord2f( 1.0f, 1.0f ); glVertex2f(  1.0f, -1.0f);
      // Top Right Of The Texture and Quad
      glTexCoord2f( 1.0f, 0.0f ); glVertex2f(  1.0f,  1.0f);
      // Top Left Of The Texture and Quad
      glTexCoord2f( 0.0f, 0.0f ); glVertex2f( -1.0f,  1.0f);
    glEnd( );

    // l'emphase
    if ( emphase_text_id ) {

    	// enabled blending
    	glEnable(GL_BLEND);

	// Move Into The Screen - légèrement devant l'image
	glLoadIdentity();
	GLfloat z = position[Z] - (GLfloat)(zoom * 0.05f) - 0.0001f;
 	glTranslatef( 0,0, -z);

	glBindTexture( GL_TEXTURE_2D, emphase_text_id );
	glBlendFunc(GL_ONE, GL_ONE);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// pour compenser la texture aux dimensions GLWidth x GLHeight
	GLfloat margeHoriz, margeVert;
	margeHoriz = (GLfloat)(current_fichier->getGLWidth() - current_fichier->getWidth())  / (GLfloat)current_fichier->getGLWidth();
	margeVert  = (GLfloat)(current_fichier->getGLHeight() - current_fichier->getHeight()) / (GLfloat)current_fichier->getGLHeight();

	//cout << "Marge : " << margeHoriz << " x " << margeVert << endl;

	glBegin(GL_QUADS);
	glTexCoord2f( 0.0f, 1.0f ); glVertex2f( -1.0f+margeHoriz, -1.0f+margeVert);
	glTexCoord2f( 1.0f, 1.0f ); glVertex2f(  1.0f-margeHoriz, -1.0f+margeVert);
	glTexCoord2f( 1.0f, 0.0f ); glVertex2f(  1.0f-margeHoriz,  1.0f-margeVert);
	glTexCoord2f( 0.0f, 0.0f ); glVertex2f( -1.0f+margeHoriz,  1.0f-margeVert);
	glEnd( );

    	glDisable(GL_BLEND);

    } // */

    if ( showComment && comment_text_id[0] && comment_text_id[1] ) {
    	glEnable(GL_BLEND);
    	
	//glDisable(GL_ALPHA_TEST);
	//glAlphaFunc(GL_ALWAYS, 0.0f);

	// Move Into The Screen - légèrement devant l'image
	glLoadIdentity();
	GLfloat z = position[Z] - (GLfloat)(zoom * 0.05f) - 0.0001f;
 	glTranslatef( 0,0, -z);
/*
	// Texture du mask
	glBindTexture( GL_TEXTURE_2D, comment_text_id[0] );
	glBlendFunc(GL_DST_COLOR, GL_ZERO);

	glBegin(GL_QUADS);
	glTexCoord2f( 0.0f, 1.0f ); glVertex2f( -0.5f, -1.0f);
	glTexCoord2f( 1.0f, 1.0f ); glVertex2f(  0.5f, -1.0f);
	glTexCoord2f( 1.0f, 0.0f ); glVertex2f(  0.5f, -0.5f);
	glTexCoord2f( 0.0f, 0.0f ); glVertex2f( -0.5f, -0.5f);
	glEnd( );
// */	
	
	// Texture du texte
	glBindTexture( GL_TEXTURE_2D, comment_text_id[1] );
	//glBlendFunc(GL_SRC_COLOR, GL_DST_COLOR);
	glBlendFunc(GL_ONE, GL_ONE);
	
	GLfloat largeur = (GLfloat)comment_text_w;
	GLfloat hauteur = (GLfloat)comment_text_h;
	GLfloat position;
	
	if (fullscreen) { // TODO : changer çà !
		largeur /= (GLfloat)desktop_w;
		hauteur = (hauteur / (GLfloat)desktop_h) / ratio;
		position = (( 1 - ((GLfloat)current_fichier->getHeight() / (GLfloat)current_fichier->getGLHeight()) ) )  *  ratio ;
	}else{
		largeur /= (GLfloat)current_w;
		hauteur = (hauteur / (GLfloat)current_h) / ratio;
		
		/////largeur = (GLfloat)current_fichier->getWidth();
		/////hauteur = (GLfloat)current_fichier->getHeight();
		position = (( 1 - ((GLfloat)current_fichier->getHeight() / (GLfloat)current_fichier->getGLHeight()) ) )  *  ratio ;
	}	
	
//*
	glBegin(GL_QUADS);
	glTexCoord2f( 0.0f, 1.0f ); glVertex2f( -largeur, position -1.0f);
	glTexCoord2f( 1.0f, 1.0f ); glVertex2f(  largeur, position -1.0f);
	glTexCoord2f( 1.0f, 0.0f ); glVertex2f(  largeur, hauteur + position -1.0f);
	glTexCoord2f( 0.0f, 0.0f ); glVertex2f( -largeur, hauteur + position -1.0f);
	glEnd( );
// */	
    	glDisable(GL_BLEND);
    }

    /* Draw it to the screen */
    SDL_GL_SwapBuffers( );

    if ( fullscreen ) {
	SDL_ShowCursor(SDL_DISABLE);
    }

    return false;
}

// refreshing the screen
bool Video::refresh() {
	return drawGLScene();
}


// pixel in the window  to  pixel in the picture
int Video::PosPixelPicX(int x) {

	GLfloat ratioExt = (GLfloat)current_fichier->getHeight() / (GLfloat)(screen->h==0?1:screen->h);

	int middle = screen->w / 2;

	int xShown = (int)(current_fichier->getWidth() / ratioExt) / 2;

	if ( ( x < (middle - xShown)) || ( x >= (middle + xShown)) ) return -1;

	x = x - ( middle - xShown );

	return (x * current_fichier->getWidth()) / (xShown*2);
}

// pixel in the window  to  pixel in the picture
int Video::PosPixelPicY(int y) {
	return y * current_fichier->getHeight() / screen->h ; 
}

// Center the window
bool Video::CenterWindow() {
	int	x, y;
	SDL_SysWMinfo info;

	
	SDL_VERSION(&info.version);
	if (isX11 && (SDL_GetWMInfo(&info) > 0)) {
		info.info.x11.lock_func();
		x = (desktop_w - screen->w) / 2;
		y = (desktop_h - screen->h) / 2;
		if (x + screen->w > desktop_w) x = 0;
		if (y + screen->h > desktop_h) y = 0;
		if (x >= 0 && y >= 0)
			XMoveWindow(info.info.x11.display, info.info.x11.wmwindow, x, y);
		info.info.x11.unlock_func();
		return true;
	}
	
	return false;
}

// sauvegarde la taille courante de la fenetre
bool Video::saveWindowSize() {
	current_w = screen->w;
	current_h = screen->h;
	return false;
}

// resize Window
bool Video::resizeWindow( int width, int height) {
	return resizeWindow( width, height, current_bpp);
}

// resize Window
bool Video::resizeWindow( int width, int height, int bpp) {
	//cout << "  resize Window to " << width << "x" << height << " " << bpp <<"bpp" << endl;

	screen = SDL_SetVideoMode( width, height, bpp, videoFlags );
	if (!screen) {
		cerr << "Window not resizable !!" << endl;
		return true;
	}

	CenterWindow();
	initGL();
	resizeViewport(width, height);

	if ( fullscreen ) {
		if (isX11) {
			SDL_WM_ToggleFullScreen(screen); // Only on X11
		}
	}
	
	return false;
}

// function to reset viewport after a window resize 
bool Video::resizeViewport( int width, int height ) {
	
	if ( current_fichier ) {
		// on corrige la déformation induite par la texture agrandie pour l'openGL
		GLfloat ratioWindow, ratioGL;
		
		ratioWindow = (GLfloat)width / (GLfloat)(height==0?1:height);
		ratioGL	= (GLfloat)current_fichier->getGLWidth() / (GLfloat)current_fichier->getGLHeight();
		
		ratio = ratioWindow / ratioGL;
	}else{
		// ne devrait jamais passer par ici
		cerr << "Chemin non correct" << endl;
		ratio = 1; //( GLfloat )width / ( GLfloat )(height==0?1:height);
	}

	// Setup our viewport
	glViewport( 0, 0, ( GLint )width, ( GLint )height );

	// Change to the projection matrix and set our viewing volume
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );

	// Set our perspective
	gluPerspective( 45.0f, ratio, 0.1f, 100.0f );

	// Make sure we're chaning the model view and not the projection
	glMatrixMode( GL_MODELVIEW );

	// Reset The View
	glLoadIdentity( );

	return false;
}


/* general OpenGL initialization function */
bool Video::initGL( ){

    // Enable Texture Mapping
    glEnable( GL_TEXTURE_2D );

    // Enable smooth shading
    glShadeModel( GL_SMOOTH );

    // remove back face
    glEnable(GL_CULL_FACE); 

    // Set the background black
	//glClearColor( 0.0f, 0.5f, 0.0f, 0.5f );
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

    // Depth buffer setup
    //glClearDepth( 1.0f );

    // Enables Depth Testing
    glDisable( GL_DEPTH_TEST );

    // The Type Of Depth Test To Do
    //glDepthFunc( GL_LEQUAL );

    // Really Nice Calculations
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );


    return false;
}

// Switch between fullscreen mode and normal mode
bool Video::toggleFullscreen() {
	position[X] = 0.0f;
	position[Y] = 0.0f;
	fullscreen = ! fullscreen;
	if (isX11) { 
		if (fullscreen) {
			saveWindowSize();
		        resizeWindow(desktop_w, desktop_h);
			SDL_WM_ToggleFullScreen(screen); // Only on X11
		}else{
			SDL_WM_ToggleFullScreen(screen); // Only on X11
		        resizeWindow(current_w, current_h);
		}
	}
	SDL_ShowCursor(SDL_DISABLE);

	return fullscreen;
}

// Show or Unshow comments
bool Video::toggleComment(bool cmt) {
	//cout << "toggle comment" << endl;
       	//viewImage(*current_fichier); 
	showComment = cmt;
	refresh();
	return showComment;
}
bool Video::toggleComment() {
	return toggleComment( !showComment);
}

// gestion du zoom
bool Video::Zoom() {
	zoom ++;
	refresh();
	return false;
}

bool Video::deZoom() {
	zoom --;
	refresh();
	return false;
}

// déplacement de la caméra
bool Video::Vertical(int sens) {
	// Verify the limit of the border of the picture of the
	// fonction(zoom)
	position[X] += (GRANULARITE * sens);
	refresh();
	return false;
}
bool Video::Horizontal(int sens) {
	// Verify the limit of the border of the picture of the
	// fonction(zoom)
	position[Y] += (GRANULARITE * sens);
	refresh();
	return false;
}


bool Video::noZoomNCenter() {
	position[X] = 0.0f;
	position[Y] = 0.0f;
	zoom = 0;
	refresh();
	return false;
}
// Constructor
Video::Video() {
	SDL_SysWMinfo info;
	int x11screen;

	//cout << "Init Video ";

	img_text_id	= 0;
	comment_text_id[0]	= 0;
	comment_text_id[1]	= 0;
	comment_text_w	= 0;
	comment_text_h	= 0;
	emphase_text_id	= 0;
	zoom		= 0;
	isX11 		= false;
	fullscreen	= false;
	showComment	= true;
	ratio		= 1.0f;
	position[X] = 0.0f;
	position[Y] = 0.0f;
	position[Z] = 1 + RACINE_DE_2;

	/* startup SDL */
	if(SDL_Init( SDL_INIT_VIDEO ) < 0) {
		cerr << "SDL_Init_Video: " << SDL_GetError() << endl;
	}

	// Get Display-Desktop infos-size
	SDL_VERSION(&info.version);
	if (SDL_GetWMInfo(&info) > 0) {
		if (info.subsystem == SDL_SYSWM_X11) isX11 = true;
	}
	if (isX11) {
		info.info.x11.lock_func();
		x11screen = DefaultScreen(info.info.x11.display);
		desktop_w = DisplayWidth (info.info.x11.display, x11screen);
		desktop_h = DisplayHeight(info.info.x11.display, x11screen);

		//cout << "X11 screen is " << desktop_w << "x" << desktop_h << endl;

		info.info.x11.unlock_func();
	}else{
		cerr << "Video:unknown display" << endl;
	}

	/* Fetch the video capabilities info */
	videoInfo = SDL_GetVideoInfo( );
	if ( !videoInfo ){
		cerr << "Video query failed: " << SDL_GetError() << endl;
	}
	
	/* the flags to pass to SDL_SetVideoMode */
	videoFlags  = SDL_OPENGL;          /* Enable OpenGL in SDL */
	videoFlags |= SDL_GL_DOUBLEBUFFER; /* Enable double buffering */
	videoFlags |= SDL_HWPALETTE;       /* Store the palette in hardware */
	videoFlags |= SDL_RESIZABLE;       /* Enable window resizing */
	videoFlags |= SDL_NOFRAME;         /* Pas de bordures à la fenêtre */

	/* This checks to see if surfaces can be stored in memory */
	if ( videoInfo->hw_available )
	       	videoFlags |= SDL_HWSURFACE; 
	else 
		videoFlags |= SDL_SWSURFACE;

	/* This checks if hardware blits can be done */
	if ( videoInfo->blit_hw )
		videoFlags |= SDL_HWACCEL;

	
	/* Sets up OpenGL Attribute (double buffering, etc ) */
	SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
	SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
	SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
	
	/* font true type pour les commentaires */
	if (TTF_Init() == -1) {
		cerr << "erreur d'initialisation TTF " << TTF_GetError() << endl;
	}else{
		font = TTF_OpenFont(FONT_FILE, FONT_SIZE);
		if (!font) {
			cerr << "erreur d'ouverture de la fonte TTF " << TTF_GetError() << endl;
		}
	}
	
}

// Destructor
Video::~Video() {
	//cout << "Done Video" << endl;
	TTF_Quit();
}



