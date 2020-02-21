#ifndef __VTX_ACTION_OPEN_API__
#define __VTX_ACTION_OPEN_API__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "httplib.h"
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

			virtual void execute() override {
				/*
				std::string url = "https://mmtf.rcsb.org/v1.0/full/";
				VTX_DEBUG( url );
				httplib::Client					   cli( url, 80 );
				std::shared_ptr<httplib::Response> response = cli.Get( _id.c_str() );
				if ( response && response->status == 200 ) { std::cout << response->body << std::endl; }
				*/
				// VTXApp::get().goToState( ID::State::LOAD, (void *)&_id );
			};

		  private:
			const std::string & _id;
		};
	} // namespace Action
} // namespace VTX
#endif
