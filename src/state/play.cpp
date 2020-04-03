#include "play.hpp"
#include "object3d/scene.hpp"
#include "tool/chrono.hpp"
#include "util/math.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace State
	{
		void Play::enter( void * const p_arg )
		{
			_path = (Model::Path *)p_arg;

			if ( _path->getDuration() == 0.f || _path->getViewpoints().size() < 2 )
			{
				VTXApp::get().goToState( ID::State::VISUALIZATION );
				return;
			}

			VTX_INFO( "Playing... total: " + std::to_string( _path->getDuration() ) + " seconds" );

			_setCamera();
		}

		void Play::exit()
		{
			VTX_INFO( "Stop" );
			_path	 = nullptr;
			_actions = nullptr;
			_time	 = 0.f;
		}

		void Play::update( const double p_deltaTime )
		{
			BaseState::update( p_deltaTime );

			_time += (float)p_deltaTime;

			// Loop.
			if ( _time >= _path->getDuration() )
			{
				if ( _path->isLooping() )
				{
					_time	 = 0.f;
					_actions = nullptr;
				}
				else
				{
					VTXApp::get().goToState( ID::State::VISUALIZATION );
					return;
				}
			}

			_setCamera();
			_executeActions();
			VTXApp::get().getScene().update( p_deltaTime );
			VTXApp::get().renderScene();

		} // namespace State

		void Play::_setCamera() const
		{
			Model::Viewpoint viewpoint = _path->getInterpolatedViewpoint( _time );
			VTXApp::get().getScene().getCamera().set( viewpoint.getPosition(), viewpoint.getRotation() );
		}

		void Play::_executeActions()
		{
			if ( _actions != _path->getCurrentActions( _time ) )
			{
				_actions = _path->getCurrentActions( _time );
				for ( const std::string & action : *_actions )
				{
					VTX_ACTION( action, true );
				}
			}
		}

	} // namespace State
} // namespace VTX
