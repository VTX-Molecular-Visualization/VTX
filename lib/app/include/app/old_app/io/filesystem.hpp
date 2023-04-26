#ifndef __VTX_IO_FILESYSTEM__
#define __VTX_IO_FILESYSTEM__

// TODO: delete those includes
#include "app/component/chemistry/molecule.hpp"
#include "app/model/selection.hpp"
#include "app/old_app/object3d/scene.hpp"
#include "app/old_app/selection/selection_manager.hpp"
#include "app/old_app/setting.hpp"
#include "struct/image_export.hpp"
#include "struct/scene_path_data.hpp"
#include <util/chrono.hpp>
#include <util/exceptions.hpp>
#include <util/filesystem.hpp>

namespace VTX::IO::Filesystem
{
	static FilePath EXECUTABLE_ABSOLUTE_PATH = std::filesystem::current_path();
	// JSon save fail when size > 192
	static const int		 MAX_FILE_LENGTH	  = 180;
	static const std::string REGEX_VALID_FILENAME = "[^\\\\/:*?\"<>|]*";

	enum class FILE_TYPE_ENUM : int
	{
		SCENE,
		CONFIGURATION,
		MOLECULE,
		TRAJECTORY,
		MESH,
		UNKNOWN,

		COUNT
	};

	inline const FilePath getExecutableDir()
	{
		assert( std::filesystem::exists( EXECUTABLE_ABSOLUTE_PATH ) );
		return EXECUTABLE_ABSOLUTE_PATH;
	}

	// Directories.
	inline const FilePath getShadersDir() { return getExecutableDir() / "shaders"; }
	inline const FilePath getSnapshotsDir() { return getExecutableDir() / "snapshots"; }
	inline const FilePath getRendersDir() { return getExecutableDir() / "renders"; }
	inline const FilePath getLogsDir() { return getExecutableDir() / "logs"; }
	inline const FilePath getLibrariesDir() { return getExecutableDir() / "libraries"; }
	inline const FilePath getRepresentationsLibraryDir() { return getLibrariesDir() / "representations"; }
	inline const FilePath getRenderEffectPresetsLibraryDir() { return getLibrariesDir() / "render_effects"; }
	inline const FilePath getInternalDataDir() { return getExecutableDir() / "data"; }
	inline const FilePath getResidueDataDir() { return getInternalDataDir() / "residue_data"; }

	// Files.
	inline const FilePath getConfigIniFile() { return getExecutableDir() / "config.ini"; }
	inline const FilePath getSettingJsonFile() { return getExecutableDir() / "setting.json"; }
	inline const FilePath getLicenseFile() { return getExecutableDir() / "license.txt"; }

	// Dev directories.
	static const FilePath SHADERS_DIR_SRC		  = FilePath( "../src/shader" );
	static const FilePath DEFAULT_SAVE_FOLDER	  = FilePath( "../save" );
	static const FilePath DEFAULT_MOLECULE_FOLDER = FilePath( "../data" );

	inline const FilePath getShadersPath( const FilePath & p_filename )
	{
		std::filesystem::create_directory( getShadersDir() );
		return FilePath( getShadersDir() / p_filename );
	}

	inline const FilePath getSnapshotsPath( const FilePath & p_filename )
	{
		std::filesystem::create_directory( getSnapshotsDir() );
		return FilePath( getSnapshotsDir() / p_filename );
	}

	inline const FilePath getRendersPath( const FilePath & p_filename )
	{
		std::filesystem::create_directory( getRendersDir() );
		return FilePath( getRendersDir() / p_filename );
	}

	inline const FilePath getLogsPath( const FilePath & p_filename )
	{
		std::filesystem::create_directory( getLogsDir() );
		return FilePath( getLogsDir() / p_filename );
	}

	inline const FilePath getRepresentationPath( const FilePath & p_filename )
	{
		std::filesystem::create_directory( getRepresentationsLibraryDir() );
		return FilePath( getRepresentationsLibraryDir() / p_filename );
	}

	inline const FilePath getRenderEffectPath( const FilePath & p_filename )
	{
		std::filesystem::create_directory( getRenderEffectPresetsLibraryDir() );
		return FilePath( getRenderEffectPresetsLibraryDir() / p_filename );
	}

