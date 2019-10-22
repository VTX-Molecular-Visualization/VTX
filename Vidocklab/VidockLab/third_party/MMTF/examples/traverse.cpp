// *************************************************************************
//
// Licensed under the MIT License (see accompanying LICENSE file).
//
// The authors of this code are: Gerardo Tauriello
// 
// The code is heavily based on traverse.c written by Julien Ferte from the
// mmtf_c project
//
// *************************************************************************

#include <mmtf.hpp>

// C-style libraries used here to keep it close to traverse.c
#include <stdio.h>
#include <string.h>
#include <assert.h>

// *************************************************************************
// JSON-style
// *************************************************************************

// printf for strings
template<typename T>
void printval(const char* pff, const T& val) {
    printf(pff, val);
}
template<>
void printval(const char* pff, const std::string& val) {
    printf(pff, val.c_str());
}

// print vector only
template<typename T>
void printvec(const char* pff, const T* v, const size_t N) {
    printf("[");
    for (size_t i = 0; i < N; ++i) {
        if (i > 0) printf(", ");
        printval(pff, v[i]);
    }
    printf("]");
}

// writing with no check if it's set or not
template<typename T>
void printreq(const char* label, const char* pff,
              const T* v, const size_t N, bool last = false) {
    printf("%s: ", label);
    printvec(pff, v, N);
    printf("%s\n", (last) ? "" : ",");
}

template<typename T>
void printreq(const char* label, const char* pff,
              const std::vector<T>& v, bool last = false) {
    if (v.empty()) {
        printf("%s: []%s\n", label, (last) ? "" : ",");
    } else {
        printreq(label, pff, &v[0], v.size(), last);
    }
}

void printreq(const char* label, const std::string& s, bool last = false) {
    printf("%s: \"%s\"%s\n", label, s.c_str(), (last) ? "" : ",");
}

void printreq(const char* label, const float& f, bool last = false) {
    printf("%s: %g%s\n", label, f, (last) ? "" : ",");
}

// write with check if set
template<typename T>
void printopt(const char* label, const T& val, bool last = false) {
    if (!mmtf::isDefaultValue(val)) {
        printreq(label, val, last);
    } else {
        printf("%s: null%s\n", label, (last) ? "" : ",");
    }
}

template<typename T>
void printopt(const char* label, const char* pff,
              const std::vector<T>& vec, bool last = false) {
    if (!mmtf::isDefaultValue(vec)) {
        printreq(label, pff, vec, last);
    } else {
        printf("%s: null%s\n", label, (last) ? "" : ",");
    }
}

// JSON style printing of parts (not very elegant hack here)
template<typename T>
void json_print(const T& t);

template<typename T>
void json_print_list(const char* label, const T* v, const size_t N,
                     int indent, bool last = false) {
    printf("%s: [", label);
    for (size_t i = 0; i < N; ++i) {
        if (i > 0) printf(",\n");
        else       printf("\n");
        json_print(v[i]);
    }
    printf("\n%*s%s\n", indent+1, "]", (last) ? "" : ",");
}

template<typename T>
void json_print_list(const char* label, const std::vector<T>& v,
                     int indent, bool last = false) {
    json_print_list(label, &v[0], v.size(), indent, last);
}

// for optional vectors
template<typename T>
void json_print_opt(const char* label, const std::vector<T>& v,
                    int indent, bool last = false) {
    if (!mmtf::isDefaultValue(v)) {
        json_print_list(label, &v[0], v.size(), indent, last);
    } else {
        printf("%s: null%s\n", label, (last) ? "" : ",");
    }
}

// just aimed for ncsOperatorList
template<>
void json_print(const std::vector<float>& list) {
    printf("  ");
    printvec("%g", &list[0], list.size());
}

template<>
void json_print(const mmtf::Transform& transform) {
    printf("    {\n");
    printreq("     \"chainIndexList\"", "%d", transform.chainIndexList);
    printreq("     \"matrix\"", "%g", transform.matrix, 16, true);
    printf("    }");
}

template<>
void json_print(const mmtf::BioAssembly& ba) {
    printf("  {\n");
    printreq("   \"name\"", ba.name);
    json_print_list("   \"transformList\"", ba.transformList, 3, true);
    printf("  }");
}

template<>
void json_print(const mmtf::Entity& ent) {
    printf("  {\n");
    printreq("   \"chainIndexList\"", "%d", ent.chainIndexList);
    printreq("   \"description\"", ent.description);
    printreq("   \"type\"", ent.type);
    printreq("   \"sequence\"", ent.sequence, true);
    printf("  }");
}

