#include "ui/qt/state/play.hpp"
#include "ui/id.hpp"
#include <app/old/application/scene.hpp>
#include <app/old/component/object3d/viewpoint.hpp>
#include <app/old/component/render/camera.hpp>
#include <app/old/component/video/path.hpp>
#include <app/old/vtx_app.hpp>
#include <util/math.hpp>

namespace VTX::UI::QT::State
{
	void Play::enter( void * const p_arg )
	{
		_path = (App::Old::Component::Video::Path *)p_arg;
		// App::Old::VTXApp::get().getSetting().backup();

		if ( _path->getDuration() == 0.f || _path->getViewpoints().size() < 2 )
		{
			App::Old::VTXApp::get().goToState( ID::State::VISUALIZATION );
			return;
		}

		VTX_INFO( "Playing... total: {} seconds.", _path->getDuration() );

		_setCamera();
	}

	void Play::exit()
	{
		VTX_INFO( "Stop" );
		_path	 = nullptr;
		_actions = nullptr;
		_time	 = 0.f;
		// App::Old::VTXApp::get().getSetting().recover();
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
				// App::Old::VTXApp::get().getSetting().recover();
			}
			else
			{
				App::Old::VTXApp::get().goToState( ID::State::VISUALIZATION );
				return;
			}
		}

		_setCamera();
		_executeActions( _time );
		App::Old::VTXApp::get().getScene().update( p_deltaTime );
		App::Old::VTXApp::get().renderScene();

	} // namespace State

	void Play::_setCamera() const
	{
		App::Old::Component::Object3D::Viewpoint viewpoint = _path->getInterpolatedViewpoint( _time );
		if ( viewpoint.getController() == ID::Controller::TRACKBALL )
		{
			App::Old::VTXApp::get().getScene().getCamera().setRotationAround(
				Quatf( viewpoint.getRotation() ), viewpoint.getTarget(), viewpoint.getDistance() );
		}
		else
		{
			App::Old::VTXApp::get().getScene().getCamera().set( viewpoint.getPosition(), viewpoint.getRotation() );
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

} // namespace VTX::UI::QT::State
