// *************************************************************************
//
// Simple test for compilation issues.
//
// *************************************************************************

#include "multi_cpp_test_helper.hpp"
#include <iostream>

void read_check_file(mmtf::StructureData& data, const std::string& filename) {
    // decode MMTF file
    mmtf::decodeFromFile(data, filename);

    // check if the data is self-consistent
    if (data.hasConsistentData()) {
      std::cout << "Successfully read " << filename << ".\n";
    } else {
      std::cout << "Inconsistent data in " << filename << ".\n";
    }
}