template<>
void json_print(const mmtf::GroupType& group) {
    printf("  {\n");
    printreq("   \"formalChargeList\"", "%d", group.formalChargeList);
    printreq("   \"atomNameList\"", "\"%s\"", group.atomNameList);
    printopt("   \"bondAtomList\"", "%d", group.bondAtomList);
    printopt("   \"bondOrderList\"", "%d", group.bondOrderList);
    printopt("   \"bondResonanceList\"", "%d", group.bondResonanceList);
    printreq("   \"groupName\"", group.groupName);
    printf("   \"singleLetterCode\": \"%c\",\n", group.singleLetterCode);
    printreq("   \"chemCompType\"", group.chemCompType, true);
    printf("  }");
}

/**
 * @brief Raw output of the whole thing json style.
 */
template<>
void json_print(const mmtf::StructureData& example) {
    printf("{\n");
    // generic info
    printreq(" \"mmtfVersion\"", example.mmtfVersion);
    printreq(" \"mmtfProducer\"", example.mmtfProducer);
    printopt(" \"unitCell\"", "%g", example.unitCell);
    printopt(" \"spaceGroup\"", example.spaceGroup);
    printopt(" \"structureId\"", example.structureId);
    printopt(" \"title\"", example.title);
    printopt(" \"depositionDate\"", example.depositionDate);
    printopt(" \"releaseDate\"", example.releaseDate);
    
    // json_print_opt(" \"ncsOperatorList\"", example.ncsOperatorList, 1);
    json_print_opt(" \"bioAssemblyList\"", example.bioAssemblyList, 1);
    json_print_opt(" \"entityList\"", example.entityList, 1);
    printopt(" \"experimentalMethods\"", "\"%s\"", example.experimentalMethods);

    printopt(" \"resolution\"", example.resolution);
    printopt(" \"rFree\"", example.rFree);
    printopt(" \"rWork\"", example.rWork);

    printf(" \"numBonds\": %d,\n", example.numBonds);
    printf(" \"numAtoms\": %d,\n", example.numAtoms);
    printf(" \"numGroups\": %d,\n", example.numGroups);
    printf(" \"numChains\": %d,\n", example.numChains);
    printf(" \"numModels\": %d,\n", example.numModels);

    json_print_list(" \"groupList\"", example.groupList, 1);

    printopt(" \"bondAtomList\"", "%d", example.bondAtomList);
    printopt(" \"bondOrderList\"", "%d", example.bondOrderList);
    printopt(" \"bondResonanceList\"", "%d", example.bondResonanceList);

    printreq(" \"xCoordList\"", "%g", example.xCoordList);
    printreq(" \"yCoordList\"", "%g", example.yCoordList);
    printreq(" \"zCoordList\"", "%g", example.zCoordList);
    printopt(" \"bFactorList\"", "%g", example.bFactorList);
    printopt(" \"atomIdList\"", "%d", example.atomIdList);
    printopt(" \"altLocList\"", "%d", example.altLocList);
    printopt(" \"occupancyList\"", "%g", example.occupancyList);

    printreq(" \"groupIdList\"", "%d", example.groupIdList);
    printreq(" \"groupTypeList\"", "%d", example.groupTypeList);
    printopt(" \"secStructList\"", "%d", example.secStructList);
    printopt(" \"insCodeList\"", "%d", example.insCodeList);
    printopt(" \"sequenceIndexList\"", "%d", example.sequenceIndexList);

    printreq(" \"chainIdList\"", "\"%s\"", example.chainIdList);
    printopt(" \"chainNameList\"", "\"%s\"", example.chainNameList);
    printreq(" \"groupsPerChain\"", "%d", example.groupsPerChain);
    printreq(" \"chainsPerModel\"", "%d", example.chainsPerModel, true);

    printf("}\n");
}

// *************************************************************************
// Verbose style
// *************************************************************************

/**
 * @brief If any value from
 * \link mmtf::StructureData::insCodeList insCodeList \endlink or
 * \link mmtf::StructureData::altLocList altLocList \endlink is empty,
 * it would would replace the character to a dot
 * @param c character which needs to be checked
 * @return The c character if it is not empty otherwise a dot
 */
char safechar(char c) {
    return (c < ' ') ? '.' : c;
}

// helper for optional entries (printval(pff, vec[i]) only if vec given)
template<typename T>
void printvalo(const char* pff, const std::vector<T>& vec, size_t i) {
    if (!mmtf::isDefaultValue(vec)) printval(pff, vec[i]);
}
// helper for char entries (do safeprint of vec[i])
template<>
void printvalo(const char* pff, const std::vector<char>& vec, size_t i) {
    if (!mmtf::isDefaultValue(vec)) printval(pff, safechar(vec[i]));
}

/**
 * @brief This is the main traversal function to read out all the contents of
 *        mmtf::StructureData
 * @param example Any existing mmtf::StructureData which you want to read
 * @return void
 */
