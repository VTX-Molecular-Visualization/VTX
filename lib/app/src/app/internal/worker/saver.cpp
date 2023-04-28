#include "app/internal/worker/saver.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/mvc.hpp"
#include "app/old_app/io/filesystem.hpp"
#include "app/old_app/io/struct/scene_path_data.hpp"
#include "app/old_app/io/writer/serialized_object.hpp"
#include "app/old_app/io/writer/writer_chemfiles.hpp"
#include "app/application/scene.hpp"
#include "app/application/selection/selection_manager.hpp"
#include <set>
#include <util/logger.hpp>

namespace VTX::Worker
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
		bool						  result = 1;
		IO::Writer::ChemfilesWriter * writer = new IO::Writer::ChemfilesWriter( this );

		// Write.
		try
		{
			// if selection is not empty -> export selected structures
			if ( !VTX::App::Application::Selection::SelectionManager::get().getSelectionModel().getMoleculesMap().empty() )
			{
				for ( const auto it : VTX::App::Application::Selection::SelectionManager::get().getSelectionModel().getMoleculesMap() )
				{
					App::Component::Chemistry::Molecule & molecule = VTX::MVC_MANAGER().getModel<App::Component::Chemistry::Molecule>( it.first );
					writer->writeFile( _path, molecule );
				}
			}
			// else export all the structures imported in the scene
			else
			{
				for ( const auto it : VTXApp::get().getScene().getMolecules() )
				{
					App::Component::Chemistry::Molecule * molecule = it.first;
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

		IO::Filesystem::checkSaveDirectoryHierarchy( _path );

		const FilePath itemDirectory = IO::Filesystem::getSceneObjectsSaveDirectory( _path );

		Util::Filesystem::removeAll( itemDirectory );

		IO::Writer::SerializedObject<VTXApp> * const writer = new IO::Writer::SerializedObject<VTXApp>( this );

		// Write.
		try
		{
			// for each molecule check if :
			// If not saved on computer => Create file in scene molecule directory
			// Else if has any modifications => Create / Save file in scene molecule directory.
			for ( const App::Application::Scene::PairMoleculePtrFloat & molecule : VTXApp::get().getScene().getMolecules() )
			{
				const IO::Struct::ScenePathData::Data & moleculePathData
					= VTXApp::get().getScenePathData().getData( molecule.first );

				FilePath filePath = moleculePathData.getFilepath();

				bool needToSaveMolecule = moleculePathData.needToSaveMolecule();

				if ( VTX_SETTING().isPortableSaveActivated() )
				{
					const bool pathIsInItemDirectory = filePath.string().rfind( itemDirectory.string(), 0 ) == 0;
					needToSaveMolecule				 = needToSaveMolecule || !pathIsInItemDirectory;
				}

				if ( needToSaveMolecule )
				{
					IO::Writer::ChemfilesWriter * const moleculeWriter = new IO::Writer::ChemfilesWriter( this );

					if ( filePath.parent_path() != itemDirectory )
					{
						filePath = itemDirectory / "molecule.mmcif";
						Util::Filesystem::generateUniqueFileName( filePath );
						VTXApp::get().getScenePathData().getData( molecule.first ).registerPath( filePath );
					}

					moleculeWriter->writeFile( filePath, *molecule.first );
					VTXApp::get().getScenePathData().getData( molecule.first ).registerWriter( moleculeWriter );
				}
			}

			writer->writeFile( _path, VTXApp::get() );

			for ( const App::Application::Scene::PairMoleculePtrFloat & molecule : VTXApp::get().getScene().getMolecules() )
			{
				VTXApp::get().getScenePathData().getData( molecule.first ).deleteWriter();
				VTXApp::get().getScenePathData().getData( molecule.first ).setHasChanged( false );
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
} // namespace VTX::Worker
