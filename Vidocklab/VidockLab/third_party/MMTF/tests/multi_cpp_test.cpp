// *************************************************************************
//
// Simple test for compilation issues.
//
// *************************************************************************

#include <mmtf.hpp>
#include "multi_cpp_test_helper.hpp"

int main(int argc, char** argv) {
    // decode MMTF file
    std::string filename = "../mmtf_spec/test-suite/mmtf/173D.mmtf";
    mmtf::StructureData data;
    read_check_file(data, filename);

    return 0;
}
