#ifndef __VTX_ACTION_TRANSLATE__
#define __VTX_ACTION_TRANSLATE__

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
		template<typename T, typename = std::enable_if<std::is_base_of<Generic::BaseTransformable, T>::value>>
		class Translate : public BaseActionUndonable
		{
		  public:
			explicit Translate( T & p_transformable, const Vec3f & p_translation ) :
				_transformable( p_transformable ), _translation( p_translation ),
				_translationOld( p_transformable.getTransform().getTranslation() )
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
