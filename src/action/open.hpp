#ifndef __VTX_ACTION_OPEN__
#define __VTX_ACTION_OPEN__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "model/molecule.hpp"
#include "vtx_app.hpp"
#include "worker/loader.hpp"
#include <vector>

namespace VTX
{
	namespace Action
	{
		class Open : public BaseAction
		{
		  public:
			explicit Open( Path * p_path ) { _paths.emplace_back( p_path ); }
			explicit Open( const std::vector<Path *> & p_paths ) : _paths( p_paths ) {}
			explicit Open( const std::map<Path *, std::string *> & p_buffers ) : _buffers( p_buffers ) {}

			virtual void execute() override
			{
				Worker::Loader * loader = nullptr;
				if ( _paths.empty() == false )
				{
					loader = new Worker::Loader( _paths );
				}
				else if ( _buffers.empty() == false )
				{
					loader = new Worker::Loader( _buffers );
				}
				if ( loader == nullptr )
				{
					return;
				}

				/*
				const Worker::CallbackSuccess * success
					= new Worker::CallbackSuccess( [ loader ]( void ) { delete loader; } );
				const Worker::CallbackError * error
					= new Worker::CallbackError( [ loader ]( const std::exception & p_e ) {
						  VTX_ERROR( p_e.what() );
						  delete loader;
					  } );

				VTX_WORKER( loader, success, error );
				*/

				VTX_WORKER( loader );
				delete loader;
			}

		  private:
			std::vector<Path *>				_paths = std::vector<Path *>();
			std::map<Path *, std::string *> _buffers;
		};
	} // namespace Action
} // namespace VTX
#endif
