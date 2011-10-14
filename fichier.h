#ifndef __FICHIER__
#define __FICHIER__

#include <string>
#include <iostream>
#include <fstream>
#include "SDL_image.h"

#define EMPHASE_W 32 //256
#define EMPHASE_H 32 //256

using namespace std;

class Fichier {
	private :
		string nom;
		string directory;	// ----> solution technique pour connaitre l'emplacement du fichier pour le chargement
		string commentaire;
		string emphaseFileName;
		bool visible;
		
		int w, h;
		int GLw, GLh;	// 2^n x 2^m : dimension of picture in puissance de 2 pour l'openGL

		SDL_Surface *image;

		int emphase[EMPHASE_W][EMPHASE_H];

		
	public  :
		// Constructeurs
		Fichier(const string n, const string dir);
		~Fichier();

		// Accesseurs
		const string getNom() const { return nom; }
		const string getFullNom() const { return directory+ "/" +nom; }
		const string getCommentaire() const { return commentaire; }
		bool isVisible() const { return visible; }
		SDL_Surface * getImage() const { return image; }
		int getBitsPerPixel() const { return image->format->BitsPerPixel; }
		int getHeight() const { return h; }
		int getWidth() const { return w; }
		int getGLHeight() const { return GLh; }
		int getGLWidth() const { return GLw; }
		int getEmphase(int x, int y) const { return emphase[x][y]; }


		void setNom(string n) { nom = n; }
		void setNom(char * n) { nom = string(n); }
		void setCommentaire(string c) { commentaire = c; }
		void setCommentaire(char * c) { commentaire = string(c); }
		void setVisible(bool b) { visible = b; }

		// Emphasis
		int emphaseValue();
		bool loadEmphase();
		void emphaseClear(int value);
		void emphasePlus(int x, int y);
		void emphaseMoins(int x, int y);

		// Charge l'image
		bool loadImg();
		// Libere l'image
		bool freeImg();
		
		// pour afficher avec l'operateur <<
		static void afficher(ostream & flux, const Fichier &f);
		
};

// fonction d'affichage
ostream & operator << (ostream & flux, const Fichier & f);
	
#endif

 
