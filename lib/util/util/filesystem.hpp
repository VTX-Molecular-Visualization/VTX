#ifndef __VTX_UTIL_FILESYSTEM__
#define __VTX_UTIL_FILESYSTEM__

#include "types.hpp"

namespace VTX::Util::Filesystem
{
	const std::string readPath( const FilePath & p_filePath );
	void			  generateUniqueFileName( FilePath & p_filePath );
	void			  removeAll( const FilePath & p_filePath );

	/*
	static const QString MOLECULE_EXTENSIONS_READ
		= "Amber Net CDF (*.nc);;"
		  "CIF(*.cif );;"
		  "CML (*.cml);;"
		  "CSSR (*.cssr);;"
		  "DCD (*.dcd);;"
		  "GRO (*.gro);;"
		  "LAMMPS (*.lammpstrj);;"
		  "MMCIF(*.mmcif);;"
		  "MMTF (*.mmtf);;"
		  "MOL2 (*.mol2);;"
		  "Molden (*.molden);;"
		  "PDB (*.pdb);;"
		  "SDF (*.sdf);;"
		  "SMI (*.smi);;"
		  "Tinker (*.arc *.psf *.prm);;"
		  "TNG (*.tng);;"
		  "TRJ (*.trj);;"
		  "TRR (*.trr);;"
		  "XTC (*.xtc);;"
		  "XYZ (*.xyz);;"
		  "All (*)";

	static const QString MOLECULE_EXTENSIONS_WRITE
		= "Amber Net CDF (*.nc);;"
		  "CIF(*.cif );;"
		  "CML (*.cml);;"
		  "CSSR (*.cssr);;"
		  "GRO (*.gro);;"
		  "LAMMPS (*.lammpstrj);;"
		  "MMCIF(*.mmcif);;"
		  "MMTF (*.mmtf);;"
		  "MOL2 (*.mol2);;"
		  "PDB (*.pdb);;"
		  "SDF (*.sdf);;"
		  "SMI (*.smi);;"
		  "Tinker (*.arc *.psf *.prm);;"
		  "TRR (*.trr);;"
		  "XTC (*.xtc);;"
		  "XYZ (*.xyz);;";

	static const QString TRAJECTORY_EXTENSIONS_READ
		= "Amber Net CDF (*.nc);;"
		  "DCD (*.dcd);;"
		  "GRO (*.gro);;"
		  "LAMMPS (*.lammpstrj);;"
		  "Tinker (*.arc *.psf *.prm);;"
		  "TNG (*.tng);;"
		  "TRJ (*.trj);;"
		  "TRR (*.trr);;"
		  "XTC (*.xtc);;"
		  "All (*)";

	static const std::string IMAGE_EXPORT_EXTENSIONS
		= "PNG (*.png);;"
		  "JPEG (*.jpg *.jpeg);;"
		  "BMP (*.bmp);;"
		  "All (*)";

	static const std::string VTX_EXTENSIONS = "VTX file (*.vtx)";

	static const std::string LOAD_MOLECULE_FILTERS	 = MOLECULE_EXTENSIONS_READ;
	static const std::string EXPORT_MOLECULE_FILTERS = MOLECULE_EXTENSIONS_WRITE;
	static const std::string OPEN_FILE_FILTERS		 = VTX_EXTENSIONS + ";;" + MOLECULE_EXTENSIONS_READ;
	static const std::string SAVE_SCENE_FILTERS		 = VTX_EXTENSIONS;

	static const std::string DEFAULT_MOLECULE_READ_FILTER	  = "All (*)";
	static const std::string DEFAULT_MOLECULE_WRITE_FILTER	  = "MMCIF(*.mmcif)";
	static const std::string DEFAULT_MOLECULE_WRITE_EXTENSION = DEFAULT_MOLECULE_WRITE_FILTER.mid(
		DEFAULT_MOLECULE_WRITE_FILTER.lastIndexOf( '.' ) + 1,
		DEFAULT_MOLECULE_WRITE_FILTER.size() - DEFAULT_MOLECULE_WRITE_FILTER.lastIndexOf( '.' ) - 2 );

	static const std::string LOAD_TRAJECTORY_FILTERS		= TRAJECTORY_EXTENSIONS_READ;
	static const std::string DEFAULT_TRAJECTORY_READ_FILTER = "All (*)";

	static const std::string DEFAULT_FILE_READ_FILTER  = "All (*)";
	static const std::string DEFAULT_FILE_WRITE_FILTER = VTX_EXTENSIONS;

	static const std::string REPRESENTATION_PRESET_FILE_FILTERS = "Representation file (*)";
	static const std::string RENDER_EFFECT_PRESET_FILE_FILTERS	= "Render effect file (*)";

	static const FilePath STYLESHEET_FILE_DEFAULT = FilePath( ":/stylesheet_ui.css" );
	static const FilePath SCENE_OBJECT_DIR		  = FilePath( "obj" );

	static const std::string DEFAULT_SCENE_FILENAME	   = "New Scene";
	static const std::string DEFAULT_MOLECULE_FILENAME = "New Molecule";


	bool			  copyFile( const FilePath & p_from, const FilePath & p_to );
	bool		  remove( const FilePath & p_filename );
	bool		  removeAll( const FilePath & p_directory );
	void		  copyDir( const FilePath & p_from, const FilePath & p_to );
	std::set<FilePath> getFilesInDirectory( const FilePath & p_directory );
	void			   checkSaveDirectoryHierarchy( const FilePath & p_savePath );
	FilePath		   getDefaultMoleculeExportPath();
	FilePath		   getDefaultSceneSavePath();
	std::string		   getImageExportDefaultFilter();
	*/

} // namespace VTX::Util::Filesystem

#endif
