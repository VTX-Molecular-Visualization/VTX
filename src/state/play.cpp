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
			_path = nullptr;
			_time = 0.f;
		}

		void Play::update( const double p_deltaTime )
		{
			BaseState::update( p_deltaTime );

			_time += (float)p_deltaTime;

			// Loop.
			if ( _time >= _path->getDuration() )
			{
				VTXApp::get().goToState( ID::State::VISUALIZATION );
				return;
			}

			_setCamera();
			VTXApp::get().getScene().update( p_deltaTime );
			VTXApp::get().renderScene();

		} // namespace State

		void Play::_setCamera() const
		{
			Model::Viewpoint::ViewpointInterpolationData data = _path->getCurrentViewpointInterpolationData( _time );
			VTXApp::get().getScene().getCamera().set(
				Util::Math::lerp( data.positionLhs, data.positionRhs, data.value ),
				Util::Math::lerp( data.rotationLhs, data.rotationRhs, data.value ) );
		}

	} // namespace State
} // namespace VTX
