#include "export.hpp"
#include "action/snapshot.hpp"
#include "util/time.hpp"
#include "vtx_app.hpp"
#include <filesystem>

namespace VTX
{
	namespace State
	{
		// Action loop
		void Export::enter( void * const p_arg )
		{
			_arg = *(Arg *)p_arg;

			_directoryName = Util::Time::getTimestamp();
			std::filesystem::create_directories( VIDEO_DIR + _directoryName );

			float duration = _arg.path->getDuration();
			_frameCount	   = uint( VIDEO_FPS * duration );

			if ( _frameCount == 0u || _arg.path->getViewpoints().size() < 2 )
			{
				VTX_WARNING( "Total time must be > 0" );
				VTXApp::get().goToState( ID::State::VISUALIZATION );
				return;
			}

			VTX_INFO( "Exporting... total: " + std::to_string( _frameCount ) + " frames / " + std::to_string( duration )
					  + " seconds" );
		}

		void Export::exit()
		{
			_actions	= nullptr;
			_frame		= 0u;
			_frameCount = 0u;
		}

		void Export::update( const double p_deltaTime )
		{
			BaseState::update( p_deltaTime );

			float			 time	   = (float)_frame / VIDEO_FPS;
			Model::Viewpoint viewpoint = _arg.path->getInterpolatedViewpoint( time );

			// Action.
			if ( _actions != _arg.path->getCurrentActions( time ) )
			{
				_actions = _arg.path->getCurrentActions( time );
				for ( const std::string & action : *_actions )
				{
					VTX_ACTION( action, true );
				}
			}

			// Update renderer.
			VTXApp::get().getScene().getCamera().set( viewpoint.getPosition(), viewpoint.getRotation() );
			VTXApp::get().getScene().update( 1.f / VIDEO_FPS );
			VTXApp::get().renderScene();

			std::string counterStr = std::to_string( _frame );
			VTX_ACTION( new Action::Snapshot( _arg.mode,
											  VIDEO_DIR + _directoryName + "/" + "video"
												  + std::string( 6 - counterStr.length(), '0' ) + counterStr ) );

			VTX_INFO( std::to_string( ( uint )( (float)_frame * 100 / _frameCount ) ) + "%" );

			if ( _frame == _frameCount ) { VTXApp::get().goToState( ID::State::VISUALIZATION ); }
			else
			{
				_frame++;
			}
		}

	} // namespace State
} // namespace VTX
