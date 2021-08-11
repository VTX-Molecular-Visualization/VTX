#ifndef __VTX_ACTION_TRANSFORMABLE__
#define __VTX_ACTION_TRANSFORMABLE__

#include "base_action.hpp"
#include "base_action_undonable.hpp"
#include "generic/base_auto_rotate.hpp"
#include "generic/base_transformable.hpp"
#include "math/transform.hpp"
#include "model/molecule.hpp"
#include "vtx_app.hpp"
#include <unordered_set>

namespace VTX::Action::Transformable
{
	class SetTranslation : public BaseActionUndonable
	{
	  public:
		explicit SetTranslation( Generic::BaseTransformable & p_transformable, const Vec3f & p_translation ) :
			_transformable( p_transformable ), _translation( p_translation ),
			_translationOld( p_transformable.getTransform().getTranslation() )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			_transformable.setTranslation( _translation );
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

		virtual void undo() override
		{
			_transformable.setTranslation( _translationOld );
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		Generic::BaseTransformable & _transformable;
		const Vec3f					 _translation;
		const Mat4f					 _translationOld;
	};
	class Translate : public BaseAction
	{
	  public:
		explicit Translate( Generic::BaseTransformable & p_transformable, const Vec3f & p_delta ) :
			_transformables { &p_transformable }, _delta( p_delta )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		explicit Translate( std::unordered_set<Generic::BaseTransformable *> & p_transformables,
							const Vec3f &									   p_delta ) :
			_transformables( p_transformables ),
			_delta( p_delta )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			for ( Generic::BaseTransformable * transformable : _transformables )
			{
				transformable->setTranslation( transformable->getTransform().getTranslationVector() + _delta );
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		std::unordered_set<Generic::BaseTransformable *> _transformables;
		const Vec3f										 _delta;
	};

	class SetRotation : public BaseActionUndonable
	{
	  public:
		explicit SetRotation( Generic::BaseTransformable & p_transformable, const Vec3f & p_euler ) :
			_transformable( p_transformable ), _euler( p_euler ),
			_eulerOld( p_transformable.getTransform().getEulerAngles() )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			_transformable.setRotation( _euler );
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

		virtual void undo() override
		{
			_transformable.setRotation( _eulerOld );
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		Generic::BaseTransformable & _transformable;
		const Vec3f					 _euler;
		const Vec3f					 _eulerOld;
	};
	class Rotate : public BaseAction
	{
	  private:
		enum class RotationType
		{
			Axis_Angle,
			Euler
		};

	  public:
		explicit Rotate( Generic::BaseTransformable & p_transformable, const float p_angle, const Vec3f & p_axis ) :
			_transformables { &p_transformable }, _angle( p_angle ), _axis( p_axis ),
			_rotationType( RotationType::Axis_Angle )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		explicit Rotate( const std::unordered_set<Generic::BaseTransformable *> & p_transformables,
						 const float											  p_angle,
						 const Vec3f &											  p_axis ) :
			_transformables( p_transformables ),
			_angle( p_angle ), _axis( p_axis ), _rotationType( RotationType::Axis_Angle )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		explicit Rotate( Generic::BaseTransformable & p_transformable, const Vec3f & p_euler ) :
			_transformables { &p_transformable }, _axis( p_euler ), _angle( 0 ), _rotationType( RotationType::Euler )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		explicit Rotate( const std::unordered_set<Generic::BaseTransformable *> & p_transformables,
						 const Vec3f &											  p_euler ) :
			_transformables( p_transformables ),
			_axis( p_euler ), _angle( 0 ), _rotationType( RotationType::Euler )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			for ( Generic::BaseTransformable * const transformable : _transformables )
			{
				switch ( _rotationType )
				{
				case RotationType::Axis_Angle: transformable->rotate( _angle, _axis ); break;
				case RotationType::Euler:
					transformable->setRotation( transformable->getTransform().getEulerAngles() + _axis );
					break;
				}
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		std::unordered_set<Generic::BaseTransformable *> _transformables;
		RotationType									 _rotationType;
		const float										 _angle;
		const Vec3f										 _axis;
	};

	class SetScale : public BaseActionUndonable
	{
	  public:
		explicit SetScale( Generic::BaseTransformable & p_transformable, const float p_scale ) :
			_transformable( p_transformable ), _scale( p_scale ), _scaleOld( p_transformable.getTransform().getScale() )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			_transformable.setScale( _scale );
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

