function(vtx_tool_mdprep_copy_files dest)
	configure_file("./external/gromacs/share/top" "${dest}/data/tool/tools/mdprep/gromacs/top" COPYONLY)
endfunction()