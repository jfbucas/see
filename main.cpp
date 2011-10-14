/*
 * 
 *  Voir des images et les retourner simplement avec SDL et openGL
 *  
 */

#include <libgen.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <utime.h>

#include "core.h"

int help(int argc, char ** argv) {
	
	cout << "Usage : " << argv[0] << " [directory|picture_name] ";

	if ( strncmp( argv[1], "-h", 2 ) == 0) {
		cout << endl;
		cout //<< "	-h	Help" << endl
		     << "	-k	Show available Keys" << endl
		     << "	-f	Start fullscreen(TODO)" << endl
		     << "	-c	Disable comment(TODO)" << endl
		     << "	-o	Show only files with comments" << endl
		     << "	-v	verbosity(TODO)" << endl;
		
	} else if ( strncmp( argv[1], "-k", 2 ) == 0) {
		cout << endl;
		cout << "Normal mode : "		 << endl
		     <<	"	Space,Wheel Down	Next" << endl
		     <<	"	Shift-Space,Wheel Up	Previous" << endl
		     << "	Home			First" << endl
		     << "	End			Last" << endl
		     << "	F1,f			Toggle Fullscreen" << endl
		     << "	c			Toggle Comments" << endl
		     << "	e,i			Enter comment edit mode" << endl
		     << "	z,+/Z,-			zoom/unZoom" << endl
		     << "	Arrows			Move" << endl
		     << "	*			Reinit(position,zoom)" << endl
		     << "	Esc,q			Quit" << endl << endl;

		cout << "Comment Edit mode : "		 << endl
		     << "	Enter			Save comment" << endl
		     << "	Esc			Leave edit mode without saving" << endl;
	} else {
		cout << "(-h  for help)" << endl;
	}

	cout << endl;

	return 0;
}

int main(int argc, char **argv) {
	
	bool result = false;
	Core * coeur = NULL;
	
	if (argc > 2) {
		// too much params
		cerr << argv[0] << " needs only one argument" << endl;
	}
	
	if (argc == 2) {
		// is the parameter a file or a directory ?
		struct stat sbuf;
		int type = 0;
		
		if (stat(argv[1], &sbuf) != -1) {
			type = sbuf.st_mode & S_IFMT;
		}
		
		switch ( type ) {
		case S_IFREG :	// File
			char *dirc, *basec, *bname, *dname;
				
			dirc  = strdup(argv[1]);
			basec = strdup(argv[1]);
			dname = dirname(dirc);
			bname = basename(basec);
	      
			coeur = new Core( string(dname), string(bname) );
		break;
		case S_IFDIR:	// Directory
			while (argv[1][strlen(argv[1])-1] == '/')
				argv[1][strlen(argv[1])-1] = 0; // remove trailing '/'

			coeur = new Core( string(argv[1]) );
		break;
		default: 
			help(argc, argv);
		break;
		}
	} else {
		// no parameter
		coeur = new Core();
	}

	// Internal parameter
	if (strcmp(argv[0], "see.comment") == 0) {
		coeur->setShowCommentedFilesOnly(true);
	}

	// Internal parameter
	if (strcmp(argv[0], "see.emphase") == 0) {
		//coeur->setShowCommentedFilesOnly(true);
		coeur->setEmphasis(true);
	}
	// Internal parameter
	if (strcmp(argv[0], "see.tablet") == 0) {
		coeur->setNextByClick(true);
	}
	
	// Lancement du Core	
	if (coeur && !coeur->isFinished()) {
		result = coeur->run();
		delete coeur;
	}
	
	if (result) return -1; else return 0;
}
