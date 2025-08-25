# The purpose of these project are to identify pdb structures that crash our IO in either reading, or read/write/read mode. 
# read/write/read mode shows additional statistics for quality of reading and writing in final reports.
# This project assumes PDB100_DIRECTORY_PATH cmake var has been set to the pdb100 data base local directory prior to this .cmake file

# There are two executable projects : the parent process that gather the list of files to be tested and the child that will be started multiple times as separated processes.



set(CHILD_PROCESS_NAME "io_vs_pdb100_child")
add_executable(io_vs_pdb100_child "${CMAKE_CURRENT_LIST_DIR}/local/io_vs_pdb100/src/vtx/io_vs_pdb100/new_process.cpp")
target_include_directories(io_vs_pdb100_child PRIVATE "${CMAKE_CURRENT_LIST_DIR}/local/io_vs_pdb100/include;")
vtx_configure_target(io_vs_pdb100_child)

target_compile_definitions(io_vs_pdb100_child PRIVATE FIRST_READ_ONLY=1)
target_link_libraries(io_vs_pdb100_child PRIVATE vtx_util)
target_link_libraries(io_vs_pdb100_child PRIVATE vtx_core)	
target_link_libraries(io_vs_pdb100_child PRIVATE vtx_io)
target_link_libraries(io_vs_pdb100_child PRIVATE LibArchive::LibArchive)	
target_link_libraries(io_vs_pdb100_child PRIVATE fmt::fmt)	
target_link_libraries(io_vs_pdb100_child PRIVATE Boost::headers)

add_executable(io_vs_pdb100 "${CMAKE_CURRENT_LIST_DIR}/local/io_vs_pdb100/src/vtx/io_vs_pdb100/pdb100.cpp")
target_compile_definitions(io_vs_pdb100 PRIVATE PDB_DATABASE_DIR="${PDB100_DIRECTORY_PATH}")
target_compile_definitions(io_vs_pdb100 PRIVATE CHILD_PROCESS_NAME="${CHILD_PROCESS_NAME}")
target_compile_definitions(io_vs_pdb100 PRIVATE NUM_PROCESSES=24)
target_include_directories(io_vs_pdb100 PRIVATE "${CMAKE_CURRENT_LIST_DIR}//local/io_vs_pdb100/include;")
vtx_configure_target(io_vs_pdb100)
target_link_libraries(io_vs_pdb100 PRIVATE vtx_util)
target_link_libraries(io_vs_pdb100 PRIVATE vtx_core)	
target_link_libraries(io_vs_pdb100 PRIVATE fmt::fmt)	
target_link_libraries(io_vs_pdb100 PRIVATE vtx_io)
target_link_libraries(io_vs_pdb100 PRIVATE LibArchive::LibArchive)	
target_link_libraries(io_vs_pdb100 PRIVATE Boost::headers)
add_dependencies(io_vs_pdb100 io_vs_pdb100_child ) # QoL