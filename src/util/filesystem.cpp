#include "filesystem.hpp"
#include "io/struct/scene_path_data.hpp"
#include "model/molecule.hpp"
#include "model/selection.hpp"
#include "mvc/mvc_manager.hpp"
#include "object3d/scene.hpp"
#include "selection/selection_manager.hpp"
#include "setting.hpp"
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

	const std::string readPath( const IO::FilePath & p_filePath, const char * p_codecName )
	{
		QString qStrContent;
		readPathQString( p_filePath, qStrContent );
		return qStrContent.toStdString();
	}
	void readPath( const IO::FilePath & p_filePath, std::string & p_content, const char * p_codecName )
	{
		QString qStrContent;
		readPathQString( p_filePath, qStrContent );
		p_content = qStrContent.toStdString();
	}

	const QString readPathQString( const IO::FilePath & p_filePath, const char * p_codecName )
	{
		QString res;
		readPathQString( p_filePath, res, p_codecName );
		return res;
	}
	void readPathQString( const IO::FilePath & p_filePath, QString & p_content, const char * p_codecName )
	{
		QFile file( p_filePath.qpath() );
		if ( file.open( QIODevice::ReadOnly | QIODevice::Text ) == false )
		{
			throw Exception::IOException( "Cannot open file " + p_filePath.path() + " : "
										  + file.errorString().toStdString() );
		}

		QTextStream out( &file );

		if ( p_codecName != nullptr )
			out.setCodec( p_codecName );

		p_content = out.readAll();
		file.close();
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

	void copyDir( const IO::FilePath & p_from, const IO::FilePath & p_to )
	{
		createDirectory( p_to );

		QDir		dir( p_from.qpath() );
		QStringList list = dir.entryList( QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot );

		for ( const QString & entry : list )
		{
			IO::FilePath pathFrom = p_from / IO::FilePath( entry.toStdString() );
			IO::FilePath pathTo	  = p_to / IO::FilePath( entry.toStdString() );

			QFileInfo fi( pathFrom.qpath() );

			if ( fi.isFile() )
			{
				copyFile( pathFrom, pathTo );
			}
			else if ( fi.isDir() )
			{
				copyDir( pathFrom, pathTo );
			}
		}
	}

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
			std::string newPath = defaultFileName + " " + std::to_string( counter );

			if ( !extension.empty() )
				newPath = newPath + '.' + extension;

			p_filePath = dir.filePath( QString::fromStdString( newPath ) ).toStdString();
			counter++;
		}
	}

	IO::FilePath getDefaultMoleculeExportPath()
	{
		const IO::FilePath defaultFolder = IO::FilePath( Setting::getLastExportedMoleculeFolder().toStdString() );

		const int nbMoleculeInSelection
			= Selection::SelectionManager::get().getSelectionModel().getMoleculeSelectedCount();

		const Model::Molecule * exportedMolecule;
		if ( nbMoleculeInSelection > 0 )
		{
			const Model::ID & moleculeID
				= Selection::SelectionManager::get().getSelectionModel().getMoleculesMap().begin()->first;
			exportedMolecule = &( MVC::MvcManager::get().getModel<Model::Molecule>( moleculeID ) );
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

	const IO::FilePath getUniqueSnapshotsPath()
	{
		std::string extension;

		switch ( VTX_SETTING().getSnapshotFormat() )
		{
		case IO::Struct::ImageExport::Format::PNG: extension = "png"; break;
		case IO::Struct::ImageExport::Format::JPEG: extension = "jpg"; break;
		case IO::Struct::ImageExport::Format::BMP: extension = "bmp"; break;
		default:
			VTX_ERROR( "Unknown format for snapshot (" + std::to_string( int( VTX_SETTING().getSnapshotFormat() ) ) );
			extension = "";
			break;
		}

		const std::string filename = Util::Time::getTimestamp() + '.' + extension;
		createDirectory( getSnapshotsDir() );

		IO::FilePath path = IO::FilePath( getSnapshotsDir() / filename );
		generateUniqueFileName( path );

		return path;
	}

	std::string getImageExportDefaultFilter()
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

} // namespace VTX::Util::Filesystem
