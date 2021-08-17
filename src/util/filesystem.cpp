#include "filesystem.hpp"
#include "io/scene_path_data.hpp"
#include "model/molecule.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "object3d/scene.hpp"
#include "selection/selection_manager.hpp"
#include "vtx_app.hpp"

namespace VTX::Util::Filesystem
{
	IO::FilePath getParentDir( const IO::FilePath & p_path )
	{
		QDir dir( p_path.qpath() );
		dir.cdUp();
		return dir.path().toStdString();
	}

	void createDirectory( const IO::FilePath & p_filePath )
	{
		if ( QDir( p_filePath.qpath() ).exists() == false )
		{
			QDir().mkpath( p_filePath.qpath() );
		}
	}

	bool copyFile( const IO::FilePath & p_from, const IO::FilePath & p_to )
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

	const std::string readPath( const IO::FilePath & p_filePath )
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

	bool remove( const IO::FilePath & p_filename )
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

	bool removeAll( const IO::FilePath & p_directory )
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

	void copyDir( const IO::FilePath & p_from, const IO::FilePath & p_to ) { createDirectory( p_to ); }

	std::set<IO::FilePath> getFilesInDirectory( const IO::FilePath & p_directory )
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

	void checkSaveDirectoryHierarchy( const IO::FilePath & p_savePath )
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

	void generateUniqueFileName( IO::FilePath & p_filePath )
	{
		uint counter = 2;

		const std::string defaultFileName = p_filePath.filenameWithoutExtension();
		const std::string extension		  = p_filePath.extension();

		QDir dir = QDir( p_filePath.qpath() );
		dir.cdUp();

		while ( QFileInfo( p_filePath.qpath() ).exists() )
		{
			p_filePath = dir.filePath( QString::fromStdString( defaultFileName + " " + std::to_string( counter )
															   + extension ) )
							 .toStdString();
			counter++;
		}
	}

	IO::FilePath getDefaultMoleculeExportPath()
	{
		const IO::FilePath defaultFolder = IO::FilePath( Setting::getLastExportedMoleculeFolder().toStdString() );

		const Model::Molecule * exportedMolecule = nullptr;
		const int				nbMoleculeInSelection
			= Selection::SelectionManager::get().getSelectionModel().getMoleculeSelectedCount();
		if ( nbMoleculeInSelection == 1 )
		{
			const Model::ID & moleculeID
				= Selection::SelectionManager::get().getSelectionModel().getItems().begin()->first;
			exportedMolecule = &( MVC::MvcManager::get().getModel<Model::Molecule>( moleculeID ) );
		}
		else if ( nbMoleculeInSelection == 0 && VTXApp::get().getScene().getMolecules().size() == 1 )
		{
			exportedMolecule = VTXApp::get().getScene().getMolecules().begin()->first;
		}

		std::string filename
			= exportedMolecule == nullptr ? DEFAULT_MOLECULE_FILENAME : exportedMolecule->getDisplayName();
		filename = filename + '.' + Util::Filesystem::DEFAULT_MOLECULE_WRITE_EXTENSION.toStdString();

		return defaultFolder / filename;
	}

	IO::FilePath getDefaultSceneSavePath()
	{
		if ( !VTXApp::get().getScenePathData().getCurrentPath().empty() )
		{
			return VTXApp::get().getScenePathData().getCurrentPath();
		}

		const IO::FilePath defaultFolder = IO::FilePath( Setting::getLastSavedSessionFolder().toStdString() );
		IO::FilePath	   defaultPath	 = defaultFolder / ( DEFAULT_SCENE_FILENAME + ".vtx" );

		generateUniqueFileName( defaultPath );

		return defaultPath;
	}

} // namespace VTX::Util::Filesystem
