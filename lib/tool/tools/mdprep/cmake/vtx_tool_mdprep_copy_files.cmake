
get_filename_component(_vtx_tool_mdprep_top_dir "./external/gromacs/share/top" ABSOLUTE )
message ("### From vtx_tool_mdprep_copy_files.cmake _vtx_tool_mdprep_top_dir <${_vtx_tool_mdprep_top_dir}>")
message ("Now calling register_build_directory_copy")
register_build_directory_copy("${_vtx_tool_mdprep_top_dir}")