		virtual void undo() override
		{
			_transformable.setScale( _scaleOld );
			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		Generic::BaseTransformable & _transformable;
		const float					 _scale;
		const Mat4f					 _scaleOld;
	};
	class Scale : public BaseAction
	{
	  public:
		explicit Scale( Generic::BaseTransformable & p_transformable, const Vec3f & p_delta ) :
			_transformables { &p_transformable }, _delta( p_delta )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		explicit Scale( std::unordered_set<Generic::BaseTransformable *> & p_transformables, const Vec3f & p_delta ) :
			_transformables( p_transformables ), _delta( p_delta )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			for ( Generic::BaseTransformable * transformable : _transformables )
			{
				transformable->setScale( transformable->getTransform().getScaleVector() + _delta );
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		std::unordered_set<Generic::BaseTransformable *> _transformables;
		const Vec3f										 _delta;
	};

	class ApplyTransform : public BaseAction
	{
	  public:
		explicit ApplyTransform( Model::Molecule &										  p_transformable,
								 const Math::Transform &								  p_transform,
								 const Generic::BaseTransformable::TransformComposantMask p_mask
								 = Generic::BaseTransformable::TransformComposantMask::TRANSFORM ) :
			_transform( p_transform ),
			_mask( p_mask ), _transformables { &p_transformable }
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}
		explicit ApplyTransform( const std::unordered_set<Model::Molecule *> &			  p_transformables,
								 const Math::Transform &								  p_transform,
								 const Generic::BaseTransformable::TransformComposantMask p_mask
								 = Generic::BaseTransformable::TransformComposantMask::TRANSFORM ) :
			_transform( p_transform ),
			_mask( p_mask )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );

			_transformables.reserve( p_transformables.size() );
			for ( Model::Molecule * const molecule : p_transformables )
			{
				_transformables.emplace_back( molecule );
			}
		}

		virtual void execute() override
		{
			for ( Generic::BaseTransformable * const transformable : _transformables )
			{
				transformable->applyTransform( _transform, _mask );
			}

			VTXApp::get().MASK |= VTX_MASK_3D_MODEL_UPDATED;
		}

	  private:
		const Math::Transform									 _transform;
		const Generic::BaseTransformable::TransformComposantMask _mask;
		std::vector<Generic::BaseTransformable *> _transformables = std::vector<Generic::BaseTransformable *>();
	};

	class SetAutoRotationOrientation : public BaseAction
	{
	  public:
		explicit SetAutoRotationOrientation( const std::unordered_set<Generic::BaseAutoRotate *> p_autoRotateComponent,
											 const Vec3f &										 p_orientation ) :
			_autoRotateComponents( p_autoRotateComponent ),
			_orientation( p_orientation )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			for ( Generic::BaseAutoRotate * const autoRotateComponent : _autoRotateComponents )
				autoRotateComponent->setAutoRotationVector( _orientation );
		}

	  private:
		std::unordered_set<Generic::BaseAutoRotate *> _autoRotateComponents;
		const Vec3f									  _orientation;
	};
	class SetAutoRotationSpeed : public BaseAction
	{
	  public:
		explicit SetAutoRotationSpeed( const std::unordered_set<Generic::BaseAutoRotate *> p_autoRotateComponent,
									   const float										   p_speed ) :
			_autoRotateComponents( p_autoRotateComponent ),
			_speed( p_speed )
		{
			_tag = ACTION_TAG( _tag | ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override
		{
			for ( Generic::BaseAutoRotate * const autoRotateComponent : _autoRotateComponents )
				autoRotateComponent->setAutoRotationMagnitude( _speed );
		}

	  private:
		std::unordered_set<Generic::BaseAutoRotate *> _autoRotateComponents;
		const float									  _speed;
	};
	class SetAutoRotationPlay : public BaseAction
	{
	  public:
		explicit SetAutoRotationPlay( const std::unordered_set<Generic::BaseAutoRotate *> p_autoRotateComponent,
									  const bool										  p_play ) :
			_autoRotateComponents( p_autoRotateComponent ),
			_play( p_play )
		{
		}

		virtual void execute() override
		{
			for ( Generic::BaseAutoRotate * const autoRotateComponent : _autoRotateComponents )
				autoRotateComponent->setAutoRotationPlaying( _play );
		}

	  private:
		std::unordered_set<Generic::BaseAutoRotate *> _autoRotateComponents;
		const bool									  _play;
	};
} // namespace VTX::Action::Transformable
#endif
