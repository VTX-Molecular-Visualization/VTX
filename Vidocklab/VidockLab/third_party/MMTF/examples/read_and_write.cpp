// *************************************************************************
//
// Licensed under the MIT License (see accompanying LICENSE file).
//
// The authors of this code is Daniel Farrell
// 
// *************************************************************************

#include <mmtf.hpp>

int main(int argc, char** argv) {
    // check arguments
    if (argc != 3) {
      printf("USAGE: read_and_write <in mmtf file> <out mmtf file>\n");
      return 1;
    }

    // decode->encode->decode
    mmtf::StructureData d;
    mmtf::decodeFromFile(d, argv[1]);
    mmtf::StructureData example(d);
    mmtf::encodeToFile(example, argv[2]);
    mmtf::StructureData d2;
    mmtf::decodeFromFile(d2, argv[2]);
    if (d == d2) {
      std::cout << "Found read+write yielded equal mmtf files" << std::endl;
      return 0;
    }
    else {
      std::cout << "Found read+write yielded NON-equal mmtf files" << std::endl;
      return 1;
    }
}
