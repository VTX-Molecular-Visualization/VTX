#include "export.hpp"
#include "action/snapshot.hpp"
#include "vtx_app.hpp"
namespace VTX
{
	namespace State
	{
		// Action loop
		void Export::enter( void * const p_arg )
		{
			_path = (Model::Path *)p_arg;

			float duration = _path->getDuration();
			_frameCount	   = uint( VIDEO_FPS * duration );

			if ( _frameCount == 0u || _path->getViewpoints().size() < 2 )
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
			_path		= nullptr;
			_frame		= 0u;
			_frameCount = 0u;
		}

		void Export::update( const double p_deltaTime )
		{
			BaseState::update( p_deltaTime );

			float			 time	   = (float)_frame / VIDEO_FPS;
			Model::Viewpoint viewpoint = _path->getInterpolatedViewpoint( time );

			// Update renderer.
			VTXApp::get().getScene().getCamera().set( viewpoint.getPosition(), viewpoint.getRotation() );
			VTXApp::get().getScene().update( 1.f / VIDEO_FPS );
			VTXApp::get().renderScene();

			std::string counterStr = std::to_string( _frame );
			VTX_ACTION( new Action::Snapshot( VIDEO_DIR + "snapshot" + std::string( 4 - counterStr.length(), '0' )
											  + counterStr ) );

			VTX_INFO( std::to_string( ( uint )( (float)_frame * 100 / _frameCount ) ) + "%" );

			if ( _frame == _frameCount ) { VTXApp::get().goToState( ID::State::VISUALIZATION ); }
			else
			{
				_frame++;
			}
		}

	} // namespace State
} // namespace VTX
