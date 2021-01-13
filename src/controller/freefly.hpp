#ifndef __VTX_CONTROLLER_FREEFLY__
#define __VTX_CONTROLLER_FREEFLY__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_camera_controller.hpp"

namespace VTX
{
	namespace Controller
	{
		class Freefly : public BaseCameraController
		{
		  public:
			explicit Freefly( Object3D::Camera & p_camera ) : BaseCameraController( p_camera ) {}

			void reset() override;

		  protected:
			void _updateInputs( const float & ) override;
			void _updateOrient( const float & ) override;
			void _computeOrientPositions( const Math::AABB & ) override;
		};
	} // namespace Controller
} // namespace VTX
#endif
