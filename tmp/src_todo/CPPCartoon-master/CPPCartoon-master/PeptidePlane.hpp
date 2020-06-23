
struct PeptidePlane {
    const residue *Residue1;
    const residue *Residue2;
    const residue *Residue3;
    v3 Position;
    v3 Normal;
    v3 Forward;
    v3 Side;
    bool Flipped;
};

PeptidePlane NewPeptidePlane(const residue &r1, const residue &r2, const residue &r3){
    PeptidePlane newPP;

    atom *CA1 = getAtom(r1, (char *)"CA");
    atom *O1 = getAtom(r1, (char *)"O");
    atom *CA2  = getAtom(r2, (char *)"CA");

    if(CA1 == NULL || O1 == NULL || CA2 == NULL){
        std::cerr<<"Failed to get all the atoms for residue "<<r1.id<<std::endl;
        return newPP;
    }

    v3 ca1 = CA1->coor;
    v3 o1 = O1->coor;
    v3 ca2 = CA2->coor;

    v3 a = (ca2 - ca1).normalized();
    v3 b = (o1 - ca1).normalized();
    v3 c = v3::crossProduct(a, b).normalized();
    v3 d = v3::crossProduct(c, a).normalized();
    v3 p = (ca1 + ca2)/ 2.0f;
    newPP.Residue1 = &r1;
    newPP.Residue2 = &r2;
    newPP.Residue3 = &r3;
    newPP.Position = p;
    newPP.Normal = c;
    newPP.Forward = a;
    newPP.Side = d;
    newPP.Flipped = false;

    return newPP;
}
PeptidePlane NewPeptidePlane(const float *r1CA,const  float *r1O,const  float *r2CA,
                                char ssr1, char ssr2, char ssr3,
                                int idr1, int idr2, int idr3){
    PeptidePlane newPP;

    // if(CA1 == NULL || O1 == NULL || CA2 == NULL){
    //     std::cerr<<"Failed to get all the atoms for residue "<<r1.id<<std::endl;
    //     return newPP;
    // }

    residue *r1 = (residue *)calloc(1, sizeof(residue));
    r1->id = idr1;
    r1->idx = idr1++;
    r1->atoms = NULL;
    r1->size = 0;
    r1->__capacity = 0;
    r1->next = NULL;
    r1->ss = ssr1;

    residue *r2 = (residue *)calloc(1, sizeof(residue));
    r2->id = idr2;
    r2->idx = idr2++;
    r2->atoms = NULL;
    r2->size = 0;
    r2->__capacity = 0;
    r2->next = NULL;
    r2->ss = ssr2;

    residue *r3 = (residue *)calloc(1, sizeof(residue));
    r3->id = idr3;
    r3->idx = idr3++;
    r3->atoms = NULL;
    r3->size = 0;
    r3->__capacity = 0;
    r3->next = NULL;
    r3->ss = ssr3;


    v3 ca1 = v3(r1CA[0], r1CA[1], r1CA[2]);
    v3 o1 = v3(r1O[0], r1O[1], r1O[2]);
    v3 ca2 = v3(r2CA[0], r2CA[1], r2CA[2]);

    v3 a = (ca2 - ca1).normalized();
    v3 b = (o1 - ca1).normalized();
    v3 c = v3::crossProduct(a, b).normalized();
    v3 d = v3::crossProduct(c, a).normalized();
    v3 p = (ca1 + ca2)/ 2.0f;

    newPP.Residue1 = r1;
    newPP.Residue2 = r2;
    newPP.Residue3 = r3;
    newPP.Position = p;
    newPP.Normal = c;
    newPP.Forward = a;
    newPP.Side = d;
    newPP.Flipped = false;

    return newPP;
}

void Transition(const PeptidePlane &pp, char &type1, char &type2) {

    char t1 = pp.Residue1->ss;
    char t2 = pp.Residue2->ss;
    char t3 = pp.Residue3->ss;
    type1 = t2;
    type2 = t2;
    if (t2 > t1 && t2 == t3){
        type1 = t1;
    }
    if (t2 > t3 && t1 == t2){
        type2 = t3;
    }
}

void Flip(PeptidePlane &pp) {
    pp.Side = pp.Side * -1;
    pp.Normal = pp.Normal * -1;
    pp.Flipped = !pp.Flipped;
}
