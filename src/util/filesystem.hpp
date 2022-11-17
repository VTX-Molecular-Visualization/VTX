#ifndef __VTX_UTIL_FILESYSTEM__
#define __VTX_UTIL_FILESYSTEM__

#include "exception.hpp"
#include "tool/logger.hpp"
#include <QCoreApplication>
#include <QDir>
#include <QDirIterator>
#include <QTextStream>
#include <lib/util/src/filepath.hpp>
#include <lib/util/src/time.hpp>
#include <set>

namespace VTX
{
	namespace Util
	{
		namespace Filesystem
		{
			inline const Util::FilePath getExecutableFile()
			{
				return QCoreApplication::applicationFilePath().toStdString();
			}
			inline const Util::FilePath getExecutableDir()
			{
				QDir dir( QCoreApplication::applicationDirPath() );
				dir.cdUp();
#ifdef _MSC_VER
				dir.cdUp();
#endif
				return dir.path().toStdString();
			}

			// Directories.
			inline const Util::FilePath getShadersDir() { return getExecutableDir() + "/shaders"; }
			inline const Util::FilePath getSnapshotsDir() { return getExecutableDir() + "/snapshots"; }
			inline const Util::FilePath getVideosDir() { return getExecutableDir() + "/videos"; }
			inline const Util::FilePath getRendersDir() { return getExecutableDir() + "/renders"; }
			inline const Util::FilePath getLogsDir() { return getExecutableDir() + "/logs"; }
			inline const Util::FilePath getLibrariesDir() { return getExecutableDir() + "/libraries"; }
			inline const Util::FilePath getRepresentationsLibraryDir()
			{
				return getLibrariesDir() + "/representations";
			}
			inline const Util::FilePath getRenderEffectPresetsLibraryDir()
			{
				return getLibrariesDir() + "/render_effects";
			}
			inline const Util::FilePath getInternalDataDir() { return getExecutableDir() + "/data"; }
			inline const Util::FilePath getResidueDataDir() { return getInternalDataDir() + "/residue_data"; }

			// Files.
			inline const Util::FilePath getConfigIniFile() { return getExecutableDir() + "/config.ini"; }
			inline const Util::FilePath getSettingJsonFile() { return getExecutableDir() + "/setting.json"; }
			inline const Util::FilePath getLicenseFile() { return getExecutableDir() + "/license.txt"; }
			inline const Util::FilePath getDefaultSnapshotsDir() { return getExecutableDir() + "/snapshots"; }

			// Dev directories.
			static const Util::FilePath DATA_DIR				= Util::FilePath( "../data" );
			static const Util::FilePath SHADERS_DIR_SRC			= Util::FilePath( "../src/shader" );
			static const Util::FilePath DEFAULT_SAVE_FOLDER		= Util::FilePath( "../save" );
			static const Util::FilePath DEFAULT_MOLECULE_FOLDER = Util::FilePath( "../data" );

			// JSon save fail when size > 192
			static const int	 MAX_FILE_LENGTH	  = 180;
			static const QString REGEX_VALID_FILENAME = "[^\\\\/:*?\"<>|]*";

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

			static const QString IMAGE_EXPORT_EXTENSIONS
				= "PNG (*.png);;"
				  "JPEG (*.jpg *.jpeg);;"
				  "BMP (*.bmp);;"
				  "All (*)";

			static const QString VTX_EXTENSIONS = "VTX file (*.vtx)";

			static const QString LOAD_MOLECULE_FILTERS	 = MOLECULE_EXTENSIONS_READ;
			static const QString EXPORT_MOLECULE_FILTERS = MOLECULE_EXTENSIONS_WRITE;
			static const QString OPEN_FILE_FILTERS		 = VTX_EXTENSIONS + ";;" + MOLECULE_EXTENSIONS_READ;
			static const QString SAVE_SCENE_FILTERS		 = VTX_EXTENSIONS;

			static const QString DEFAULT_MOLECULE_READ_FILTER	  = "All (*)";
			static const QString DEFAULT_MOLECULE_WRITE_FILTER	  = "MMCIF(*.mmcif)";
			static const QString DEFAULT_MOLECULE_WRITE_EXTENSION = DEFAULT_MOLECULE_WRITE_FILTER.mid(
				DEFAULT_MOLECULE_WRITE_FILTER.lastIndexOf( '.' ) + 1,
				DEFAULT_MOLECULE_WRITE_FILTER.size() - DEFAULT_MOLECULE_WRITE_FILTER.lastIndexOf( '.' ) - 2 );

			static const QString LOAD_TRAJECTORY_FILTERS		= TRAJECTORY_EXTENSIONS_READ;
			static const QString DEFAULT_TRAJECTORY_READ_FILTER = "All (*)";

			static const QString DEFAULT_FILE_READ_FILTER  = "All (*)";
			static const QString DEFAULT_FILE_WRITE_FILTER = VTX_EXTENSIONS;

			static const QString REPRESENTATION_PRESET_FILE_FILTERS = "Representation file (*)";
			static const QString RENDER_EFFECT_PRESET_FILE_FILTERS	= "Render effect file (*)";

