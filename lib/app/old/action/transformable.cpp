#include "app/old/action/transformable.hpp"
#include "app/old/application/setting.hpp"
#include "app/old/vtx_app.hpp"
#include <util/math.hpp>

namespace VTX::App::Old::Action::Transformable
{
	void SetTranslation::execute()
	{
		_transformable.setTranslation( _translation );
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void SetTranslation::undo()
	{
		_transformable.setTranslation( _translationOld );
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}
	void Translate::execute()
	{
		for ( Component::Generic::BaseTransformable * transformable : _transformables )
		{
			Vec3f newPos = transformable->getTransform().getTranslationVector() + _delta;
			newPos		 = Util::Math::clamp( newPos, VTX::App::Old::Application::Setting::MIN_SCENE_POS, VTX::App::Old::Application::Setting::MAX_SCENE_POS );
			transformable->setTranslation( newPos );
		}

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void SetRotation::execute()
	{
		_transformable.setRotation( _euler );
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void SetRotation::undo()
	{
		_transformable.setRotation( _eulerOld );
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void Rotate::execute()
	{
		for ( Component::Generic::BaseTransformable * const transformable : _transformables )
		{
			switch ( _rotationType )
			{
			case RotationType::Axis_Angle: transformable->rotate( _angle, _axis ); break;
			case RotationType::Euler:
				Vec3f newEuler = transformable->getTransform().getEulerAngles() + _axis;
				newEuler = Util::Math::clamp( newEuler, VTX::App::Old::Application::Setting::MIN_EULER_VEC, VTX::App::Old::Application::Setting::MAX_EULER_VEC );
				transformable->setRotation( newEuler );
				break;
			}
		}

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void SetScale::execute()
	{
		_transformable.setScale( _scale );
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void SetScale::undo()
	{
		_transformable.setScale( _scaleOld );
		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void Scale::execute()
	{
		for ( Component::Generic::BaseTransformable * transformable : _transformables )
		{
			Vec3f newScale = transformable->getTransform().getScaleVector() + _delta;
			newScale	   = Util::Math::clamp( newScale, VTX::App::Old::Application::Setting::MIN_SCALE_VEC, VTX::App::Old::Application::Setting::MAX_SCALE_VEC );

			transformable->setScale( newScale );
		}

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void ApplyTransform::execute()
	{
		for ( Component::Generic::BaseTransformable * const transformable : _transformables )
		{
			transformable->applyTransform( _transform, _mask );
		}

		App::Old::VTXApp::get().MASK |= Render::VTX_MASK_3D_MODEL_UPDATED;
	}

	void SetAutoRotationOrientation::execute()
	{
		for ( Component::Generic::BaseAutoRotate * const autoRotateComponent : _autoRotateComponents )
			autoRotateComponent->setAutoRotationVector( _orientation );
	}

	void AddToAutoRotationOrientation::execute()
	{
		for ( Component::Generic::BaseAutoRotate * const autoRotateComponent : _autoRotateComponents )
			autoRotateComponent->setAutoRotationVector( autoRotateComponent->getAutoRotationVector() + _delta );
	}

	void SetAutoRotationSpeed::execute()
	{
		for ( Component::Generic::BaseAutoRotate * const autoRotateComponent : _autoRotateComponents )
			autoRotateComponent->setAutoRotationMagnitude( _speed );
	}

	void SetAutoRotationPlay::execute()
	{
		for ( Component::Generic::BaseAutoRotate * const autoRotateComponent : _autoRotateComponents )
			autoRotateComponent->setAutoRotationPlaying( _play );
	}
} // namespace VTX::App::Old::Action::Transformable