void traverse_main(const mmtf::StructureData& example) {
    // generic info
    printreq("mmtfVersion", example.mmtfVersion, true);
    printreq("mmtfProducer", example.mmtfProducer, true);
    printopt("unitCell", "%g", example.unitCell, true);
    printopt("spaceGroup", example.spaceGroup, true);
    printopt("structureId", example.structureId, true);
    printopt("title", example.title, true);
    printopt("depositionDate", example.depositionDate, true);
    printopt("releaseDate", example.releaseDate, true);

    for (size_t i = 0; i < example.ncsOperatorList.size(); ++i) {
        printf("ncsOperatorList[%d]", int(i));
        printreq("", "%g", example.ncsOperatorList[i], true);
    }

    for (size_t i = 0; i < example.bioAssemblyList.size(); ++i) {
        printf("bioAssemblyIndex: %d\n", int(i));
        const mmtf::BioAssembly& ba = example.bioAssemblyList[i];
        printreq(" name", ba.name, true);
        for (size_t j = 0; j < ba.transformList.size(); ++j) {
            printf(" bioAssemblyTransformIndex: %d\n", int(j));
            const mmtf::Transform & transform = ba.transformList[j];
            printreq("  chainIndexList", "%d", transform.chainIndexList, true);
            printreq("  matrix", "%g", transform.matrix, 16, true);
        }
    }
    
    for (size_t i = 0; i < example.entityList.size(); ++i) {
        printf("entityIndex: %d\n", int(i));
        const mmtf::Entity& ent = example.entityList[i];
        printreq(" chainIndexList", "%d", ent.chainIndexList, true);
        printreq(" description", ent.description, true);
        printreq(" type", ent.type, true);
        printreq(" sequence", ent.sequence, true);
    }
    
    for (size_t i = 0; i < example.experimentalMethods.size(); ++i) {
        printf("experimentalMethod %d: %s\n", int(i),
               example.experimentalMethods[i].c_str());
    }

    printopt("resolution", example.resolution, true);
    printopt("rFree", example.rFree, true);
    printopt("rWork", example.rWork, true);

    printf("numBonds: %d\n", example.numBonds);
    printf("numAtoms: %d\n", example.numAtoms);
    printf("numGroups: %d\n", example.numGroups);
    printf("numChains: %d\n", example.numChains);
    printf("numModels: %d\n", example.numModels);

    //  # initialize index counters
    int modelIndex = 0;
    int chainIndex = 0;
    int groupIndex = 0;
    int atomIndex = 0;
    //# traverse models
    int i;
    for (i = 0; i < example.numModels; i++) {
        int modelChainCount = example.chainsPerModel[i];
        printf("modelIndex: %d\n", modelIndex);
        //    # traverse chains
        int j;
        for (j = 0; j < modelChainCount; j++) {
            printf(" chainIndex : %d\n", chainIndex);
            printval("  Chain id: %s\n", example.chainIdList[chainIndex]);
            printvalo("  Chain name: %s\n", example.chainNameList, chainIndex);
            int chainGroupCount = example.groupsPerChain[chainIndex];
            //        # traverse groups
            int k;
            for (k = 0; k < chainGroupCount; k++) {
                printf("  groupIndex: %d\n", groupIndex);
                printf("   groupId: %d\n", example.groupIdList[groupIndex]);
                printvalo("   insCodeList: %c\n", example.insCodeList,
                          groupIndex);
                printvalo("   secStruc: %d\n", example.secStructList,
                          groupIndex);
                printvalo("   seqIndex: %i\n", example.sequenceIndexList,
                          groupIndex);
                printf("   groupType: %d\n", example.groupTypeList[groupIndex]);
                const mmtf::GroupType& group =
                        example.groupList[example.groupTypeList[groupIndex]];
                printval("    Group name: %s\n", group.groupName);
                printf("    Single letter code: %c\n", group.singleLetterCode);
                printval("    Chem comp type: %s\n", group.chemCompType);
                int atomOffset = atomIndex;

                int l;
                for (l = 0; l < group.bondAtomList.size(); l += 2) {
                    printf("    Atom id One: %d\n",
                           (atomOffset + group.bondAtomList[l]));
                    printf("    Atom id Two: %d\n",
                           (atomOffset + group.bondAtomList[l + 1]));
                    printvalo("    Bond order: %d\n",
                              group.bondOrderList, l / 2);
                    printvalo("    Bond resonance: %d\n",
                              group.bondResonanceList, l / 2);
                }
                int groupAtomCount = group.atomNameList.size();
                for (l = 0; l < groupAtomCount; l++) {
                    printf("    atomIndex: %d\n", atomIndex);
                    printf("     x coord: %.3f\n",
                           example.xCoordList[atomIndex]);
                    printf("     y coord: %.3f\n",
                           example.yCoordList[atomIndex]);
                    printf("     z coord: %.3f\n",
                           example.zCoordList[atomIndex]);
                    printvalo("     b factor: %.2f\n", example.bFactorList,
                              atomIndex);
                    printvalo("     atom id: %d\n", example.atomIdList,
                              atomIndex);
                    printvalo("     altLocList: %c\n", example.altLocList,
                              atomIndex);
                    printvalo("     occupancy: %.2f\n", example.occupancyList,
                              atomIndex);
                    printf("     charge: %d\n", group.formalChargeList[l]);
                    printval("     atom name: %s\n", group.atomNameList[l]);
                    printval("     element: %s\n", group.elementList[l]);
                    atomIndex++;
                }
                groupIndex++;
            }
            chainIndex++;
        }
        modelIndex++;
    }
    printf("Number of inter group bonds: %d\n",
           (int) example.bondAtomList.size() / 2);
    for (i = 0; i < example.bondAtomList.size(); i += 2) {
        printf(" Atom One: %d\n", example.bondAtomList[i]);
        printf(" Atom Two: %d\n", example.bondAtomList[i + 1]);
        printvalo(" Bond order: %d\n", example.bondOrderList, i / 2);
        printvalo(" Bond resonance: %d\n", example.bondResonanceList, i / 2);
    }
}

