#include "app/old/internal/worker/saver.hpp"
#include "app/old/application/scene.hpp"
#include "app/old/application/selection/selection_manager.hpp"
#include "app/old/component/chemistry/molecule.hpp"
#include "app/old/core/io/writer/serialized_object.hpp"
#include "app/old/internal/io/filesystem.hpp"
#include "app/old/internal/io/serialization/scene_path_data.hpp"
#include "app/old/internal/io/writer/writer_chemfiles.hpp"
#include "app/old/mvc.hpp"
#include <set>
#include <util/logger.hpp>

namespace VTX::App::Old::Internal::Worker
{
	uint Saver::_run()
	{
		bool		 result = 1;
		Util::Chrono chrono;

		chrono.start();
		emitLogInfo( "Saving " + _path.filename().string() );

		const MODE mode = _getMode( _path );

		switch ( mode )
		{
		case MODE::UNKNOWN: emitLogError( "Format not supported" ); break;
		case MODE::MOLECULE: result = _saveMolecule(); break;
		case MODE::VTX: result = _saveSession(); break;

		default: emitLogError( "Mode " + std::to_string( int( mode ) ) + "  not supported" ); break;
		}

		chrono.stop();
		emitLogInfo( "File treated in " + std::to_string( chrono.elapsedTime() ) + "s" );

		return result;
	}

	bool Saver::_saveMolecule()
	{
		bool										 result = 1;
		App::Old::Internal::IO::Writer::ChemfilesWriter * writer = new App::Old::Internal::IO::Writer::ChemfilesWriter( this );

		// Write.
		try
		{
			// if selection is not empty -> export selected structures
			if ( !VTX::App::Old::Application::Selection::SelectionManager::get()
					  .getSelectionModel()
					  .getMoleculesMap()
					  .empty() )
			{
				for ( const auto it :
					  VTX::App::Old::Application::Selection::SelectionManager::get().getSelectionModel().getMoleculesMap() )
				{
					App::Old::Component::Chemistry::Molecule & molecule
						= VTX::MVC_MANAGER().getModel<App::Old::Component::Chemistry::Molecule>( it.first );
					writer->writeFile( _path, molecule );
				}
			}
			// else export all the structures imported in the scene
			else
			{
				for ( const auto it : App::Old::VTXApp::get().getScene().getMolecules() )
				{
					App::Old::Component::Chemistry::Molecule * molecule = it.first;
					writer->writeFile( _path, *molecule );
				}
			}
		}
		catch ( const std::exception & p_e )
		{
			emitLogError( "Error saving file" );
			emitLogError( p_e.what() );
			result = 0;
		}

		delete writer;

		return result;
	}

	bool Saver::_saveSession()
	{
		bool result = 1;

		App::Old::Internal::IO::Filesystem::checkSaveDirectoryHierarchy( _path );

		const FilePath itemDirectory = App::Old::Internal::IO::Filesystem::getSceneObjectsSaveDirectory( _path );

		Util::Filesystem::removeAll( itemDirectory );

		App::Old::Core::IO::Writer::SerializedObject<VTXApp> * const writer
			= new App::Old::Core::IO::Writer::SerializedObject<VTXApp>( this );

		// Write.
		try
		{
			// for each molecule check if :
			// If not saved on computer => Create file in scene molecule directory
			// Else if has any modifications => Create / Save file in scene molecule directory.
			for ( const App::Old::Application::Scene::PairMoleculePtrFloat & molecule :
				  App::Old::VTXApp::get().getScene().getMolecules() )
			{
				const App::Old::Internal::IO::Serialization::ScenePathData::Data & moleculePathData
					= App::Old::VTXApp::get().getScenePathData().getData( molecule.first );

				FilePath filePath = moleculePathData.getFilepath();

				bool needToSaveMolecule = moleculePathData.needToSaveMolecule();

				if ( VTX_SETTING().isPortableSaveActivated() )
				{
					const bool pathIsInItemDirectory = filePath.string().rfind( itemDirectory.string(), 0 ) == 0;
					needToSaveMolecule				 = needToSaveMolecule || !pathIsInItemDirectory;
				}

				if ( needToSaveMolecule )
				{
					App::Old::Internal::IO::Writer::ChemfilesWriter * const moleculeWriter
						= new App::Old::Internal::IO::Writer::ChemfilesWriter( this );

					if ( filePath.parent_path() != itemDirectory )
					{
						filePath = itemDirectory / "molecule.mmcif";
						Util::Filesystem::generateUniqueFileName( filePath );
						App::Old::VTXApp::get().getScenePathData().getData( molecule.first ).registerPath( filePath );
					}

					moleculeWriter->writeFile( filePath, *molecule.first );
					App::Old::VTXApp::get().getScenePathData().getData( molecule.first ).registerWriter( moleculeWriter );
				}
			}

			writer->writeFile( _path, App::Old::VTXApp::get() );

			for ( const App::Old::Application::Scene::PairMoleculePtrFloat & molecule :
				  App::Old::VTXApp::get().getScene().getMolecules() )
			{
				App::Old::VTXApp::get().getScenePathData().getData( molecule.first ).deleteWriter();
				App::Old::VTXApp::get().getScenePathData().getData( molecule.first ).setHasChanged( false );
			}
		}
		catch ( const std::exception & p_e )
		{
			emitLogError( "Error saving file" );
			emitLogError( p_e.what() );
			result = 0;
		}

		delete writer;

		return result;
	}

	Saver::MODE Saver::_getMode( const FilePath & p_path ) const
	{
		std::string extension = p_path.extension().string();

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
} // namespace VTX::App::Old::Internal::Worker