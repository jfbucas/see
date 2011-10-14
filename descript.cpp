
#include "descript.h"

// upcase std
const string upcase(const string &st) {
	
	string ret="";
	unsigned int i=0,nb = st.size();

	while(i<nb) {
		if(st[i]>='a' && st[i]<='z')
			ret += (st[i] & 0xDF);
		else
			ret += st[i];
		i++;
	}
	return(ret);
}

// récupère le commentaire du nom dans la liste
const string Description::getCommentaire(const string nom) {
	string NOM = upcase(nom);
	
	list<Line>::iterator c = liste_description->begin();
	while (	(c != liste_description->end()) &&
		!(upcase((*c).getNom()) == NOM) ) {
		c++;
	}

	if (c == liste_description->end()) { 
		//cerr << "warning : " << nom << " not found." << endl;
		return "";
	}

	return (*c).getDescription();
}

// est ce qu'on a des descriptions ?
bool Description::isDescription() {
	return ! liste_description->empty();
}	

// charge le fichier de description
bool Description::Load() {
	#define MAX_BUFFER MAX_COMMENT + MAX_NAME + 5
	char buffer[MAX_BUFFER];
	string name = dir+"/"+DESCRIPTION_NAME;
	ifstream file;
	int i;

	file.open(name.c_str());
	if (file == 0) {	
		//cout << "pas de description "<< endl;
		return false;
	}
	
	do {
		// on lit la ligne
		file.getline(buffer, MAX_BUFFER, '\n') ;

		if (buffer[0] != 0){
			// on enleve les traillings  0D 0A de ms-DOS
			i=0;
			while (buffer[i] != 0) i++;
			i--;
			while (buffer[i] == 0x0D || buffer[i] == 0x0A) {
				buffer[i] = 0;
				i--;
			}
			
			// on cherche la coupure
			i=0;
			while (buffer[i]!=' ' && buffer[i]!=0 ) i++;
				
			// put it in the liste
			string nom = string(buffer, 0, i);
			string commentaire = string(&buffer[i+1]);
			//cout << nom << " - " << commentaire << endl;
			liste_description->push_back(Line(nom, commentaire));
		}
			
	} while ( ! file.eof() );
	
	file.close();

	return true;
}	

// Sauvegarde des commentaires
bool Description::Save() {
	string name = dir+"/"+DESCRIPTION_NAME;
	ofstream file;
	
	file.open(name.c_str());
	if (file == 0) {	
		cerr << "impossible de créer le fichier de description "<< endl;
		return false;
	}

	list<Line>::iterator l = liste_description->begin();
	while ( l != liste_description->end()) {
		file << (*l).getNom() << " " << (*l).getDescription() << endl;
		l++;
	}
	
	file.close();
	
	return true;
}


// met à jour la nouvelle valeur du commentaire
bool Description::UpdateList(const string & nom, const string & description) {
	string NOM = upcase(nom);
	
	bool trouve = false;
	list<Line>::iterator l = liste_description->begin();
	while ( (l != liste_description->end()) && !trouve) {
		if (upcase((*l).getNom()) == NOM) {
			trouve = true;
			(*l).setDescription(description);
		}
		l++;
	}

	// si le fichier n'est pas dans la liste on le rajoute
	if ( ! trouve ) 
		liste_description->push_back(Line(nom, description));

	
	return trouve;
}
