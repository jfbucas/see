#ifndef __DESCRIPTION__
#define __DESCRIPTION__

#include <string>
#include <iostream>
#include <fstream>
#include <list>

#include "fichier.h"

#define DESCRIPTION_NAME "descript.ion"
#define MAX_COMMENT 500
#define MAX_NAME 50

using namespace std;

class Description {
	private :
		class Line {
			private:
				string nom;
				string description;
			public:
				Line(const string n, const string d) : nom(n), description(d) {}
				const string & getNom() { return nom; }
				const string & getDescription() { return description; }
				void setNom(const string n) { nom = n; }
				void setDescription(const string d) { description = d; }
		};
		
		string dir;
		list<Line> * liste_description;
		
	public  :
		// Constructeurs
		Description(const string d) : dir(d) {
		       		liste_description = new list<Line>();
				Load();
		       	};
		~Description() {
				liste_description->clear();
				delete liste_description;
			};	
		
		// Accesseurs
		void setDir(const string d) { dir = d; }
		const string getCommentaire(const string nom);

		// est ce que le fichier existe ?
		bool isDescription();
		
		// Charge les descriptions
		bool Load();
		
		// Sauvegarde les changements
		bool Save();

		// Met à jour la liste
		bool UpdateList(const string & nom, const string & description);

};

#endif

 
