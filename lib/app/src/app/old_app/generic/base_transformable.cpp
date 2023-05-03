#include "app/old_app/generic/base_transformable.hpp"
#include "app/vtx_app.hpp"

namespace VTX
{
	namespace Generic
	{
		void BaseTransformable::rotate( const float p_angle, const Vec3f & p_axis )
		{
			_transform.rotate( p_angle, p_axis );
			_transformModifiedEvent();
		}

		void BaseTransformable::setRotation( const Vec3f & p_euler )
		{
			_transform.setRotation( p_euler );
			_transformModifiedEvent();
		}

		void BaseTransformable::setScale( const float & p_scale )
		{
			_transform.setScale( p_scale );
			_transformModifiedEvent();
		}

		void BaseTransformable::setScale( const Vec3f & p_scale )
		{
			_transform.setScale( p_scale );
			_transformModifiedEvent();
		}

		void BaseTransformable::setScale( const Mat4f & p_scale )
		{
			_transform.setScale( p_scale );
			_transformModifiedEvent();
		}

		void BaseTransformable::setTranslation( const Vec3f & p_translation )
		{
			_transform.setTranslation( p_translation );
			_transformModifiedEvent();
		}

		void BaseTransformable::setTranslation( const Mat4f & p_translation )
		{
			_transform.setTranslation( p_translation );
			_transformModifiedEvent();
		}

		void BaseTransformable::applyTransform( const App::Internal::Math::Transform & p_transform )
		{
			_transform = p_transform;
			_transformModifiedEvent();
		}

		void BaseTransformable::applyTransform( const App::Internal::Math::Transform &		 p_transform,
												const TransformComposantMask p_mask )
		{
			if ( p_mask == TransformComposantMask::TRANSFORM )
			{
				_transform = p_transform;
			}
			else
			{
				if ( bool( p_mask & TransformComposantMask::TRANSLATE_VECTOR ) )
				{
					const Vec3f appliedTranslation = p_transform.getTranslationVector();
					Vec3f		newTranslation	   = _transform.getTranslationVector();

					if ( bool( p_mask & TransformComposantMask::TRANSLATE_X ) )
						newTranslation.x = appliedTranslation.x;
					if ( bool( p_mask & TransformComposantMask::TRANSLATE_Y ) )
						newTranslation.y = appliedTranslation.y;
					if ( bool( p_mask & TransformComposantMask::TRANSLATE_Z ) )
						newTranslation.z = appliedTranslation.z;

					_transform.setTranslation( newTranslation );
				}

				if ( bool( p_mask & TransformComposantMask::EULER_VECTOR ) )
				{
					const Vec3f appliedEuler = p_transform.getEulerAngles();
					Vec3f		newEuler	 = _transform.getEulerAngles();

					if ( bool( p_mask & TransformComposantMask::EULER_X ) )
						newEuler.x = appliedEuler.x;
					if ( bool( p_mask & TransformComposantMask::EULER_Y ) )
						newEuler.y = appliedEuler.y;
					if ( bool( p_mask & TransformComposantMask::EULER_Z ) )
						newEuler.z = appliedEuler.z;

					_transform.setRotation( newEuler );
				}

				if ( bool( p_mask & TransformComposantMask::SCALE_VECTOR ) )
				{
					const Vec3f appliedScale = p_transform.getScaleVector();
					Vec3f		newScale	 = _transform.getScaleVector();

					if ( bool( p_mask & TransformComposantMask::SCALE_X ) )
						newScale.x = appliedScale.x;
					if ( bool( p_mask & TransformComposantMask::SCALE_Y ) )
						newScale.y = appliedScale.y;
					if ( bool( p_mask & TransformComposantMask::SCALE_Z ) )
						newScale.z = appliedScale.z;

					_transform.setScale( newScale );
				}
			}

			_transformModifiedEvent();
		}

	} // namespace Generic
} // namespace VTX
