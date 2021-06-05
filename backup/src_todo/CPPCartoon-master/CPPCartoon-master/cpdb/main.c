//
//  main.c
//  cpdb
//
//  Created by Gokhan SELAMET on 28/09/2016.
//  Copyright © 2016 Gokhan SELAMET. All rights reserved.
//

#include <stdio.h>
#include "cpdb.h"

int main(int argc, const char * argv[]) {
	
	pdb *P;
	P = initPDB();
	parsePDB ("./test/1cqu.pdb", P, "");
	printPDB (P);
	freePDB (P);
    return 0;
	
}
