
#include "dir.h"

int main(char * argc, char ** argv) {

	Dir d(".");
	d.afficher();
	if (d.update()) {
		d.afficher();
	}
}
