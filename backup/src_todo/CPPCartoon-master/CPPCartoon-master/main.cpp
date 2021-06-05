
#include <fstream>
#include <vector>

#include "cpdb/cpdb.h"
#include "cartoon.h"

#define USEOMP 1

#if USEOMP
#include <omp.h>
#endif

#include <chrono>
#include <iomanip>

using namespace std;

#define TIMEIT 1


/// Tool to create cartoon meshes for secondary structures of proteins
/// Can be used as a library by calling computeCartoonMesh or as a standalone tool by calling the executable on a PDB file
/// Secondary structure code : 0 = COIL / 1 = HELIX / 2 = STRAND

/// Example: computeCartoonMesh(numberOfChains, (int *)numberOfResiduePerChain, (float *)CA_O_Positions, (char *)ssTypePerResidue);

/// Utility function to write meshes to an OBJ file
void writeToObj(string fileName, vector<Mesh> &meshes) {

    FILE *fich = NULL;

    cerr << "Writting to " << fileName << endl;

    fich = fopen(fileName.c_str(), "w");
    if (fich == NULL)
    {
        printf("Error opening file  '%s'!\n", fileName.c_str());
        exit(1);
    }


    for (int i = 0; i < meshes.size(); i++) {
        vector<v3> verts = meshes[i].vertices;
        vector<v3> colors = meshes[i].colors;

        for (int j = 0; j < verts.size(); j++) {
            // myfile << "v "<<meshes[i].vertices[j].x<<" "<<meshes[i].vertices[j].y<<" "<<meshes[i].vertices[j].z<<endl;
            fprintf(fich, "v %.3f %.3f %.3f %.3f %.3f %.3f\n", verts[j].x, verts[j].y, verts[j].z, colors[j].x, colors[j].y, colors[j].z);
        }
    }
    int cpt = 1;
    for (int i = 0; i < meshes.size(); i++) {
        vector<int> tri = meshes[i].triangles;
        for (int j = 0; j < tri.size(); j += 3) {
            // myfile << "f "<<cpt+tri[j]<<" "<<cpt+tri[j+1]<<" "<<cpt+tri[j+2]<<endl;
            fprintf(fich, "f %d %d %d\n", cpt + tri[j], cpt + tri[j + 1], cpt + tri[j + 2] );
        }
        cpt += meshes[i].vertices.size();
    }

    fclose(fich);

    cerr << "Wrote " << meshes.size() << " meshes to " << fileName << endl;
}

//Assuming there are always 2 atoms per residue
vector<Mesh> computeCartoonMesh(int nbChain, int *nbResPerChain, float *CA_OPositions, char *ssTypePerRes) {
    vector<Mesh> meshes(nbChain);
    if (nbChain <= 0) {
        return meshes;
    }

#if USEOMP
    #pragma omp parallel for num_threads(nbChain)
    for (int chainId = 0; chainId < nbChain; chainId++) {
        Mesh m = createChainMesh(chainId, nbResPerChain, CA_OPositions, ssTypePerRes);
        meshes[omp_get_thread_num()] = m;
    }
#else
    for (int chainId = 0; chainId < nbChain; chainId++) {
        Mesh m = createChainMesh(chainId, nbResPerChain, CA_OPositions, ssTypePerRes);
        meshes[chainId] = m;
    }
#endif

    return meshes;
}


int main(int argc, char const *argv[]) {

    if (argc != 3) {
        std::cout << "Usage : " << argv[0] << " file.pdb output.obj" << endl;
        exit(-1);
    }

    string outputPath = argv[2];

    pdb *P;
    P = initPDB();

    parsePDB((char *)argv[1], P, (char *)"");

#if TIMEIT
    auto start = std::chrono::high_resolution_clock::now();
#endif

#if 0 //Previous implementation
    // vector<Mesh> meshes;
    vector<Mesh> meshes(P->size);
    #pragma omp parallel for num_threads(P->size)
    for (int chainId = 0; chainId < P->size; chainId++) {
        chain C = P->chains[chainId];

        Mesh m = createChainMesh(C);
        // meshes.push_back(m);
        meshes[omp_get_thread_num()] = m;
    }

#else
    //Get atom positions and call computeCartoonMesh
    vector<int> nbResPerChain(P->size);
    vector<float> CAOPos;
    vector<char> allSS;

    for (int chainId = 0; chainId < P->size; chainId++) {
        chain *C = &P->chains[chainId];
        nbResPerChain[chainId] = C->size;
        for (int r = 0; r < C->size; r++) {
            residue *R = &C->residues[r];
            atom *CA = getAtom(*R, (char *)"CA");
            atom *O = getAtom(*R, (char *)"O");
            char ss = R->ss;
            if (CA == NULL || O == NULL) {
                cerr << "CA or O not found in chain " << C->id << " residue " << R->type << "_" << R->id << endl;
                exit(-1);
            }
            CAOPos.push_back(CA->coor.x);
            CAOPos.push_back(CA->coor.y);
            CAOPos.push_back(CA->coor.z);
            CAOPos.push_back(O->coor.x);
            CAOPos.push_back(O->coor.y);
            CAOPos.push_back(O->coor.z);
            allSS.push_back(ss);
        }
    }

    cout << "Starting" << endl;
    fflush(stdout);
    vector<Mesh> meshes = computeCartoonMesh(P->size, &nbResPerChain[0], &CAOPos[0], &allSS[0]);
#endif

#if TIMEIT
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( stop - start ).count();
    cerr << duration << " ms" << endl;
#endif

    //Write the meshes to an OBJ file
    writeToObj(outputPath, meshes);

    return 0;
}