	inline bool isSessionFile( const FilePath & p_filePath ) { return p_filePath.extension().string() == "vtx"; }

	inline FilePath getSceneSaveDirectory( const FilePath & p_savePath )
	{
		const FilePath projectDirectoryName = FilePath( p_savePath.stem().append( "_data" ) );

		return p_savePath.parent_path() / projectDirectoryName;
	}

	inline FilePath getSceneObjectsSaveDirectory( const FilePath & p_savePath )
	{
		return getSceneSaveDirectory( p_savePath ) / "obj";
	}

	inline FilePath getResidueDataFilePath( const std::string & p_residueName )
	{
		return getResidueDataDir() / p_residueName.substr( 0, 1 );
	}

	inline FILE_TYPE_ENUM getFileTypeFromFilePath( const FilePath & p_path )
	{
		const FilePath extension = p_path.extension();

		if ( extension == ".vtx" )
		{
			return FILE_TYPE_ENUM::SCENE;
		}
		else if ( extension == ".prm" || extension == ".psf" )
		{
			return FILE_TYPE_ENUM::CONFIGURATION;
		}
		else if ( extension == ".cif" || extension == ".cml" || extension == ".cssr" || extension == ".gro"
				  || extension == ".mmcif" || extension == ".mmtf" || extension == ".mol2" || extension == ".molden"
				  || extension == ".pdb" || extension == ".sdf" || extension == ".smi" || extension == ".mmtf"
				  || extension == ".xyz" )
		{
			return FILE_TYPE_ENUM::MOLECULE;
		}
		else if ( extension == ".obj" )
		{
			return FILE_TYPE_ENUM::MESH;
		}
		else if ( extension == ".nc" || extension == ".dcd" || extension == ".lammpstrj" || extension == ".arc"
				  || extension == ".trr" || extension == ".xtc" || extension == ".tng" || extension == ".trj" )
		{
			return FILE_TYPE_ENUM::TRAJECTORY;
		}
		else
		{
			return FILE_TYPE_ENUM::UNKNOWN;
		}
	}

	inline void fillFilepathPerMode( std::vector<FilePath>				  p_filepaths,
									 std::vector<std::vector<FilePath>> & p_filepathPerMode )
	{
		p_filepathPerMode.resize( int( FILE_TYPE_ENUM::COUNT ) );

		for ( const FilePath & path : p_filepaths )
		{
			const FILE_TYPE_ENUM filetype = getFileTypeFromFilePath( path );
			p_filepathPerMode[ int( filetype ) ].emplace_back( path );
		}
	}

	inline void checkSaveDirectoryHierarchy( const FilePath & p_savePath )
	{
		const FilePath projectDirectory = getSceneSaveDirectory( p_savePath );
		if ( std::filesystem::exists( projectDirectory ) == false )
		{
			std::filesystem::create_directory( projectDirectory );
		}

		const FilePath objectsPath = getSceneObjectsSaveDirectory( p_savePath );
		if ( std::filesystem::exists( objectsPath ) == false )
		{
			std::filesystem::create_directory( objectsPath );
		}
	}

	inline const FilePath getUniqueSnapshotsPath( const Struct::ImageExport::Format p_format )
	{
		std::string extension;

		switch ( p_format )
		{
		case IO::Struct::ImageExport::Format::PNG: extension = "png"; break;
		case IO::Struct::ImageExport::Format::JPEG: extension = "jpg"; break;
		case IO::Struct::ImageExport::Format::BMP: extension = "bmp"; break;
		default:
			throw IOException( "Unknown format for snapshot: " + std::to_string( int( p_format ) ) );
			extension = "";
			break;
		}

		const std::string filename = std::to_string( Util::Chrono::getTimestamp() ) + '.' + extension;
		std::filesystem::create_directory( getSnapshotsDir() );

		FilePath path = FilePath( getSnapshotsDir() / filename );
		Util::Filesystem::generateUniqueFileName( path );

		return path;
	}

	static const std::string MOLECULE_EXTENSIONS_READ
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

	static const std::string MOLECULE_EXTENSIONS_WRITE
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

	static const std::string TRAJECTORY_EXTENSIONS_READ
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

