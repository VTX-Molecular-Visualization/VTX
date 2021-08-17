#ifndef __VTX_UTIL_FILESYSTEM__
#define __VTX_UTIL_FILESYSTEM__

#include "exception.hpp"
#include "io/filepath.hpp"
#include "tool/logger.hpp"
#include <QCoreApplication>
#include <QDir>
#include <QDirIterator>
#include <QTextStream>
#include <set>

namespace VTX
{
	namespace Util
	{
		namespace Filesystem
		{
			inline const IO::FilePath getExecutableFile()
			{
				return QCoreApplication::applicationFilePath().toStdString();
			}
			inline const IO::FilePath getExecutableDir()
			{
				QDir dir( QCoreApplication::applicationDirPath() );
				dir.cdUp();
#ifdef _MSC_VER
				dir.cdUp();
#endif
				return dir.path().toStdString();
			}

			// Directories.
			inline const IO::FilePath getShadersDir() { return getExecutableDir() + "/shaders"; }
			inline const IO::FilePath getSnapshotsDir() { return getExecutableDir() + "/snapshots"; }
			inline const IO::FilePath getRendersDir() { return getExecutableDir() + "/renders"; }
			inline const IO::FilePath getLogsDir() { return getExecutableDir() + "/logs"; }
			inline const IO::FilePath getLibrariesDir() { return getExecutableDir() + "/libraries"; }
			inline const IO::FilePath getRepresentationsLibraryDir() { return getLibrariesDir() + "/representations"; }
			inline const IO::FilePath getRenderEffectPresetsLibraryDir()
			{
				return getLibrariesDir() + "/render_effects";
			}
			inline const IO::FilePath getInternalDataDir() { return getExecutableDir() + "/data"; }
			inline const IO::FilePath getResidueDataDir() { return getInternalDataDir() + "/residue_data"; }

			// Files.
			inline const IO::FilePath getConfigIniFile() { return getExecutableDir() + "/config.ini"; }
			inline const IO::FilePath getSettingJsonFile() { return getExecutableDir() + "/setting.json"; }
			inline const IO::FilePath getLicenseFile() { return getExecutableDir() + "/license.txt"; }

			// Dev directories.
			static const IO::FilePath DATA_DIR				  = IO::FilePath( "../data" );
			static const IO::FilePath SHADERS_DIR_SRC		  = IO::FilePath( "../src/shader" );
			static const IO::FilePath DEFAULT_SAVE_FOLDER	  = IO::FilePath( "../save" );
			static const IO::FilePath DEFAULT_MOLECULE_FOLDER = IO::FilePath( "../data" );

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

			static const QString VTX_EXTENSIONS = "VTX file (*.vtx)";

			static const QString LOAD_MOLECULE_FILTERS	 = MOLECULE_EXTENSIONS_READ;
			static const QString EXPORT_MOLECULE_FILTERS = MOLECULE_EXTENSIONS_WRITE;
			static const QString OPEN_FILE_FILTERS		 = VTX_EXTENSIONS + ";;" + MOLECULE_EXTENSIONS_READ;
			static const QString SAVE_SCENE_FILTERS		 = VTX_EXTENSIONS;

			static const QString DEFAULT_MOLECULE_READ_FILTER  = "All (*)";
			static const QString DEFAULT_MOLECULE_WRITE_FILTER = "MMCIF(*.mmcif)";
			static const QString DEFAULT_FILE_READ_FILTER	   = "All (*)";
			static const QString DEFAULT_FILE_WRITE_FILTER	   = VTX_EXTENSIONS;

			static const QString REPRESENTATION_PRESET_FILE_FILTERS = "Representation file (*)";
			static const QString RENDER_EFFECT_PRESET_FILE_FILTERS	= "Render effect file (*)";

			static const IO::FilePath STYLESHEET_FILE_DEFAULT = ":/stylesheet.css";
			static const IO::FilePath SCENE_OBJECT_DIR		  = "/obj";

			inline const IO::FilePath getDataPath( const IO::FilePath & p_filename )
			{
				return IO::FilePath( DATA_DIR / p_filename );
			}

			inline const IO::FilePath getShadersPath( const IO::FilePath & p_filename )
			{
				return IO::FilePath( getShadersDir() / p_filename );
			}

			inline const IO::FilePath getSnapshotsPath( const IO::FilePath & p_filename )
			{
				return IO::FilePath( getSnapshotsDir() / p_filename );
			}

			inline const IO::FilePath getRendersPath( const IO::FilePath & p_filename )
			{
				return IO::FilePath( getRendersDir() / p_filename );
			}

			inline const IO::FilePath getLogsPath( const IO::FilePath & p_filename )
			{
				return IO::FilePath( getLogsDir() / p_filename );
			}

