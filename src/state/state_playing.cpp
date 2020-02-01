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

			VTX_DEBUG( "Playing... total: " + std::to_string( _totalTime ) );

			_setCamera();
		}

		void StatePlaying::exit()
		{
			_path	   = nullptr;
			_time	   = 0.f;
			_totalTime = 0.f;
		}

		void StatePlaying::update( const double p_deltaTime )
		{
			_time += (float)p_deltaTime;

			// Loop.
			if ( _time >= _totalTime ) { _time = 0.f; }

			_setCamera();
		}

		void StatePlaying::_setCamera() const
		{
			float total = 0.f;

			uint offset = 0;
			for ( ; offset < _path->getCheckpoints().size(); ++offset )
			{
				Model::ModelCheckpoint * checkpoint = _path->getCheckpoints()[ offset ];
				if ( offset >= 1 ) { total += checkpoint->getDuration(); }
				if ( total >= _time ) { break; }
			}

			float value = 1.f - ( ( -( _time - total ) ) / _path->getCheckpoints()[ offset ]->getDuration() );

			float theta	   = glm::lerp( _path->getCheckpoints()[ offset > 0 ? offset - 1 : 0 ]->getTheta(),
										_path->getCheckpoints()[ offset ]->getTheta(),
										value );
			float phi	   = glm::lerp( _path->getCheckpoints()[ offset > 0 ? offset - 1 : 0 ]->getPhi(),
									_path->getCheckpoints()[ offset ]->getPhi(),
									value );
			Vec3f position = glm::lerp( _path->getCheckpoints()[ offset > 0 ? offset - 1 : 0 ]->getPosition(),
										_path->getCheckpoints()[ offset ]->getPosition(),
										value );

			VTXApp::get().getScene().getCamera().set( position, theta, phi );
		}

	} // namespace State
} // namespace VTX
