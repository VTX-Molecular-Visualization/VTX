#ifndef __VTX_ACTION_OPEN_API__
#define __VTX_ACTION_OPEN_API__

#ifdef _MSC_VER
#pragma once
#endif

#pragma warning( push, 0 )
#include <httplib.h>
#pragma warning( pop )
#include "base_action.hpp"
#include "model/molecule.hpp"
#include "open.hpp"
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
				try
				{
					std::string url = "https://mmtf.rcsb.org";
					VTX_INFO( url );
					httplib::Client					   cli( url );
					std::shared_ptr<httplib::Response> response = cli.Get( ( "/v1.0/full/" + _id ).c_str() );
					if ( response )
					{
						VTX_DEBUG( std::to_string( response->status ) );
						std::cout << response->body << std::endl;

						if ( response->status == 200 ) {}
					}
					else
					{
						VTX_DEBUG( "Response null" );
					}

					// VTXApp::get().goToState( ID::State::LOAD, (void *)&_id );
				}
				catch ( const std::exception p_e )
				{
					VTX_ERROR( p_e.what() );
				}
			};

		  private:
			const std::string & _id;
		};
	} // namespace Action
} // namespace VTX
#endif
