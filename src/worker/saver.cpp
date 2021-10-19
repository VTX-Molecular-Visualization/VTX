#include "saver.hpp"
#include "io/scene_path_data.hpp"
#include "io/writer/serialized_object.hpp"
#include "io/writer/writer_chemfiles.hpp"
#include "model/molecule.hpp"
#include "mvc/mvc_manager.hpp"
#include "object3d/scene.hpp"
#include "selection/selection_manager.hpp"
#include "tool/chrono.hpp"
#include "tool/logger.hpp"
#include "util/filesystem.hpp"
#include <set>

namespace VTX::Worker
{
	uint Saver::_run()
	{
		bool		 result = 1;
		Tool::Chrono chrono;

		chrono.start();
		emit logInfo( "Saving " + _path.filename() );

		const MODE mode = _getMode( _path );

		switch ( mode )
		{
		case MODE::UNKNOWN: emit logError( "Format not supported" ); break;
		case MODE::MOLECULE: result = _saveMolecule(); break;
		case MODE::VTX: result = _saveSession(); break;

		default: emit logError( "Mode " + std::to_string( int( mode ) ) + "  not supported" ); break;
		}

		chrono.stop();
		emit logInfo( "File treated in " + std::to_string( chrono.elapsedTime() ) + "s" );

		return result;
	}

	bool Saver::_saveMolecule()
	{
		bool						  result = 1;
		IO::Writer::ChemfilesWriter * writer = new IO::Writer::ChemfilesWriter( this );

		// Write.
		try
		{
			// if selection is not empty -> export selected structures
			if ( !VTX::Selection::SelectionManager::get().getSelectionModel().getItems().empty() )
			{
				for ( const auto it : VTX::Selection::SelectionManager::get().getSelectionModel().getItems() )
				{
					Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( it.first );
					writer->writeFile( _path, molecule );
				}
			}
			// else export all the structures imported in the scene
			else
			{
				for ( const auto it : VTXApp::get().getScene().getMolecules() )
				{
					Model::Molecule * molecule = it.first;
					writer->writeFile( _path, *molecule );
				}
			}
		}
		catch ( const std::exception & p_e )
		{
			emit logError( "Error saving file" );
			emit logError( p_e.what() );
			result = 0;
		}

		delete writer;

		return result;
	}

	bool Saver::_saveSession()
	{
		bool result = 1;

		Util::Filesystem::checkSaveDirectoryHierarchy( _path );

		const IO::FilePath itemDirectory = Util::Filesystem::getSceneObjectsSaveDirectory( _path );

		std::set<IO::FilePath> filesToRemove = Util::Filesystem::getFilesInDirectory( itemDirectory );

		IO::Writer::SerializedObject<VTXApp> * const writer = new IO::Writer::SerializedObject<VTXApp>( this );

		// Write.
		try
		{
			// for each molecule check if :
			// If not saved on computer => Create file in scene molecule directory
			// Else if has any modifications => Create / Save file in scene molecule directory.
			for ( const std::pair<Model::Molecule *, float> & molecule : VTXApp::get().getScene().getMolecules() )
			{
				const IO::ScenePathData::Data & moleculePathData
					= VTXApp::get().getScenePathData().getData( molecule.first );

				IO::FilePath filePath = moleculePathData.getFilepath();

				bool needToSaveMolecule = moleculePathData.needToSaveMolecule();

				if ( VTX_SETTING().isPortableSaveActivated() )
				{
					const bool pathIsInItemDirectory = filePath.path().rfind( itemDirectory.path(), 0 ) == 0;
					needToSaveMolecule				 = needToSaveMolecule || !pathIsInItemDirectory;
				}

				if ( needToSaveMolecule )
				{
					IO::Writer::ChemfilesWriter * const moleculeWriter = new IO::Writer::ChemfilesWriter( this );

					if ( Util::Filesystem::getParentDir( filePath ) != itemDirectory )
					{
						filePath = itemDirectory + "/molecule.mmcif";
						Util::Filesystem::generateUniqueFileName( filePath );
						VTXApp::get().getScenePathData().getData( molecule.first ).registerPath( filePath );
					}

					moleculeWriter->writeFile( filePath, *molecule.first );
					VTXApp::get().getScenePathData().getData( molecule.first ).registerWriter( moleculeWriter );
				}

				filesToRemove.erase( filePath );
			}

			writer->writeFile( _path, VTXApp::get() );

			for ( const std::pair<Model::Molecule *, float> & molecule : VTXApp::get().getScene().getMolecules() )
			{
				VTXApp::get().getScenePathData().getData( molecule.first ).deleteWriter();
				VTXApp::get().getScenePathData().getData( molecule.first ).setHasChanged( false );
			}

			// Clean files
			while ( filesToRemove.size() > 0 )
			{
				const IO::FilePath fileToRemove = *filesToRemove.begin();
				filesToRemove.erase( filesToRemove.begin() );
				Util::Filesystem::remove( fileToRemove );
			}
		}
		catch ( const std::exception & p_e )
		{
			emit logError( "Error saving file" );
			emit logError( p_e.what() );
			result = 0;
		}

		delete writer;

		return result;
	}

	Saver::MODE Saver::_getMode( const IO::FilePath & p_path ) const
	{
		std::string extension = p_path.extension();

		if ( extension == "nc" || extension == "cif" || extension == "cml" || extension == "cssr" || extension == "dcd"
			 || extension == "gro" || extension == "lammpstrj" || extension == "mmcif" || extension == "mmtf"
			 || extension == "mol2" || extension == "molden" || extension == "pdb" || extension == "sdf"
			 || extension == "smi" || extension == "arc" || extension == "trr" || extension == "mmtf"
			 || extension == "xtc" || extension == "tng" || extension == "trj" || extension == "xyz" )
		{
			return MODE::MOLECULE;
		}
		else if ( extension == "vtx" )
		{
			return MODE::VTX;
		}
		else
		{
			return MODE::UNKNOWN;
		}
	}
} // namespace VTX::Worker
