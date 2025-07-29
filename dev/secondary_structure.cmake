# The purpose of this project is to benchmark our secondary-structure algorithm against the rcsb-pdb data. 
# The goal is to use the rcsb-pdb data to improve our algorithm to predict secondary structure in a more reliable way without relying on user input data (that might be missing or garbage)


file(GLOB_RECURSE PDB100_GEN_SOURCES "${CMAKE_CURRENT_LIST_DIR}/local/secondary_structure/src/*")
file(GLOB_RECURSE PDB100_GEN_HEADERS "${CMAKE_CURRENT_LIST_DIR}/local/secondary_structure/include/*")
file(GLOB_RECURSE PDB100_GEN_SHARED_HEADERS "${CMAKE_CURRENT_LIST_DIR}/local/secondary_structure/shared/include/*")
set(PDB100_GEN_SHARED_HEADER_DIR "${CMAKE_CURRENT_LIST_DIR}/local/secondary_structure/shared/include")

file(GLOB_RECURSE PDB100_CHILD_GEN_SOURCES "${CMAKE_CURRENT_LIST_DIR}/local/secondary_structure/child/src/*")
file(GLOB_RECURSE PDB100_CHILD_GEN_HEADERS "${CMAKE_CURRENT_LIST_DIR}/local/secondary_structure/child/include/*")


set(CHILD_PROCESS_NAME "secondary_structure_child")

# I shall multi thread : workers for reading file and the main thread will be able to produce and compare results with our in-house algorithm. When all database is parsed, a report will be generated so we can investigate errors . 
# Maybe we will provide a list of structure that we will ignore to validate our algorithms for some specific reasons.
add_executable(secondary_structure "${PDB100_GEN_SOURCES};${PDB100_GEN_HEADERS};${PDB100_GEN_SHARED_HEADERS}")
add_executable(secondary_structure_child "${PDB100_CHILD_GEN_SOURCES};${PDB100_CHILD_GEN_HEADERS};${PDB100_GEN_SHARED_HEADERS}")
target_include_directories(secondary_structure PUBLIC "${CMAKE_CURRENT_LIST_DIR}/local/secondary_structure/include;${PDB100_GEN_SHARED_HEADER_DIR}")
target_include_directories(secondary_structure_child PUBLIC "${CMAKE_CURRENT_LIST_DIR}/local/secondary_structure/child/include;${PDB100_GEN_SHARED_HEADER_DIR}")

target_compile_definitions(secondary_structure PRIVATE PDB100_DATABASE_DIR="${PDB100_DIRECTORY_PATH}")
target_compile_definitions(secondary_structure PRIVATE CHILD_PROCESS_NAME="${CHILD_PROCESS_NAME}")

target_link_libraries(secondary_structure PRIVATE vtx_util)
target_link_libraries(secondary_structure PRIVATE vtx_core)
target_link_libraries(secondary_structure PRIVATE vtx_io)
target_link_libraries(secondary_structure PRIVATE fmt::fmt)
target_link_libraries(secondary_structure PRIVATE LibArchive::LibArchive)		
target_link_libraries(secondary_structure PRIVATE Boost::headers)
target_link_libraries(secondary_structure PRIVATE Boost::process)

target_link_libraries(secondary_structure_child PRIVATE vtx_util)
target_link_libraries(secondary_structure_child PRIVATE vtx_core)
target_link_libraries(secondary_structure_child PRIVATE vtx_io)
target_link_libraries(secondary_structure_child PRIVATE fmt::fmt)
target_link_libraries(secondary_structure_child PRIVATE LibArchive::LibArchive)		
target_link_libraries(secondary_structure_child PRIVATE Boost::headers)

