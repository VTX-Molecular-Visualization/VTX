#include "ui/old_ui/controller/base_camera_controller.hpp"

namespace VTX::Controller
{
	void BaseCameraController::update( const float & p_deltaTime )
	{
		BaseMouseController::update( p_deltaTime );

		if ( _isOrienting )
		{
			_orientTime += p_deltaTime;

			if ( _orientTime >= ORIENT_DURATION )
			{
				_updateOrient( 1.f );
				_isOrienting = false;

				_deltaMousePosition = VEC2I_ZERO;
			}
			else
			{
				_updateOrient( _orientTime / ORIENT_DURATION );
			}
		}
		else
		{
			_updateInputs( p_deltaTime );
		}
	}

	void BaseCameraController::orient( const App::Component::Object3D::Helper::AABB & p_aabb )
	{
		_orientTime = 0.f;
		_computeOrientPositions( p_aabb );
	}

	void BaseCameraController::orient( const Vec3f & p_position, const Quatf & p_orientation )
	{
		_orientTime = 0.f;
		_computeOrientPositions( p_position, p_orientation );
	}
} // namespace VTX::Controller