// *************************************************************************
// PDB style
// *************************************************************************

/**
 * @brief Read out the contents of mmtf::StructureData in a PDB-like fashion
 * Columns are in order:
 * ATOM/HETATM AtomId Element AtomName AltLoc GroupId GroupType
 * InsCode ChainName x y z B-factor Occupancy Charge
 * @param example Any existing mmtf::StructureData which you want to read
 */
void traverse_pdb_like(const mmtf::StructureData& example) {


    int modelIndex = 0;
    int chainIndex = 0;
    int groupIndex = 0;
    int atomIndex = 0;

    //# traverse models
    for (int i = 0; i < example.numModels; i++, modelIndex++) {
        //    # traverse chains
        for (int j = 0; j < example.chainsPerModel[modelIndex]; j++, chainIndex++) {
            //        # traverse groups
            for (int k = 0; k < example.groupsPerChain[chainIndex]; k++, groupIndex++) {
                const mmtf::GroupType& group =
                        example.groupList[example.groupTypeList[groupIndex]];
                int groupAtomCount = group.atomNameList.size();

                for (int l = 0; l < groupAtomCount; l++, atomIndex++) {
                    // ATOM or HETATM
                    if (mmtf::is_hetatm(chainIndex, example.entityList, group))
                        printf("HETATM ");
                    else
                        printf("ATOM ");
                    // Atom serial
                    printvalo("%d ", example.atomIdList, atomIndex);
                    // Atom name
                    printval("%s ", group.atomNameList[l]);
                    // Alternate location
                    printvalo("%c ", example.altLocList, atomIndex);
                    // Group name
                    printval("%s ", group.groupName);
                    // Chain
                    printvalo("%s ", example.chainNameList, chainIndex);
                    // Group serial
                    printf("%d ", example.groupIdList[groupIndex]);
                    // Insertion code
                    printvalo("%c ", example.insCodeList, groupIndex);
                    // x, y, z
                    printf("%.3f ", example.xCoordList[atomIndex]);
                    printf("%.3f ", example.yCoordList[atomIndex]);
                    printf("%.3f ", example.zCoordList[atomIndex]);
                    // B-factor
                    printvalo("%.2f ", example.bFactorList, atomIndex);
                    // Occupancy
                    printvalo("%.2f ", example.occupancyList, atomIndex);
                    // Element
                    printval("%s ", group.elementList[l]);
                    // Charge
                    printf("%d \n", group.formalChargeList[l]);
                }
            }
        }
    }
}

int main(int argc, char** argv) {
    // check arguments
    if (argc < 2) {
      printf("USAGE: traverse <mmtffile> [<style>]\n");
      printf("-> if style ommited, we output a PDB-like output\n");
      printf("-> if style == 'json', we output a json-dump of all data\n");
      printf("-> if style == 'print', we output a built-in tab delimited format\n");
      printf("-> else, we output all the data in a verbose, readable form\n");
      return 1;
    }

    // decode MMTF file
    mmtf::StructureData example;
    mmtf::decodeFromFile(example, argv[1]);

    // traverse it according to user specified style
    if (argc > 2) {
        if (strcmp(argv[2], "json") == 0) {
            json_print(example);
        } else if (strcmp(argv[2], "print") == 0) {
            std::cout << example.print() << std::endl;
        } else {
            traverse_main(example);
        }
    } else {
      traverse_pdb_like(example);
    }
    return 0;
}
