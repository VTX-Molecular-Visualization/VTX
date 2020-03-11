#ifndef __VTX_ACTION_TRANSFORMABLE_TRANSLATE__
#define __VTX_ACTION_TRANSFORMABLE_TRANSLATE__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_action_undonable.hpp"
#include "generic/base_transformable.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Action
	{
		class TransformableTranslate : public BaseActionUndonable
		{
		  public:
			explicit TransformableTranslate( Generic::BaseTransformable & p_transformable,
											 const Vec3f &				  p_translation ) :
				_transformable( p_transformable ),
				_translation( p_translation ), _translationOld( p_transformable.getTransform().getTranslation() )
			{
			}

			virtual void execute() override { _transformable.setTranslation( _translation ); }

			virtual void undo() override { _transformable.setTranslation( _translationOld ); }

		  private:
			Generic::BaseTransformable & _transformable;
			const Vec3f					 _translation;
			const Mat4f					 _translationOld;
		};
	} // namespace Action
} // namespace VTX
#endif
