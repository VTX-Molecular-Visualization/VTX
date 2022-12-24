#include "base_camera_controller.hpp"

namespace VTX::UI::QT::Controller
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
} // namespace VTX::UI::QT::Controller
