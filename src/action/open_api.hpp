#ifndef __VTX_ACTION_OPEN_API__
#define __VTX_ACTION_OPEN_API__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "io/path_fake.hpp"
#include "model/molecule.hpp"
#include "open.hpp"
#include "tool/api_fetcher.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class OpenApi : public BaseAction
		{
		  public:
			explicit OpenApi( const std::string & p_id ) : _id( p_id ) {}

			virtual void execute() override
			{
				Tool::ApiFetcher client = Tool::ApiFetcher( API_URL_MMTF + _id );
				std::string		 id		= _id;

				try
				{
					client.run( [ &client, &id ]( void ) {
						IO::PathFake path = IO::PathFake( id + ".mmtf" );
						path.write( client.getBuffer() );
						VTXApp::get().goToState( ID::State::LOAD, (void *)&path );
					} );
				}
				catch ( const std::exception & p_e )
				{
					VTX_ERROR( "File not found" );
					VTX_DEBUG( p_e.what() );
				}
			};

		  private:
			const std::string & _id;
		}; // namespace Action
	}	   // namespace Action
} // namespace VTX
#endif
