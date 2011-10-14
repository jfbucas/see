#ifndef __VIDEO_H__
#define __VIDEO_H__

#include <string>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_syswm.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

//#include "font.h"
#include "fichier.h"

using namespace::std;

#define FONT_FILE "/usr/share/fonts/corefonts/arialbd.ttf" //"font.ttf"
#define FONT_SIZE 20
#define GRANULARITE 0.05

// Constantes
#define RACINE_DE_2 1.41421356237309504880168872420969807856967187f
#define X 0
#define Y 1
#define Z 2

class Video {
	private:
		// Configuration Video
		SDL_Surface * screen;		// the screen
		int videoFlags;			// Flags to pass to SDL_SetVideoMode
		bool isX11;
		const SDL_VideoInfo *videoInfo;	// this holds some info about our display

		int desktop_w, desktop_h, desktop_bpp; // attributs du fullscreen
		int current_w, current_h, current_bpp; // attributs du !fullscreen
		
		// Paramètres de visualisation
		GLfloat position[3];
		bool fullscreen;
		int zoom;
		GLfloat ratio;

		// Le fichier Image
		Fichier * current_fichier;	// fichier en cours de visualisation
		GLuint img_text_id;		// Id for the image

		// Les commentaires
		TTF_Font *font;			// the font
		bool showComment;
		GLuint comment_text_id[2];	// ID for the comment
		int comment_text_w, comment_text_h; // dimensions de la texture commentaire

		// Emphase
		GLuint emphase_text_id;
		
		
		// Initialize Open GL
		bool initGL();
		// Load the texture into video memory
		bool loadGLTextures(Fichier & fichier);
		// Draw the Scene
		bool drawGLScene( void );
		// Save position of window
		bool saveWindowSize();
		// resize the main window with change of bpp
		bool resizeWindow( int width, int height, int bpp );

	public:
		// Con et De structeurs
		Video();
		~Video();
		
		// view the image into the main window
		bool viewImage(Fichier & fichier);
		// génère la texture du commentaire
		bool generateGLComment(const string & commentaire);
		// génère la texture de l'emphase
		bool generateGLEmphase(Fichier & fichier);
		
		// resize the main window
		bool resizeWindow( int width, int height );
		// resize the view port OpenGL
		bool resizeViewport( int width, int height );

		// Zooming
		bool Zoom();
		bool deZoom();
		// Deplacing
		bool Vertical(int sens);
		bool Horizontal(int sens);
		
		bool noZoomNCenter();

		// refresh screen
		bool refresh();

		// Provide some interaction
		int PosPixelPicX(int x);
		int PosPixelPicY(int y);

		// toggle to fullscreen
		bool toggleFullscreen();
		// toggle comment
		bool toggleComment();
		bool toggleComment(bool cmt);

		// centering the window on the screen
		bool CenterWindow();
};

#endif
