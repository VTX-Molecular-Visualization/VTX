// *************************************************************************
//
// Licensed under the MIT License (see accompanying LICENSE file).
//
// The authors of this code is Thomas Holder
//
// *************************************************************************
//
// This example demonstrates how to export molecular data which is given
// as atom and bond tables.
//
// *************************************************************************

#include <mmtf.hpp>
#include <mmtf/export_helpers.hpp>

#include <iostream>
#include <string>

#define ARRAYEND(a) ((a) + sizeof(a) / sizeof(a)[0])

/*
 * Atom table of example "ALA-GLY-ALA" tripeptide
 */
struct Atom {
  std::string chain;
  int residue_number;
  std::string residue_name;
  std::string atom_name;
  std::string element;
  int formal_charge;
  float x, y, z;
} atomarray[] = {
  {"A", 1, "ALA", "N",  "N", 0, -0.677, -1.230, -0.491},
  {"A", 1, "ALA", "CA", "C", 0, -0.001,  0.064, -0.491},
  {"A", 1, "ALA", "C",  "C", 0,  1.499, -0.110, -0.491},
  {"A", 1, "ALA", "O",  "O", 0,  2.030, -1.227, -0.502},
  {"A", 1, "ALA", "CB", "C", 0, -0.509,  0.856,  0.727},
  {"A", 2, "GLY", "N",  "N", 0,  2.250,  0.939, -0.479},
  {"A", 2, "GLY", "CA", "C", 0,  3.700,  0.771, -0.479},
  {"A", 2, "GLY", "C",  "C", 0,  4.400,  2.108, -0.463},
  {"A", 2, "GLY", "O",  "O", 0,  3.775,  3.173, -0.453},
  {"A", 3, "ALA", "N",  "N", 0,  5.689,  2.140, -0.462},
  {"A", 3, "ALA", "CA", "C", 0,  6.365,  3.434, -0.447},
  {"A", 3, "ALA", "C",  "C", 0,  7.865,  3.260, -0.447},
  {"A", 3, "ALA", "O",  "O", 0,  8.396,  2.144, -0.470},
  {"A", 3, "ALA", "CB", "C", 0,  5.855,  4.213,  0.778}
};

/*
 * Bond table for "ALA-GLY-ALA" tripeptide
 */
struct Bond {
  size_t atom1;
  size_t atom2;
  int8_t order;
} bondarray[] = {
  {0, 1, 1},
  {1, 2, 1},
  {1, 4, 1},
  {2, 3, 2},
  {2, 5, 1},
  {5, 6, 1},
  {6, 7, 1},
  {7, 8, 2},
  {7, 9, 1},
  {9, 10, 1},
  {10, 11, 1},
  {10, 13, 1},
  {11, 12, 2}
};

int main(int argc, char** argv)
{
  mmtf::StructureData data;
  mmtf::GroupType* residue = NULL;
  const Atom* prevatom = NULL;

  // start new model
  data.chainsPerModel.push_back(0);

  // add atoms
  for (const Atom* atom = atomarray; atom != ARRAYEND(atomarray); ++atom) {
    data.xCoordList.push_back(atom->x);
    data.yCoordList.push_back(atom->y);
    data.zCoordList.push_back(atom->z);

    bool is_same_residue = false;
    bool is_same_chain = prevatom && prevatom->chain == atom->chain;

    if (!is_same_chain) {
      data.chainsPerModel.back() += 1;
      data.groupsPerChain.push_back(0); // increment with every group
      data.chainIdList.push_back(atom->chain);
    } else {
      is_same_residue = prevatom && prevatom->residue_number == atom->residue_number;
    }

    if (!is_same_residue) {
      data.groupsPerChain.back() += 1;
      data.groupTypeList.push_back(data.groupList.size());
      data.groupIdList.push_back(atom->residue_number);
      data.groupList.resize(data.groupList.size() + 1);
      residue = &data.groupList.back();
      residue->groupName = atom->residue_name;
    }

    residue->formalChargeList.push_back(atom->formal_charge);
    residue->atomNameList.push_back(atom->atom_name);
    residue->elementList.push_back(atom->element);

    prevatom = atom;
  }

  data.numAtoms = data.xCoordList.size();
  data.numGroups = data.groupIdList.size();
  data.numChains = data.chainIdList.size();
  data.numModels = data.chainsPerModel.size();

  // construct the BondAdder after adding atoms was completed
  mmtf::BondAdder bondadder(data);

  // add bonds
  for (const Bond* bond = bondarray; bond != ARRAYEND(bondarray); ++bond) {
    bondadder(bond->atom1, bond->atom2, bond->order);
  }

  std::cout << "INFO numBonds (total): " << data.numBonds << std::endl;
  std::cout << "INFO numBonds (inter-residue): " << (data.bondAtomList.size() / 2) << std::endl;
  std::cout << "INFO groupList.size() before compression: " << data.groupList.size() << std::endl;

  mmtf::compressGroupList(data);

  std::cout << "INFO groupList.size() after compression: " << data.groupList.size() << std::endl;

  // write to file if filename provided
  if (argc != 2) {
    printf("USAGE: export <out mmtf file>\n");
  } else {
    mmtf::encodeToFile(data, argv[1]);
  }

  return 0;
}

// vi:sw=2:expandtab
