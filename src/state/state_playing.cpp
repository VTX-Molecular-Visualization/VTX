#include "state_playing.hpp"
#include "../model/model_path.hpp"
#include "../object3d/scene.hpp"
#include "../tool/chrono.hpp"
#include "../vtx_app.hpp"
#include <glm/gtx/compatibility.hpp>

namespace VTX
{
	namespace State
	{
		void StatePlaying::enter( void * const p_arg )
		{
			Model::ModelPath * const path = (Model::ModelPath * const)p_arg;
			_totalTime					  = path->computeTotalTime();

			if ( _totalTime == 0.f )
			{
				VTXApp::get().goToState( ID::State::VISUALIZATION );
				return;
			}

			VTX_DEBUG( "Playing... total: " + std::to_string( _totalTime ) );

			_setCamera();
		}

		void StatePlaying::exit()
		{
			_time	   = 0.f;
			_totalTime = 0.f;
		}

		void StatePlaying::update( const double p_deltaTime )
		{
			_time += (float)p_deltaTime;

			// Loop.
			if ( _time > _totalTime ) { _time = 0.f; }

			_setCamera();
		}

		void StatePlaying::_setCamera() const {}

	} // namespace State
} // namespace VTX
