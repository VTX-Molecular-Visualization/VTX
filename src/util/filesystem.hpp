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

			static const QString DEFAULT_MOLECULE_READ_FILTER	  = "All (*)";
			static const QString DEFAULT_MOLECULE_WRITE_FILTER	  = "MMCIF(*.mmcif)";
			static const QString DEFAULT_MOLECULE_WRITE_EXTENSION = DEFAULT_MOLECULE_WRITE_FILTER.mid(
				DEFAULT_MOLECULE_WRITE_FILTER.lastIndexOf( '.' ) + 1,
				DEFAULT_MOLECULE_WRITE_FILTER.size() - DEFAULT_MOLECULE_WRITE_FILTER.lastIndexOf( '.' ) - 2 );

			static const QString DEFAULT_FILE_READ_FILTER  = "All (*)";
			static const QString DEFAULT_FILE_WRITE_FILTER = VTX_EXTENSIONS;

			static const QString REPRESENTATION_PRESET_FILE_FILTERS = "Representation file (*)";
			static const QString RENDER_EFFECT_PRESET_FILE_FILTERS	= "Render effect file (*)";

			static const IO::FilePath STYLESHEET_FILE_DEFAULT = IO::FilePath( ":/stylesheet.css" );
			static const IO::FilePath SCENE_OBJECT_DIR		  = IO::FilePath( "/obj" );

			static const std::string DEFAULT_SCENE_FILENAME	   = "New Scene";
			static const std::string DEFAULT_MOLECULE_FILENAME = "New Molecule";

			IO::FilePath	  getParentDir( const IO::FilePath & p_path );
			void			  createDirectory( const IO::FilePath & p_filePath );
			bool			  copyFile( const IO::FilePath & p_from, const IO::FilePath & p_to );
			const std::string readPath( const IO::FilePath & p_filePath, const char * p_codecName = nullptr );
			void			  readPath( const IO::FilePath & p_filePath,
										std::string &		 p_content,
										const char *		 p_codecName = nullptr );
			const QString	  readPathQString( const IO::FilePath & p_filePath, const char * p_codecName = nullptr );
			void			  readPathQString( const IO::FilePath & p_filePath,
											   QString &			p_content,
											   const char *			p_codecName = nullptr );
			bool			  remove( const IO::FilePath & p_filename );
			bool			  removeAll( const IO::FilePath & p_directory );
			void			  copyDir( const IO::FilePath & p_from, const IO::FilePath & p_to );
			std::set<IO::FilePath> getFilesInDirectory( const IO::FilePath & p_directory );
			void				   generateUniqueFileName( IO::FilePath & p_filePath );
			void				   checkSaveDirectoryHierarchy( const IO::FilePath & p_savePath );
			IO::FilePath		   getDefaultMoleculeExportPath();
			IO::FilePath		   getDefaultSceneSavePath();

			inline const IO::FilePath getDataPath( const IO::FilePath & p_filename )
			{
				return IO::FilePath( DATA_DIR / p_filename );
			}

			inline const IO::FilePath getShadersPath( const IO::FilePath & p_filename )
			{
				createDirectory( getShadersDir() );
				return IO::FilePath( getShadersDir() / p_filename );
			}

			inline const IO::FilePath getSnapshotsPath( const IO::FilePath & p_filename )
			{
				createDirectory( getSnapshotsDir() );
				return IO::FilePath( getSnapshotsDir() / p_filename );
			}

			inline const IO::FilePath getRendersPath( const IO::FilePath & p_filename )
			{
				createDirectory( getRendersDir() );
				return IO::FilePath( getRendersDir() / p_filename );
			}

			inline const IO::FilePath getLogsPath( const IO::FilePath & p_filename )
			{
				createDirectory( getLogsDir() );
				return IO::FilePath( getLogsDir() / p_filename );
			}

			inline const IO::FilePath getRepresentationPath( const IO::FilePath & p_filename )
			{
				createDirectory( getRepresentationsLibraryDir() );
				return IO::FilePath( getRepresentationsLibraryDir() / p_filename );
			}

			inline const IO::FilePath getRenderEffectPath( const IO::FilePath & p_filename )
			{
				createDirectory( getRenderEffectPresetsLibraryDir() );
				return IO::FilePath( getRenderEffectPresetsLibraryDir() / p_filename );
			}

			inline bool isSessionFile( const IO::FilePath & p_filePath ) { return p_filePath.extension() == "vtx"; }

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

			inline IO::FilePath getResidueDataFilePath( const IO::FilePath & p_residueName )
			{
				return getResidueDataDir() / p_residueName.path().substr( 0, 1 );
			}

		} // namespace Filesystem
	}	  // namespace Util
} // namespace VTX

#endif
