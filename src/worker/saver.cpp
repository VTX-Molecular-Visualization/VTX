#include "saver.hpp"
#include "io/writer/vtx.hpp"
#include "io/writer/writer_chemfiles.hpp"
#include "model/molecule.hpp"
#include "mvc/mvc_manager.hpp"
#include "object3d/scene.hpp"
#include "selection/selection_manager.hpp"
#include "tool/chrono.hpp"
#include "tool/logger.hpp"

namespace VTX
{
	namespace Worker
	{
		uint Saver::_run()
		{
			bool		 result = 1;
			Tool::Chrono chrono;

			chrono.start();
			_logInfo( "Saving " + _path->filename().string() );

			// Create Writer.
			IO::Writer::ChemfilesWriter * writer = new IO::Writer::ChemfilesWriter();

			// Write.
			try
			{
				// if selection is not empty -> export selected structures
				if ( !VTX::Selection::SelectionManager::get().getSelectionModel().getItems().empty() )
				{
					for ( const auto it : VTX::Selection::SelectionManager::get().getSelectionModel().getItems() )
					{
						Model::Molecule & molecule = MVC::MvcManager::get().getModel<Model::Molecule>( it.first );
						writer->writeFile( _path->filename().string(), molecule );
					}
				}
				// else export all the structures imported in the scene
				else
				{
					for ( const auto it : VTXApp::get().getScene().getMolecules() )
					{
						Model::Molecule * molecule = it.first;
						writer->writeFile( _path->string(), *molecule );
					}
				}
			}
			catch ( const std::exception & p_e )
			{
				_logError( "Error saving file" );
				_logError( p_e.what() );
				result = 0;
			}

			delete writer;
			delete _path;

			chrono.stop();
			_logInfo( "File treated in " + std::to_string( chrono.elapsedTime() ) + "s" );

			return result;
		} // namespace Worker
	}	  // namespace Worker
} // namespace VTX
