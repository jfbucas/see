
#include "fichier.h"

			
// Contructeur
Fichier::Fichier(const string n, const string dir) 
	: nom(n), commentaire() {
	directory = dir;
	visible = true;
	image = NULL;
	w = 0;
	h = 0;

	emphaseFileName = directory + "/." + nom + ".emphase.txt";
	

};

// Destructeur
Fichier::~Fichier() {
	if (image  ) SDL_FreeSurface(image);
}

// charge l'image en mémoire
bool Fichier::loadImg() {

	// on sait déjà que l'image n'est pas visible
	if (! visible) return false;
	
	// si l'image n'est pas déjà chargée
	if (!image) {
		string location = directory + "/" + nom;
		
		// appel de la fonction SDL
		//cout << " load image " << nom << " ";
		image = IMG_Load(location.c_str());
		
		if (!image) {
			//cout << " error"  << endl;
			visible = false;
			return false;
		}
		//cout << " success"  << endl;
		
		w = image->w;
		h = image->h;

		GLw=1; while (GLw < w) GLw = GLw << 1;
		GLh=1; while (GLh < h) GLh = GLh << 1;

		cout << nom << " is " << w << "x" << h << " - " << commentaire << endl;

		loadEmphase();
	}
	
	return true;
		
}

// libération de l'image préchargée
bool Fichier::freeImg() {
	
	if ( (visible) && ( image ) )  {
		//cout << " free image " << nom << endl;
		SDL_FreeSurface(image);
		image = NULL;
		return true;
	}
	
	return false;
}

// Value of the emphase
int Fichier::emphaseValue() {
	int result = 0;
	for(int i=0; i < EMPHASE_W; i++) 
		for(int j=0; j < EMPHASE_H; j++) 
			result += 1 << emphase[i][j];

	return result;
}

// Load emphase file if exists
bool Fichier::loadEmphase() {
	
	for(int i=0; i < EMPHASE_W; i++) 
		for(int j=0; j < EMPHASE_H; j++) 
			emphase[i][j] = 0;

	ifstream emphile;
	emphile.open( emphaseFileName.c_str());
	if (emphile == 0) {
		return false;
	}

	char action;
	int value1, value2;

	do {
		emphile >> action;
		emphile >> value1;
		emphile >> value2;

		switch ( action ) {
			case '-': emphase[value1][value2] --; break;
			case '+': emphase[value1][value2] ++; break;
			case 'C':
				for(int i=0; i < EMPHASE_W; i++) 
					for(int j=0; j < EMPHASE_H; j++) 
						emphase[i][j] = value1;
				break;
		}
	} while ( ! emphile.eof() );

	return true;
}
// Clear the emphase
void Fichier::emphaseClear(int value) {
	for(int i=0; i < EMPHASE_W; i++) 
		for(int j=0; j < EMPHASE_H; j++) 
			emphase[i][j] = value;

	ofstream emphile;
	emphile.open( emphaseFileName.c_str(), ios_base::app);
	if ( emphile == 0 ) {
		cerr << "impossible de sauvegarder l'emphase" << endl;
		return;
	}
	emphile << "C " << value << " " << 0 << endl; 
	emphile.close();

	return;
}

// Add an emphase
void Fichier::emphasePlus(int x, int y) {
	
	int px =  ((x * EMPHASE_W) / w);
	int py =  ((y * EMPHASE_H) / h);

	emphase[px][py] ++;
	
	ofstream emphile;
	emphile.open( emphaseFileName.c_str(), ios_base::app);
	if ( emphile == 0 ) {
		cerr << "impossible de sauvegarder l'emphase" << endl;
		return;
	}
	emphile << "+ " <<  px << " " << py << endl; 
	emphile.close();

	return;
}

// Remove an emphase
void Fichier::emphaseMoins(int x, int y) {

	int px =  ((x * EMPHASE_W) / w);
	int py =  ((y * EMPHASE_H) / h);

	emphase[px][py] --;
	
	ofstream emphile;
	emphile.open( emphaseFileName.c_str(), ios_base::app);
	if ( emphile == 0 ) {
		cerr << "impossible de sauvegarder l'emphase" << endl;
		return;
	}
	emphile << "- " <<  px << " " << py << endl; 
	emphile.close();

	return;
}


// show info
void Fichier::afficher(ostream & flux, const Fichier & f) {
	flux << f.getNom() << " - " << f.getCommentaire();
}

// -------- Methodes auxilliaires

// Affichage d'un fichier avec <<
ostream & operator << (ostream & flux, const Fichier & f) {
	Fichier::afficher(flux, f);
	return flux;
}

/*
	cout << "    emphase " << px << ", " << py << endl;
	for (int j=0; j < EMPHASE_H; j++) {
		for(int i=0; i < EMPHASE_W; i++) {
			cout << emphase[i][j] << " ";
		}
		cout << endl;
	}
*/
