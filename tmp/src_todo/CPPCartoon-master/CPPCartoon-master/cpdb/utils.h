//
//  utils.c
//  cpdb
//
//  Created by Gokhan Selamet on 20/10/16.
//  Copyright © 2016 Gokhan SELAMET. All rights reserved.
//

#include <stdio.h>
#include "cpdb.h"

// v3 vMul(v3 p, float val){
// 	p.x*=val;
// 	p.y*=val;
// 	p.z*=val;
// 	return p;
// }

atom* getAtom (residue *resA, char *atomType) {
	atom *atomA;
	int i = 0;
	for (i=0; i<resA->size; i++) {
		atomA = &resA->atoms[i];
		if ( ! strncmp(atomA->type, atomType, 5) )
			return atomA;
	}
	return NULL;
}

float distanceAtom (atom *A, atom*B) {
	return v3::getDistance(A->coor, B->coor);
	// float dx,dy,dz;
	// dx = A->coor.x - B->coor.x;
	// dy = A->coor.y - B->coor.y;
	// dz = A->coor.z - B->coor.z;
	// return sqrtf(dx*dx + dy*dy + dz*dz);
}

/*
char isInteract (residue *A, residue *B, float limitDistance, int limitChainDistance) {
	if (B->id - A->id < limitChainDistance ) return 0;
	atom *I, *J;
	for (int i = 0; i<A->size; i++) {
		I = & A->atoms[i];
		for (int j=0; j<B->size; j++) {
			J = & B->atoms[j];
			if (distanceAtom(I,J) < limitDistance) 
				return 1;
		}
	}
	return 0;
}

int intraInteractionCount (chain *C, float distanceLimit, int limitChainDistance) {
	int counter = 0;
	residue *A, *B;
	for (int i=0; i<C->size; i++) {
		A = & C->residues[i];
		B = A;
		while (B) {
			if ( isInteract(A, B, distanceLimit, limitChainDistance) )
				counter += 1;
			B = B->next;
		}
	}
	return counter;
}
*/
