#include "ui/old_ui/state/play.hpp"
#include "ui/id.hpp"
#include <app/application/scene.hpp>
#include <app/component/object3d/viewpoint.hpp>
#include <app/component/render/camera.hpp>
#include <app/component/video/path.hpp>
#include <app/old_app/vtx_app.hpp>
#include <util/math.hpp>

namespace VTX
{
	namespace State
	{
		void Play::enter( void * const p_arg )
		{
			_path = (App::Component::Video::Path *)p_arg;
			// VTXApp::get().getSetting().backup();

			if ( _path->getDuration() == 0.f || _path->getViewpoints().size() < 2 )
			{
				VTXApp::get().goToState( UI::ID::State::VISUALIZATION );
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
			// VTXApp::get().getSetting().recover();
		}

		void Play::update( const float & p_deltaTime )
		{
			BaseState::update( p_deltaTime );

			_time += p_deltaTime;

			// Loop.
			if ( _time >= _path->getDuration() )
			{
				_executeActions( _path->getDuration() );

				if ( _path->isLooping() )
				{
					_time	 = 0.f;
					_actions = nullptr;
					// VTXApp::get().getSetting().recover();
				}
				else
				{
					VTXApp::get().goToState( UI::ID::State::VISUALIZATION );
					return;
				}
			}

			_setCamera();
			_executeActions( _time );
			VTXApp::get().getScene().update( p_deltaTime );
			VTXApp::get().renderScene();

		} // namespace State

		void Play::_setCamera() const
		{
			App::Component::Object3D::Viewpoint viewpoint = _path->getInterpolatedViewpoint( _time );
			if ( viewpoint.getController() == UI::ID::Controller::TRACKBALL )
			{
				VTXApp::get().getScene().getCamera().setRotationAround(
					Quatf( viewpoint.getRotation() ), viewpoint.getTarget(), viewpoint.getDistance() );
			}
			else
			{
				VTXApp::get().getScene().getCamera().set( viewpoint.getPosition(), viewpoint.getRotation() );
			}
		}

		void Play::_executeActions( const float p_time )
		{
			/*
			if ( _actions != _path->getCurrentActions( p_time ) )
			{
				_actions = _path->getCurrentActions( p_time );
				for ( const std::string & action : *_actions )
				{
					VTX_ACTION( action );
				}
			}
			*/
		}

	} // namespace State
} // namespace VTX
