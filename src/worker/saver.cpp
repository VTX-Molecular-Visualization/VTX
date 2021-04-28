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
			emit logInfo( "Saving " + _path->filename().string() );

			const MODE mode = _getMode( *_path );

			switch ( _getMode( *_path ) )
			{
			case MODE::UNKNOWN: emit logError( "Format not supported" ); break;
			case MODE::MOLECULE: result = _saveMolecule(); break;
			case MODE::VTX: result = _saveSession(); break;

			default: emit logError( "Mode " + std::to_string( int( mode ) ) + "  not supported" ); break;
			}

			delete _path;

			chrono.stop();
			emit logInfo( "File treated in " + std::to_string( chrono.elapsedTime() ) + "s" );

			return result;
		}

		bool Saver::_saveMolecule()
		{
			bool result = 1;

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

			IO::Writer::VTX * writer = new IO::Writer::VTX();

			// Write.
			try
			{
				writer->writeFile( _path->string(), VTXApp::get() );
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

		Saver::MODE Saver::_getMode( const FilePath & p_path ) const
		{
			FilePath extension = p_path.extension();

			if ( extension == ".sdf" || extension == ".pdb" || extension == ".mmtf" || extension == ".cif"
				 || extension == ".arc" || extension == ".xyz" || extension == ".dcd" || extension == ".mol2" )
			{
				return MODE::MOLECULE;
			}
			else if ( extension == ".vtx" )
			{
				return MODE::VTX;
			}
			else
			{
				return MODE::UNKNOWN;
			}
		}
	} // namespace Worker
} // namespace VTX
