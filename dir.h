#ifndef __DIR__
#define __DIR__

#include <dirent.h>
#include <list>
#include <string>
#include <iostream>
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"

#include "fichier.h"
#include "descript.h"

//using namespace std;

namespace fs = boost::filesystem;

class Dir {
	private :
		std::string directory;	// le repertoire en cours
		std::time_t date;	// la date de modification du répertoire	
		int nb_fichier;		// le nombre de fichiers dans le repertoire
		list<Fichier> * liste;	// la liste des fichiers
		Description * description; // la liste des descriptions

		// Mise à jour de la liste
		void SetListe(fs::path & full_path);
	public  :
		// Constructeurs
		Dir(const std::string directory = ".");  // { Dir(directory.c_str()); }; 
		~Dir();
		
		// Accesseurs
		int getNbFichier() { return nb_fichier; }
		
		// mise à jour de la liste si nécessaire
		bool update();

		// trouve un fichier dans la liste
		list<Fichier>::iterator find_file(const std::string file);
		
		// fichier de commentaires ? ( descript.ion )
		bool isComment();		

		// met à jour la description du fichier
		bool updateDescription(const Fichier & f);

		// afficher
		void afficher();

		// Iterateur sur la liste
		const list<Fichier>::iterator getBegin();
		const list<Fichier>::iterator getFirst();
		const list<Fichier>::iterator getNext(const list<Fichier>::iterator & from);
		const list<Fichier>::iterator getPrec(const list<Fichier>::iterator & from);
		const list<Fichier>::iterator getLast();
		const list<Fichier>::iterator getEnd();
		const list<Fichier>::iterator getFirstCommented();
		const list<Fichier>::iterator getNextCommented(const list<Fichier>::iterator & from);
};


#endif

 
