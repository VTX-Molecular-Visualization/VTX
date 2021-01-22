#include "saver.hpp"
#include "io/writer/vtx.hpp"
#include "tool/chrono.hpp"
#include "tool/logger.hpp"

namespace VTX
{
	namespace Worker
	{
		void Saver::work()
		{
			Tool::Chrono chrono;

			chrono.start();
			VTX_INFO( "Saving " + _path->filename().string() );

			IO::Writer::VTX * writer = new IO::Writer::VTX();

			try
			{
				writer->writeFile( *_path, VTXApp::get() );
			}
			catch ( const std::exception & p_e )
			{
				VTX_ERROR( "Error saving file" );
				VTX_ERROR( p_e.what() );
			}

			delete writer;

			// Rename if extension is missing.
			if ( std::filesystem::exists( *_path ) && _path->extension() != ".vtx" )
			{
				std::filesystem::rename( *_path, Path( _path->string() + ".vtx" ) );
			}

			delete _path;

			chrono.stop();
			VTX_INFO( "File treated in " + std::to_string( chrono.elapsedTime() ) + "s" );

		} // namespace Worker
	}	  // namespace Worker
} // namespace VTX