	static const std::string LOAD_MOLECULE_FILTERS			  = MOLECULE_EXTENSIONS_READ;
	static const std::string EXPORT_MOLECULE_FILTERS		  = MOLECULE_EXTENSIONS_WRITE;
	static const std::string OPEN_FILE_FILTERS				  = VTX_EXTENSIONS + ";;" + MOLECULE_EXTENSIONS_READ;
	static const std::string SAVE_SCENE_FILTERS				  = VTX_EXTENSIONS;
	static const std::string DEFAULT_MOLECULE_READ_FILTER	  = "All (*)";
	static const std::string DEFAULT_MOLECULE_WRITE_FILTER	  = "MMCIF(*.mmcif)";
	static const std::string DEFAULT_MOLECULE_WRITE_EXTENSION = "mmcif";

	static const std::string LOAD_TRAJECTORY_FILTERS		= TRAJECTORY_EXTENSIONS_READ;
	static const std::string DEFAULT_TRAJECTORY_READ_FILTER = "All (*)";

	static const std::string DEFAULT_FILE_READ_FILTER  = "All (*)";
	static const std::string DEFAULT_FILE_WRITE_FILTER = VTX_EXTENSIONS;

	static const std::string REPRESENTATION_PRESET_FILE_FILTERS = "Representation file (*)";
	static const std::string RENDER_EFFECT_PRESET_FILE_FILTERS	= "Render effect file (*)";

	static const FilePath STYLESHEET_FILE_DEFAULT = FilePath( ":/stylesheet_ui.css" );
	static const FilePath STYLESHEET_FILE_WINDOWS = FilePath( ":/stylesheet_windows.css" );
	static const FilePath STYLESHEET_FILE_LINUX	  = FilePath( ":/stylesheet_linux.css" );
	static const FilePath SCENE_OBJECT_DIR		  = FilePath( "obj" );

	static const std::string DEFAULT_SCENE_FILENAME	   = "New Scene";
	static const std::string DEFAULT_MOLECULE_FILENAME = "New Molecule";

	inline FilePath getDefaultMoleculeExportPath()
	{
		const FilePath defaultFolder = FilePath( Setting::getLastExportedMoleculeFolder() );

		const int nbMoleculeInSelection
			= Selection::SelectionManager::get().getSelectionModel().getMoleculeSelectedCount();

		const App::Component::Chemistry::Molecule * exportedMolecule;
		if ( nbMoleculeInSelection > 0 )
		{
			const App::Core::Model::ID & moleculeID
				= Selection::SelectionManager::get().getSelectionModel().getMoleculesMap().begin()->first;
			exportedMolecule = &( VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Molecule>( moleculeID ) );
		}
		else if ( VTXApp::get().getScene().getMolecules().size() > 0 )
		{
			exportedMolecule = VTXApp::get().getScene().getMolecules().begin()->first;
		}
		else
		{
			exportedMolecule = nullptr;
		}

		std::string filename
			= exportedMolecule == nullptr ? DEFAULT_MOLECULE_FILENAME : exportedMolecule->getDisplayName();
		filename = filename + '.' + DEFAULT_MOLECULE_WRITE_EXTENSION;

		return defaultFolder / filename;
	}

	inline FilePath getDefaultSceneSavePath()
	{
		if ( !VTXApp::get().getScenePathData().getCurrentPath().empty() )
		{
			return VTXApp::get().getScenePathData().getCurrentPath();
		}

		const FilePath defaultFolder = FilePath( Setting::getLastSavedSessionFolder() );
		FilePath	   defaultPath	 = defaultFolder / ( DEFAULT_SCENE_FILENAME + ".vtx" );

		Util::Filesystem::generateUniqueFileName( defaultPath );

		return defaultPath;
	}

	inline std::string getImageExportDefaultFilter()
	{
		std::string filter;
		switch ( VTX_SETTING().getSnapshotFormat() )
		{
		case IO::Struct::ImageExport::Format::PNG: filter = "PNG (*.png)"; break;
		case IO::Struct::ImageExport::Format::JPEG: filter = "JPEG (*.jpg *.jpeg)"; break;
		case IO::Struct::ImageExport::Format::BMP: filter = "BMP (*.bmp)"; break;
		default: filter = "All (*)"; break;
		}

		return filter;
	}
} // namespace VTX::IO::Filesystem

#endif
