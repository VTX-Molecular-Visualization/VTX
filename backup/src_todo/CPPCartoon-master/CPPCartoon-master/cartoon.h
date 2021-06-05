#include <map>

const int splineSteps = 32/2;
const int profileDetail = 16/2;

const float ribbonWidth = 2.0f;
const float ribbonHeight = 0.125f;
const float ribbonOffset = 1.5f;
const float arrowHeadWidth = 3.0f;
const float arrowWidth = 2.0f;
const float arrowHeight = 0.5f;
const float tubeSize = 0.25f;


#ifndef M_PI
   #define M_PI 3.14159265358979323846
#endif

struct cmpFloat3 {
    bool operator()(const v3 &a, const v3 &b) const {
        if      (a.x <= b.x && a.y <= b.y && a.z < b.z) return true;
        else if (a.x <= b.x && a.y < b.y) return true;
        else if (a.x < b.x) return true;
        else return false;
    }
};

struct Mesh
{
    std::vector<v3> vertices;
    std::vector<v3> colors;
    std::vector<int> triangles;
    std::map<v3, int, cmpFloat3> verticesDict;
};

Mesh createChainMesh(const chain &C);
Mesh createChainMesh(int chainId, const int *nbResPerChain, const float *CA_OPositions, const char *ssTypePerRes);