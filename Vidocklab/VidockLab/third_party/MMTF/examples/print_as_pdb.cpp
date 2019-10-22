// *************************************************************************
//
// Licensed under the MIT License (see accompanying LICENSE file).
//
// The authors of this code is Daniel Farrell
// 
// NOTE:  This wasn't made to be a well tested error-free executable... More
// of an example of how you might iterate over an mmtf file in pdb style.  
// Please DO NOT USE THIS IN PRODUCTION.
// *************************************************************************

#include <mmtf.hpp>

#include <iostream>
#include <string>

std::string print_sd_as_pdb(mmtf::StructureData const & sd, bool index_at_0) {
  std::ostringstream out;
  int modelIndex = 0;
  int chainIndex = 0;
  int groupIndex = 0;
  int atomIndex = 0;

  //# traverse models
  for (int i = 0; i < sd.numModels; i++, modelIndex++) {
    //    # traverse chains
    for (int j = 0; j < sd.chainsPerModel[modelIndex]; j++, chainIndex++) {
      //        # traverse groups
      for (int k = 0; k < sd.groupsPerChain[chainIndex]; k++, groupIndex++) {
        const mmtf::GroupType& group =
            sd.groupList[sd.groupTypeList[groupIndex]];
        int groupAtomCount = group.atomNameList.size();

        for (int l = 0; l < groupAtomCount; l++, atomIndex++) {
          // ATOM or HETATM
          if (mmtf::is_hetatm(chainIndex, sd.entityList, group))
            out << "HETATM";
          else
            out << "ATOM  ";
          // Atom serial
          if (index_at_0 || mmtf::isDefaultValue(sd.atomIdList)) {
            out << std::setfill(' ') << std::internal << std::setw(5) <<
              std::right << atomIndex+1;
          } else {
            out << std::setfill(' ') << std::internal << std::setw(5) <<
              std::right << sd.atomIdList[atomIndex] << "  ";
          }
          // Atom name
          out << std::left << std::setw(4) << std::setfill(' ') << group.atomNameList[l];
          // Group name
          out << group.groupName << " ";
          // Chain
          out << sd.chainIdList[chainIndex];
          // Group serial
          out << std::setfill(' ') << std::right << std::setw(4) << sd.groupIdList[groupIndex];
          out << "    ";
          // x, y, z
          out << std::setfill(' ') << std::fixed << std::setprecision(3) << std::setw(8) << std::right;
          out << sd.xCoordList[atomIndex];
          out << std::setfill(' ') << std::fixed << std::setprecision(3) << std::setw(8) << std::right;
          out << sd.yCoordList[atomIndex];
          out << std::setfill(' ') << std::fixed << std::setprecision(3) << std::setw(8) << std::right;
          out << sd.zCoordList[atomIndex];

          // Occupancy
          out << std::setfill(' ') << std::fixed << std::setprecision(2) << std::setw(6) << std::right;
          if ( !mmtf::isDefaultValue(sd.occupancyList) ) {
            out << sd.occupancyList[atomIndex];
          } else out << " ";
          out << "                ";

          // Element
          out << std::right << std::setw(2) << std::setfill(' ') << group.elementList[l] << "  \n";
        }
      }
    }
  }
  out << "END";
  return out.str();
}


int main(int argc, char** argv) {
    // check arguments
    if (argc != 2) {
      std::cout << "USAGE: ./print_sd_as_pdb <in mmtf file>" << std::endl;
      return 1;
    }

    mmtf::StructureData d;
    mmtf::decodeFromFile(d, argv[1]);
    std::cout << print_sd_as_pdb(d, false) << std::endl;
}
