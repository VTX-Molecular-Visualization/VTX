#ifndef __VTX_ACTION_TRANSFORMABLE_ROTATE__
#define __VTX_ACTION_TRANSFORMABLE_ROTATE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action.hpp"
#include "generic/base_transformable.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class TransformableRotate : public BaseAction
		{
		  public:
			explicit TransformableRotate( Generic::BaseTransformable & p_transformable,
										  const float				   p_angle,
										  const Vec3f &				   p_axis ) :
				_transformable( p_transformable ),
				_angle( p_angle ), _axis( p_axis )
			{
			}

			virtual void execute() override { _transformable.rotate( _angle, _axis ); }

		  private:
			Generic::BaseTransformable & _transformable;
			const float					 _angle;
			const Vec3f					 _axis;
		};
	} // namespace Action
} // namespace VTX
#endif
