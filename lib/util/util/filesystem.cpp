#include "filesystem.hpp"
#include "exceptions.hpp"
#include <fstream>

namespace VTX::Util::Filesystem
{

	const std::string readPath( const FilePath & p_filePath )
	{
		std::ifstream inputFile( p_filePath );

		if ( inputFile.is_open() )
		{
			std::string fileContent( ( std::istreambuf_iterator<char>( inputFile ) ),
									 std::istreambuf_iterator<char>() );

			return fileContent;
		}
		else
		{
			throw IOException( "Can not read " + p_filePath.string() );
		}
	}

	void generateUniqueFileName( FilePath & p_filePath )
	{
		FilePath parentPath = p_filePath.parent_path();
		uint	 counter	= 2;
		while ( std::filesystem::exists( p_filePath ) )
		{
			p_filePath
				= parentPath
				  / p_filePath.stem().append( "_" + std::to_string( counter ) + "." + p_filePath.extension().string() );
			counter++;
		}
	}

	void removeAll( const FilePath & p_filePath )
	{
		std::filesystem::remove_all( p_filePath.has_filename() ? p_filePath.parent_path() : p_filePath );
	}

} // namespace VTX::Util::Filesystem

/*
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
	bool copyFile( const FilePath & p_from, const FilePath & p_to )
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



	const QString readPathQString( const FilePath & p_filePath, const QStringConverter::Encoding & p_codecName )
	{
		QString res;
		readPathQString( p_filePath, res, p_codecName );
		return res;
	}
	void readPathQString( const FilePath &			 p_filePath,
						  QString &							 p_content,
						  const QStringConverter::Encoding & p_codecName )
	{
		QFile file( QString::fromStdString( p_filePath ) );
		if ( file.open( QIODevice::ReadOnly | QIODevice::Text ) == false )
		{
			throw Exception::IOException( "Cannot open file " + p_filePath + " : "
										  + file.errorString().toStdString() );
		}

		QTextStream out( &file );

		out.setEncoding( p_codecName );

		p_content = out.readAll();
		file.close();
	}

	bool remove( const FilePath & p_filename )
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

	bool removeAll( const FilePath & p_directory )
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

	void copyDir( const FilePath & p_from, const FilePath & p_to )
	{
		createDirectory( p_to );

		QDir		dir( QString::fromStdString( p_from ) );
		QStringList list = dir.entryList( QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot );

		for ( const QString & entry : list )
		{
			FilePath pathFrom = p_from / FilePath( entry.toStdString() );
			FilePath pathTo	= p_to / FilePath( entry.toStdString() );

			QFileInfo fi( QString::fromStdString( pathFrom ) );

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

	std::set<FilePath> getFilesInDirectory( const FilePath & p_directory )
	{
		std::set<FilePath> result = std::set<FilePath>();
		QDir					 dir( QString::fromStdString( p_directory ) );
		QStringList				 list = dir.entryList( QDir::Files );

		for ( const QString & entry : list )
		{
			FilePath path = p_directory / FilePath( entry.toStdString() );
			result.emplace( path );
		}

		return result;
	}

	void checkSaveDirectoryHierarchy( const FilePath & p_savePath )
	{
		const FilePath projectDirectory = getSceneSaveDirectory( p_savePath );
		if ( projectDirectory.exists() == false )
		{
			createDirectory( projectDirectory );
		}

		const FilePath objectsPath = getSceneObjectsSaveDirectory( p_savePath );
		if ( objectsPath.exists() == false )
		{
			createDirectory( objectsPath );
		}
	}

	FilePath getDefaultMoleculeExportPath()
	{
		const FilePath defaultFolder = FilePath( Setting::getLastExportedMoleculeFolder().toStdString() );

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

	FilePath getDefaultSceneSavePath()
	{
		if ( !VTXApp::get().getScenePathData().getCurrentPath().empty() )
		{
			return VTXApp::get().getScenePathData().getCurrentPath();
		}

		const FilePath defaultFolder = FilePath( Setting::getLastSavedSessionFolder().toStdString() );
		FilePath		 defaultPath   = defaultFolder / ( DEFAULT_SCENE_FILENAME + ".vtx" );

		generateUniqueFileName( defaultPath );

		return defaultPath;
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
*/
