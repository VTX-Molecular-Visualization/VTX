#ifndef __VTX_APP_FIXTURE__
#define __VTX_APP_FIXTURE__

#include "action/io.hpp"
#include "pass/pass_manager.hpp"
#include "services.hpp"
#include "vtx_app.hpp"
#include <util/filesystem.hpp>

namespace VTX::App
{
	class Fixture
	{
	  public:
		inline static Arguments getDefaultArguments()
		{
			static const char * argv[] = { "" };

			Arguments args;
			args.argc		= 1;
			args.argv		= argv;
			args.noGraphics = true;
			args.noUpdates	= true;
			args.noPython	= true;
			args.noGui		= true;
			args.debug		= true;
			return args;
		}

		inline Fixture() : Fixture( getDefaultArguments() ) {}

		inline explicit Fixture( Arguments p_args )
		{
			if ( p_args.argv == nullptr )
			{
				static const char * argv[] = { "" };
				p_args.argc				   = 1;
				p_args.argv				   = argv;
			}

			_app = std::make_unique<VTXApp>( std::move( p_args ) );
			_app->start();
		}

		inline void loadSystem( const FilePath & p_path )
		{
			ACTION().execute<Action::IO::LoadSystem>( p_path );
			PASS().update( 0.f, 0.f );
		}

		inline void loadSystem( const std::string_view p_filename = "1AGA.mmtf" )
		{ loadSystem( Util::Filesystem::getExecutableDir() / "data" / p_filename ); }

		inline VTXApp * const get() { return _app.get(); }

		inline ~Fixture() { _app.reset(); }

	  private:
		std::unique_ptr<VTXApp> _app;
	};
} // namespace VTX::App

#endif
