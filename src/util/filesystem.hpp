#ifndef __VTX_UTIL_FILESYSTEM__
#define __VTX_UTIL_FILESYSTEM__

#include "define.hpp"
#include "exception.hpp"
#include "tool/logger.hpp"
#include <QCoreApplication>
#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <set>

namespace VTX
{
	namespace Util
	{
		namespace Filesystem
		{
			inline const FilePath getExecutableFile() { return QCoreApplication::applicationFilePath().toStdString(); }
			inline const FilePath getExecutableDir()
			{
				QDir dir( QCoreApplication::applicationDirPath() );
				dir.cdUp();
#ifdef _MSC_VER
				dir.cdUp();
#endif
				return dir.path().toStdString();
			}

			// Directories.
			inline const FilePath getShadersDir() { return getExecutableDir() + "/shaders"; }
			inline const FilePath getSnapshotsDir() { return getExecutableDir() + "/snapshots"; }
			inline const FilePath getRendersDir() { return getExecutableDir() + "/renders"; }
			inline const FilePath getLogsDir() { return getExecutableDir() + "/logs"; }
			inline const FilePath getLibrariesDir() { return getExecutableDir() + "/libraries"; }
			inline const FilePath getRepresentationsLibraryDir() { return getLibrariesDir() + "/representations"; }
			inline const FilePath getRenderEffectPresetsLibraryDir() { return getLibrariesDir() + "/render_effects"; }
			inline const FilePath getInternalDataDir() { return getExecutableDir() + "/data"; }
			inline const FilePath getResidueDataDir() { return getInternalDataDir() + "/residue_data"; }

			// Files.
			inline const FilePath getConfigIniFile() { return getExecutableDir() + "/config.ini"; }
			inline const FilePath getSettingJsonFile() { return getExecutableDir() + "/setting.json"; }
			inline const FilePath getLicenseFile() { return getExecutableDir() + "/license.txt"; }

			// Dev directories.
			static const FilePath DATA_DIR				  = FilePath( "../data" );
			static const FilePath SHADERS_DIR_SRC		  = FilePath( "../src/shader" );
			static const FilePath DEFAULT_SAVE_FOLDER	  = FilePath( "../save" );
			static const FilePath DEFAULT_MOLECULE_FOLDER = FilePath( "../data" );

			// JSon save fail when size > 192
			static const int	  MAX_FILE_LENGTH	   = 180;
			static const FilePath REGEX_VALID_FILENAME = "[^\\\\/:*?\"<>|]*";

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

			static const FilePath STYLESHEET_FILE_DEFAULT = ":/stylesheet.css";
			static const FilePath SCENE_OBJECT_DIR		  = "/obj";

			inline const FilePath getDataPath( const FilePath & p_filename )
			{
				return FilePath( DATA_DIR + "/" + p_filename );
			}

			inline const FilePath getShadersPath( const FilePath & p_filename )
			{
				return FilePath( getShadersDir() + "/" + p_filename );
			}

			inline const FilePath getSnapshotsPath( const FilePath & p_filename )
			{
				return FilePath( getSnapshotsDir() + "/" + p_filename );
			}

			inline const FilePath getRendersPath( const FilePath & p_filename )
			{
				return FilePath( getRendersDir() + "/" + p_filename );
			}

			inline const FilePath getLogsPath( const FilePath & p_filename )
			{
				return FilePath( getLogsDir() + "/" + p_filename );
			}

			inline const FilePath getRepresentationPath( const FilePath & p_filename )
			{
				return FilePath( getRepresentationsLibraryDir() + "/" + p_filename );
			}

			inline const FilePath getRenderEffectPath( const FilePath & p_filename )
			{
				return FilePath( getRenderEffectPresetsLibraryDir() + "/" + p_filename );
			}

			inline void createDirectory( const FilePath & p_path )
			{
				if ( QDir( QString::fromStdString( p_path ) ).exists() == false )
				{
					QDir().mkpath( QString::fromStdString( p_path ) );
				}
			}

			inline std::string getFileName( const FilePath & p_filePath )
			{
				QFileInfo fi( QString::fromStdString( p_filePath ) );
				return fi.fileName().toStdString();
			}

			inline std::string getFileNameWithoutExtension( const FilePath & p_filePath )
			{
				QFileInfo fi( QString::fromStdString( p_filePath ) );
				return fi.baseName().toStdString();
			}

