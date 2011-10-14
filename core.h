#ifndef __CORE_H__
#define __CORE_H__


#include <string>
#include "dir.h"
#include "video.h"

#define SECONDE *1000
#define TIMER_EVENT 0

using namespace std;

class Core {
	private:
		bool done;
		Dir * dirList; 
		Video * video;
		list<Fichier>::iterator current;
		bool reload;

		SDL_TimerID id_timer;
		int timer_counter;
		int cursor_mouse_timer;

		string desiredFilename;
		bool showCommentedFilesOnly;
		bool emphase_mode;
		bool next_by_click;

		// Edition
		bool edit_mode;
		string edited_comment;
		bool cursor_keyboard;
		int cursor_position;

		bool Rotate(int angle);
		
		// Fonctions pour l'édition des commentaires
		bool enter_edit_mode();
		bool leave_edit_mode(bool save);
		
		// Gestion des évenements
		bool handle_keyboard( const SDL_KeyboardEvent & event);
		bool handle_mouse( const SDL_MouseButtonEvent & event);
		bool handle_mouse_motion( const SDL_MouseMotionEvent & event);
		bool handle_userevent();
		static Uint32 handle_timer(Uint32 _val, void* tptr);

	public:
		Core(const string dir= ".", const string filename= "");
		~Core();
		
		bool run();
		void setShowCommentedFilesOnly(bool b) { showCommentedFilesOnly = b; }
		void setEmphasis(bool b) { emphase_mode = b; }
		void setNextByClick(bool b) { next_by_click = b; }
		bool isFinished() { return done; }
};


#endif
