#ifndef __VTX_ACTION_SCALE__
#define __VTX_ACTION_SCALE__

#ifdef _MSC_VER
#pragma once
#endif

#include "../generic/base_transformable.hpp"
#include "../vtx_app.hpp"
#include "base_action_undonable.hpp"

namespace VTX
{
	namespace Action
	{
		template<typename T, typename = std::enable_if<std::is_base_of<Generic::BaseTransformable, T>::value>>
		class ActionScale : public BaseActionUndonable
		{
		  public:
			explicit ActionScale( T & p_transformable, const float p_scale ) :
				_transformable( p_transformable ), _scale( p_scale ),
				_scaleOld( p_transformable.getTransform().getScale() )
			{
			}

			virtual void execute() override { _transformable.getTransform().setScale( _scale ); }

			virtual void undo() override { _transformable.getTransform().setScale( _scaleOld ); }

		  private:
			Generic::BaseTransformable & _transformable;
			const float					 _scale;
			const Mat4f					 _scaleOld;
		};
	} // namespace Action
} // namespace VTX
#endif