			inline const IO::FilePath getRepresentationPath( const IO::FilePath & p_filename )
			{
				return IO::FilePath( getRepresentationsLibraryDir() / p_filename );
			}

			inline const IO::FilePath getRenderEffectPath( const IO::FilePath & p_filename )
			{
				return IO::FilePath( getRenderEffectPresetsLibraryDir() / p_filename );
			}

			inline void createDirectory( const IO::FilePath & p_filePath )
			{
				if ( QDir( p_filePath.qpath() ).exists() == false )
				{
					QDir().mkpath( p_filePath.qpath() );
				}
			}

			inline bool isSessionFile( const IO::FilePath & p_filePath ) { return p_filePath.extension() == "vtx"; }

			inline const std::string readPath( const IO::FilePath & p_filePath )
			{
				QFile file( p_filePath.qpath() );
				if ( file.open( QIODevice::ReadOnly | QIODevice::Text ) == false )
				{
					throw Exception::IOException( "Cannot open file " + p_filePath.path() + " : "
												  + file.errorString().toStdString() );
				}

				QTextStream out( &file );
				std::string content = out.readAll().toStdString();
				file.close();
				return content;
			}

			inline void generateUniqueFileName( IO::FilePath & p_filePath )
			{
				uint counter = 2;

				const std::string defaultFileName = p_filePath.filenameWithoutExtension();
				const std::string extension		  = p_filePath.extension();

				QDir dir = QDir( p_filePath.qpath() );

				while ( QFileInfo( p_filePath.qpath() ).exists() )
				{
					p_filePath = dir.absolutePath().toStdString() + defaultFileName + " " + std::to_string( counter )
								 + extension;
					counter++;
				}
			}

			inline bool copyFile( const IO::FilePath & p_from, const IO::FilePath & p_to )
			{
				try
				{
					return QFile::copy( p_from.qpath(), p_to.qpath() );
				}
				catch ( std::exception p_e )
				{
					VTX_WARNING( "Cannot copy file " + p_from.path() + " to " + p_to.path() + " : " + p_e.what() );
					return false;
				}
			}

			inline void copyDir( const IO::FilePath & p_from, const IO::FilePath & p_to ) { createDirectory( p_to ); }

			inline bool removeAll( const IO::FilePath & p_directory )
			{
				try
				{
					QDir dir( p_directory.qpath() );
					return dir.removeRecursively();
				}
				catch ( const std::exception & p_e )
				{
					VTX_ERROR( "Error when clear directory " + p_directory.path() + " : " + p_e.what() );
					return false;
				}
			}

			inline std::set<IO::FilePath> getFilesInDirectory( const IO::FilePath & p_directory )
			{
				std::set<IO::FilePath> result = std::set<IO::FilePath>();
				QDir				   dir( p_directory.qpath() );
				QStringList			   list = dir.entryList( QDir::Files );

				for ( const QString & entry : list )
				{
					IO::FilePath path = p_directory / IO::FilePath( entry.toStdString() );
					result.emplace( path );
				}

				return result;
			}

			inline bool remove( const IO::FilePath & p_filename )
			{
				try
				{
					return QFile::remove( p_filename.qpath() );
				}
				catch ( const std::exception & p_e )
				{
					VTX_ERROR( "Error when removing file " + p_filename.path() + " : " + p_e.what() );
					return false;
				}
			}

			inline IO::FilePath getParentDir( const IO::FilePath & p_path )
			{
				QDir dir( p_path.qpath() );
				dir.cdUp();
				return dir.path().toStdString();
			}

			inline IO::FilePath getSceneSaveDirectory( const IO::FilePath & p_savePath )
			{
				const IO::FilePath projectDirectoryName
					= IO::FilePath( p_savePath.filenameWithoutExtension() + "_data" );

				return getParentDir( p_savePath ) / projectDirectoryName;
			}

			inline IO::FilePath getSceneObjectsSaveDirectory( const IO::FilePath & p_savePath )
			{
				return getSceneSaveDirectory( p_savePath ) / SCENE_OBJECT_DIR;
			}

			inline void checkSaveDirectoryHierarchy( const IO::FilePath & p_savePath )
			{
				const IO::FilePath projectDirectory = getSceneSaveDirectory( p_savePath );
				if ( projectDirectory.exists() == false )
				{
					createDirectory( projectDirectory );
				}

				const IO::FilePath objectsPath = getSceneObjectsSaveDirectory( p_savePath );
				if ( objectsPath.exists() == false )
				{
					createDirectory( objectsPath );
				}
			}

			inline IO::FilePath getResidueDataFilePath( const IO::FilePath & p_residueName )
			{
				return getResidueDataDir() / p_residueName.path().substr( 0, 1 );
			}

		} // namespace Filesystem
	}	  // namespace Util
} // namespace VTX

#endif
