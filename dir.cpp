
#include "dir.h"

//
// Construire la liste
// 

Dir::Dir(const std::string dir) {
	//cout << "Init Dir is " << dir << endl;
	//std::cout << "In directory: " << full_path.native_directory_string() << endl;
	
	directory = "";
	date = std::time_t();

	fs::path full_path( dir ); // dir.c_str() );
	
	liste = new list<Fichier>();
	
	
	if ( ! fs::is_directory( full_path ) ){
		std::cerr << dir << " is not a directory" << std::endl;
		return;
	}


	directory = dir;
	
	// chargement des descriptions
	description = new Description(dir);
	
	// création de la liste des fichiers
	SetListe(full_path);
	
}

// Destruction
Dir::~Dir() {
	liste->clear();
	delete liste;
	delete description;
	//cout << "Done Dir" << endl;
}



// Construit la liste des fichiers du répertoire
// 
void Dir::SetListe(fs::path & full_path) {

	nb_fichier = 0;
	liste->clear();
	fs::directory_iterator end_iter;
	for ( fs::directory_iterator dir_itr( full_path );
		dir_itr != end_iter;
		++dir_itr ){
		
		try{
			if ( fs::is_directory( *dir_itr ) ){
				// std::cout << dir_itr->leaf()<< " [directory]" << std::endl;
			}else{
				++nb_fichier;
				// std::cout << dir_itr->leaf() << std::endl;
				//Fichier * fichier = new Fichier(dir_itr->leaf(),directory);
				//fichier->setCommentaire(description->getCommentaire(*fichier));
				//liste->push_back(*fichier);
				list<Fichier>::iterator c = liste->begin();
				while ( ( c != liste->end() ) && ( (*c).getNom() <= dir_itr->leaf() ) ) c++;
				liste->insert(c, Fichier(dir_itr->leaf(), directory));
				//liste->push_back(Fichier(dir_itr->leaf(),directory));
			}
		}
		catch ( const std::exception & ex ){
			std::cout << dir_itr->leaf() << " " << ex.what() << std::endl;
		}
	}
	date = fs::last_write_time( full_path );
	// std::cout << nb_fichier << " fichiers - le " << date <<std::endl;
	
			
	// Affiche la liste
	/*list<Fichier>::iterator c = liste->begin();
	while ( ( c != liste->end() ) ) {
		std::cout << (*c).getNom() << std::endl;
		c++;
	}*/


	// on met les commentaires si on les as
	if ( description->isDescription() ) {
		list<Fichier>::iterator c = liste->begin();
	
		while ( c != liste->end() ) {
			//cout << "[" << (*c).getNom() << "]";
			(*c).setCommentaire( description->getCommentaire((*c).getNom()) );
			c++;
		}
	}
}

// mise à jour de la liste
bool Dir::update() {
	
	fs::path full_path( directory );
	std::time_t current_date = fs::last_write_time( full_path );

	if ( date != current_date ) {
		SetListe(full_path);
		return true;
	}

	return false;
}

// Afficher la liste
void Dir::afficher() {

	if (nb_fichier == 0) {
		cerr << "Pas de fichiers" << endl;
		return;
	}
	
	list<Fichier>::const_iterator it;
	
	for(it=liste->begin(); it!=liste->end(); ++it){
		cout << *it << endl;
	} 
}

bool Dir::updateDescription(const Fichier & f) {
	if ( description ) {
		description->UpdateList(f.getNom(), f.getCommentaire());
		description->Save();
	}
	return true;
}

// Accesseurs
const list<Fichier>::iterator Dir::getBegin() { return liste->begin(); }
const list<Fichier>::iterator Dir::getFirst() {
	
	list<Fichier>::iterator c = liste->begin();
	
	while ( (c != liste->end()) && (! (*c).loadImg()) ) c++;

	return c; 
}
const list<Fichier>::iterator Dir::getNext(const list<Fichier>::iterator & from) { 
	list<Fichier>::iterator c = from;

	//if ( c != liste->end() ) {
		c++;
	
		while ( (c != liste->end()) && (! (*c).loadImg()) ) c++;

		if (c == liste->end() /*&& (! (*c).loadImg())*/ ) c = from;
	//}
	
	return c; 
}
const list<Fichier>::iterator Dir::getPrec(const list<Fichier>::iterator & from) { 
	list<Fichier>::iterator c = from;

	//if ( c != liste->begin() ) {
		c--;
	
		while ( (c != liste->begin()) && (! (*c).loadImg()) ) c--;
	
		if (c == liste->begin() /*&& (! (*c).loadImg())*/ ) c = from;
	//}
	
	return c;
}
const list<Fichier>::iterator Dir::getLast() { 
	list<Fichier>::iterator c = liste->end();

	while ( (c != liste->begin()) && (! (*c).loadImg()) ) c--;

	return c;
}
const list<Fichier>::iterator Dir::getEnd() { return liste->end(); }


list<Fichier>::iterator Dir::find_file(const std::string filename) {
	list<Fichier>::iterator c = liste->begin();
	
	while ( (c != liste->end()) && !((*c).getNom() == filename) ) c++;

	if (c == liste->end()) { 
		cerr << "warning : " << filename << " not found." << endl;
		c = getFirst(); 
	}

	if ( !(*c).loadImg()) return liste->end();

	return c;
}

const list<Fichier>::iterator Dir::getFirstCommented() {

	list<Fichier>::iterator c = liste->begin();
	bool done = false;

	while ( (c != liste->end()) && !done) {
		if ( !(*c).getCommentaire().empty()  ) {
			if ( (*c).loadImg() ) {
				done  = true;
			}
		}
		if (!done) c++;
	}

	return c;
}

const list<Fichier>::iterator Dir::getNextCommented(const list<Fichier>::iterator & from) { 

	list<Fichier>::iterator c = from;
	bool done = false;

	c++;

	while ( (c != liste->end()) && !done) {
		if ( !(*c).getCommentaire().empty()  ) {
			if ( (*c).loadImg() ) {
				done  = true;
			}
		}
		if (!done) c++;
	}

	return c;
}