			static const Util::FilePath STYLESHEET_FILE_DEFAULT = Util::FilePath( ":/stylesheet.css" );
			static const Util::FilePath SCENE_OBJECT_DIR		= Util::FilePath( "obj" );

			static const std::string DEFAULT_SCENE_FILENAME	   = "New Scene";
			static const std::string DEFAULT_MOLECULE_FILENAME = "New Molecule";

			Util::FilePath			 getParentDir( const Util::FilePath & p_path );
			void					 createDirectory( const Util::FilePath & p_filePath );
			bool					 copyFile( const Util::FilePath & p_from, const Util::FilePath & p_to );
			const std::string		 readPath( const Util::FilePath & p_filePath, const char * p_codecName = nullptr );
			void					 readPath( const Util::FilePath & p_filePath,
											   std::string &		  p_content,
											   const char *			  p_codecName = nullptr );
			const QString			 readPathQString( const Util::FilePath &			 p_filePath,
													  const QStringConverter::Encoding & p_codecName
													  = QStringConverter::Encoding::Utf8 );
			void					 readPathQString( const Util::FilePath &			 p_filePath,
													  QString &							 p_content,
													  const QStringConverter::Encoding & p_codecName = QStringConverter::Encoding::Utf8 );
			bool					 remove( const Util::FilePath & p_filename );
			bool					 removeAll( const Util::FilePath & p_directory );
			void					 copyDir( const Util::FilePath & p_from, const Util::FilePath & p_to );
			std::set<Util::FilePath> getFilesInDirectory( const Util::FilePath & p_directory );
			void					 generateUniqueFileName( Util::FilePath & p_filePath );
			void					 checkSaveDirectoryHierarchy( const Util::FilePath & p_savePath );
			Util::FilePath			 getDefaultMoleculeExportPath();
			Util::FilePath			 getDefaultSceneSavePath();

			inline const Util::FilePath getDataPath( const Util::FilePath & p_filename )
			{
				return Util::FilePath( DATA_DIR / p_filename );
			}

			inline const Util::FilePath getShadersPath( const Util::FilePath & p_filename )
			{
				createDirectory( getShadersDir() );
				return Util::FilePath( getShadersDir() / p_filename );
			}

			inline const Util::FilePath getSnapshotsPath( const Util::FilePath & p_filename )
			{
				createDirectory( getSnapshotsDir() );
				return Util::FilePath( getSnapshotsDir() / p_filename );
			}
			const Util::FilePath getUniqueSnapshotsPath();
			std::string			 getImageExportDefaultFilter();

			inline const Util::FilePath getVideosPath( const Util::FilePath & p_filename )
			{
				createDirectory( getVideosDir() );

				Util::FilePath path = Util::FilePath( getVideosDir() / p_filename );
				generateUniqueFileName( path );

				return path;
			}

			inline const Util::FilePath getRendersPath( const Util::FilePath & p_filename )
			{
				createDirectory( getRendersDir() );
				return Util::FilePath( getRendersDir() / p_filename );
			}

			inline const Util::FilePath getLogsPath( const Util::FilePath & p_filename )
			{
				createDirectory( getLogsDir() );
				return Util::FilePath( getLogsDir() / p_filename );
			}

			inline const Util::FilePath getRepresentationPath( const Util::FilePath & p_filename )
			{
				createDirectory( getRepresentationsLibraryDir() );
				return Util::FilePath( getRepresentationsLibraryDir() / p_filename );
			}

			inline const Util::FilePath getRenderEffectPath( const Util::FilePath & p_filename )
			{
				createDirectory( getRenderEffectPresetsLibraryDir() );
				return Util::FilePath( getRenderEffectPresetsLibraryDir() / p_filename );
			}

			inline bool isSessionFile( const Util::FilePath & p_filePath ) { return p_filePath.extension() == "vtx"; }

			inline Util::FilePath getSceneSaveDirectory( const Util::FilePath & p_savePath )
			{
				const Util::FilePath projectDirectoryName
					= Util::FilePath( p_savePath.filenameWithoutExtension() + "_data" );

				return getParentDir( p_savePath ) / projectDirectoryName;
			}

			inline Util::FilePath getSceneObjectsSaveDirectory( const Util::FilePath & p_savePath )
			{
				return getSceneSaveDirectory( p_savePath ) / SCENE_OBJECT_DIR;
			}

			inline Util::FilePath getResidueDataFilePath( const Util::FilePath & p_residueName )
			{
				return getResidueDataDir() / p_residueName.path().substr( 0, 1 );
			}

			inline bool isAbsolutePath( const Util::FilePath & p_path )
			{
				const std::string pathStr = p_path.path();
				return pathStr.size() >= 3 && pathStr[ 1 ] == ':' && ( pathStr[ 2 ] == '/' || pathStr[ 2 ] == '\\' );
			}
			inline bool isRelativePath( const Util::FilePath & p_path ) { return !isAbsolutePath( p_path ); }

		} // namespace Filesystem
	}	  // namespace Util
} // namespace VTX

#endif
