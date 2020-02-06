#include "state_playing.hpp"
#include "../model/model_checkpoint.hpp"
#include "model/model_path.hpp"
#include "object3d/scene.hpp"
#include "tool/chrono.hpp"
#include "util/math.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace State
	{
		void StatePlaying::enter( void * const p_arg )
		{
			_path = (Model::ModelPath *)p_arg;

			if ( _path->getCheckpoints().size() < 2 )
			{
				VTXApp::get().goToState( ID::State::VISUALIZATION );
				return;
			}

			_totalTime = _path->computeTotalTime();

			if ( _totalTime == 0.f )
			{
				VTXApp::get().goToState( ID::State::VISUALIZATION );
				return;
			}

			VTX_INFO( "Playing... total: " + std::to_string( _totalTime ) + " seconds" );

			_setCamera();
		}

		void StatePlaying::exit()
		{
			VTX_INFO( "Stop" );
			_path	   = nullptr;
			_time	   = 0.f;
			_totalTime = 0.f;
		}

		void StatePlaying::update( const double p_deltaTime )
		{
			BaseState::update( p_deltaTime );

			_time += (float)p_deltaTime;

			// Loop.
			if ( _time >= _totalTime )
			{
				VTXApp::get().goToState( ID::State::VISUALIZATION );
				return;
			}

			_setCamera();
			VTXApp::get().getScene().update( p_deltaTime );
			VTXApp::get().renderScene();

		} // namespace State

		void StatePlaying::_setCamera() const
		{
			Model::ModelCheckpoint::CheckpointInterpolationData data
				= _path->getCurrentCheckpointInterpolationData( _time );
			VTXApp::get().getScene().getCamera().set(
				Util::Math::lerp( data.positionLhs, data.positionRhs, data.value ),
				Util::Math::lerp( data.rotationLhs, data.rotationRhs, data.value ) );
		}

	} // namespace State
} // namespace VTX
