#include <vector>
#include "cpdb/cpdb.h"

struct Color32{
    char r;
    char g;
    char b;
    char a;
};

struct Segment{
    std::vector<residue *> residues;
};


struct CartoonSizeCoords
{
    float *widths;
    float *heights;
    v3 *coords;
    v3 *vects;
};

struct SplineStruct
{
    int scale;
    v3 *pts;
    v3 *prps;
    Color32 *cols;
    float *interpwidths;
    float *interpheights;
};

// Mesh *computeCartoonMesh(int nbChain, int *nbResPerChain, float *CA_OPositions, int *ssTypePerRes);
vector<Mesh> computeCartoonMesh(int nbChain, int *nbResPerChain, float *CA_OPositions, char *ssTypePerRes);