			inline std::string getFileExtension( const FilePath & p_filePath )
			{
				QFileInfo fi = QFileInfo( QString::fromStdString( p_filePath ) );
				return fi.suffix().toStdString();
			}

			inline bool isSessionFile( const FilePath & p_filePath ) { return getFileExtension( p_filePath ) == "vtx"; }

			inline const std::string readPath( const FilePath & p_filePath )
			{
				QFile file( QString::fromStdString( p_filePath ) );
				if ( file.open( QIODevice::ReadOnly | QIODevice::Text ) == false )
				{
					throw Exception::IOException( "Cannot open file " + p_filePath + " : "
												  + file.errorString().toStdString() );
				}

				QTextStream out( &file );
				std::string content = out.readAll().toStdString();
				file.close();
				return content;
			}

			inline void generateUniqueFileName( FilePath & p_filePath )
			{
				uint counter = 2;

				const std::string defaultFileName = getFileNameWithoutExtension( p_filePath );
				const std::string extension		  = getFileExtension( p_filePath );

				QDir dir = QDir( QString::fromStdString( p_filePath ) );

				while ( QFileInfo( QString::fromStdString( p_filePath ) ).exists() )
				{
					p_filePath = dir.absolutePath().toStdString() + defaultFileName + " " + std::to_string( counter )
								 + extension;
					counter++;
				}

				std::cout << p_filePath << std::endl;
			}

			inline bool copyFile( const FilePath & p_from, const FilePath & p_to )
			{
				try
				{
					return QFile::copy( QString::fromStdString( p_from ), QString::fromStdString( p_to ) );
				}
				catch ( std::exception p_e )
				{
					VTX_WARNING( "Cannot copy file " + p_from + " to " + p_to + " : " + p_e.what() );
					return false;
				}
			}

			inline void copyDir( const FilePath & p_from, const FilePath & p_to )
			{
				createDirectory( p_to );

				
			}

			inline bool removeAll( const FilePath & p_directory )
			{
				try
				{
					QDir dir( QString::fromStdString( p_directory ) );
					return dir.removeRecursively();
				}
				catch ( const std::exception & p_e )
				{
					VTX_ERROR( "Error when clear directory " + p_directory + " : " + p_e.what() );
					return false;
				}
			}

			inline std::set<FilePath> getFilesInDirectory( const FilePath & p_directory )
			{
				std::set<FilePath> result = std::set<FilePath>();
				QDir			   dir( QString::fromStdString( p_directory ) );
				QStringList		   list = dir.entryList( QDir::Files );

				for ( const QString & entry : list )
				{
					FilePath path = p_directory + "/" + entry.toStdString();
					result.emplace( path );
				}

				return result;
			}

			inline bool remove( const FilePath & p_filename )
			{
				try
				{
					return QFile::remove( QString::fromStdString( p_filename ) );
				}
				catch ( const std::exception & p_e )
				{
					VTX_ERROR( "Error when removing file " + p_filename + " : " + p_e.what() );
					return false;
				}
			}

			inline bool exists( const FilePath & p_path ) { return QFile::exists( QString::fromStdString( p_path ) ); }

			inline FilePath getParentDir( const FilePath & p_path )
			{
				QDir dir( QString::fromStdString( p_path ) );
				dir.cdUp();
				return dir.path().toStdString();
			}

			inline FilePath getSceneSaveDirectory( const FilePath & p_savePath )
			{
				const FilePath projectDirectoryName = FilePath( getFileNameWithoutExtension( p_savePath ) + "_data" );

				return getParentDir( p_savePath ) + "/" + projectDirectoryName;
			}

			inline FilePath getSceneObjectsSaveDirectory( const FilePath & p_savePath )
			{
				return getSceneSaveDirectory( p_savePath ) + "/" + SCENE_OBJECT_DIR;
			}

			inline void checkSaveDirectoryHierarchy( const FilePath & p_savePath )
			{
				const FilePath projectDirectory = getSceneSaveDirectory( p_savePath );
				if ( !Util::Filesystem::exists( projectDirectory ) )
					createDirectory( projectDirectory );

				const FilePath objectsPath = getSceneObjectsSaveDirectory( p_savePath );
				if ( !Util::Filesystem::exists( objectsPath ) )
					createDirectory( objectsPath );
			}

			inline FilePath getResidueDataFilePath( const FilePath & p_residueName )
			{
				return getResidueDataDir() + "/" + p_residueName.substr( 0, 1 );
			}

		} // namespace Filesystem
	}	  // namespace Util
} // namespace VTX

#endif
