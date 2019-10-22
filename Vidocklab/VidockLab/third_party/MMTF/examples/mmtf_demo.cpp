
#include <mmtf.hpp>

#include <iostream>
#include <string>

int main(int argc, char** argv) {
    // check arguments
    if (argc < 2) {
      printf("USAGE: mmtf_demo <mmtffile>\n");
      return 1;
    }

    // decode MMTF file
    std::string filename(argv[1]);
    mmtf::StructureData data;
    mmtf::decodeFromFile(data, filename);

    // check if the data is self-consistent
    if (data.hasConsistentData(true)) {
      std::cout << "Successfully read " << filename << ".\n";
      return 0;
    } else {
      std::cout << "Inconsistent data in " << filename << ".\n";
      return 1;
    }
}
