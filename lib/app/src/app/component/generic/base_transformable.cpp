#include "app/component/generic/base_transformable.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Component::Generic
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

	void BaseTransformable::applyTransform( const App::Internal::Math::Transform &		   p_transform,
											const Internal::Math::TRANSFORM_COMPOSANT_MASK p_mask )
	{
		if ( p_mask == Internal::Math::TRANSFORM_COMPOSANT_MASK::ENUM::TRANSFORM )
		{
			_transform = p_transform;
		}
		else
		{
			if ( p_mask & Internal::Math::TRANSFORM_COMPOSANT_MASK::ENUM::TRANSLATE_VECTOR )
			{
				const Vec3f appliedTranslation = p_transform.getTranslationVector();
				Vec3f		newTranslation	   = _transform.getTranslationVector();

				if ( p_mask & Internal::Math::TRANSFORM_COMPOSANT_MASK::ENUM::TRANSLATE_X )
					newTranslation.x = appliedTranslation.x;
				if ( p_mask & Internal::Math::TRANSFORM_COMPOSANT_MASK::ENUM::TRANSLATE_Y )
					newTranslation.y = appliedTranslation.y;
				if ( p_mask & Internal::Math::TRANSFORM_COMPOSANT_MASK::ENUM::TRANSLATE_Z )
					newTranslation.z = appliedTranslation.z;

				_transform.setTranslation( newTranslation );
			}

			if ( p_mask & Internal::Math::TRANSFORM_COMPOSANT_MASK::ENUM::EULER_VECTOR )
			{
				const Vec3f appliedEuler = p_transform.getEulerAngles();
				Vec3f		newEuler	 = _transform.getEulerAngles();

				if ( p_mask & Internal::Math::TRANSFORM_COMPOSANT_MASK::ENUM::EULER_X )
					newEuler.x = appliedEuler.x;
				if ( p_mask & Internal::Math::TRANSFORM_COMPOSANT_MASK::ENUM::EULER_Y )
					newEuler.y = appliedEuler.y;
				if ( p_mask & Internal::Math::TRANSFORM_COMPOSANT_MASK::ENUM::EULER_Z )
					newEuler.z = appliedEuler.z;

				_transform.setRotation( newEuler );
			}

			if ( p_mask & Internal::Math::TRANSFORM_COMPOSANT_MASK::ENUM::SCALE_VECTOR )
			{
				const Vec3f appliedScale = p_transform.getScaleVector();
				Vec3f		newScale	 = _transform.getScaleVector();

				if ( p_mask & Internal::Math::TRANSFORM_COMPOSANT_MASK::ENUM::SCALE_X )
					newScale.x = appliedScale.x;
				if ( p_mask & Internal::Math::TRANSFORM_COMPOSANT_MASK::ENUM::SCALE_Y )
					newScale.y = appliedScale.y;
				if ( p_mask & Internal::Math::TRANSFORM_COMPOSANT_MASK::ENUM::SCALE_Z )
					newScale.z = appliedScale.z;

				_transform.setScale( newScale );
			}
		}

		_transformModifiedEvent();
	}
} // namespace VTX::App::Component::Generic
