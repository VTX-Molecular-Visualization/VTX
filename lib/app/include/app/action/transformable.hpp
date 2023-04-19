#ifndef __VTX_ACTION_TRANSFORMABLE__
#define __VTX_ACTION_TRANSFORMABLE__

#include "app/core/action/base_action.hpp"
#include "app/core/action/base_action_undonable.hpp"
#include "app/old_app/generic/base_auto_rotate.hpp"
#include "app/old_app/generic/base_transformable.hpp"
#include "app/old_app/math/transform.hpp"
#include "app/old_app/model/molecule.hpp"
#include <unordered_set>
#include <util/types.hpp>
#include <vector>

namespace VTX::Action::Transformable
{
	class SetTranslation : public Core::Action::BaseActionUndonable
	{
	  public:
		explicit SetTranslation( Generic::BaseTransformable & p_transformable, const Vec3f & p_translation ) :
			_transformable( p_transformable ), _translation( p_translation ),
			_translationOld( p_transformable.getTransform().getTranslation() )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;
		virtual void undo() override;

	  private:
		Generic::BaseTransformable & _transformable;
		const Vec3f					 _translation;
		const Mat4f					 _translationOld;
	};
	class Translate : public Core::Action::BaseAction
	{
	  public:
		explicit Translate( Generic::BaseTransformable & p_transformable, const Vec3f & p_delta ) :
			_transformables { &p_transformable }, _delta( p_delta )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit Translate( std::unordered_set<Generic::BaseTransformable *> & p_transformables,
							const Vec3f &									   p_delta ) :
			_transformables( p_transformables ),
			_delta( p_delta )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		std::unordered_set<Generic::BaseTransformable *> _transformables;
		const Vec3f										 _delta;
	};

	class SetRotation : public Core::Action::BaseActionUndonable
	{
	  public:
		explicit SetRotation( Generic::BaseTransformable & p_transformable, const Vec3f & p_euler ) :
			_transformable( p_transformable ), _euler( p_euler ),
			_eulerOld( p_transformable.getTransform().getEulerAngles() )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;
		virtual void undo() override;

	  private:
		Generic::BaseTransformable & _transformable;
		const Vec3f					 _euler;
		const Vec3f					 _eulerOld;
	};
	class Rotate : public Core::Action::BaseAction
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
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit Rotate( const std::unordered_set<Generic::BaseTransformable *> & p_transformables,
						 const float											  p_angle,
						 const Vec3f &											  p_axis ) :
			_transformables( p_transformables ),
			_angle( p_angle ), _axis( p_axis ), _rotationType( RotationType::Axis_Angle )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		explicit Rotate( Generic::BaseTransformable & p_transformable, const Vec3f & p_euler ) :
			_transformables { &p_transformable }, _axis( p_euler ), _angle( 0 ), _rotationType( RotationType::Euler )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit Rotate( const std::unordered_set<Generic::BaseTransformable *> & p_transformables,
						 const Vec3f &											  p_euler ) :
			_transformables( p_transformables ),
			_axis( p_euler ), _angle( 0 ), _rotationType( RotationType::Euler )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		std::unordered_set<Generic::BaseTransformable *> _transformables;
		RotationType									 _rotationType;
		const float										 _angle;
		const Vec3f										 _axis;
	};

	class SetScale : public Core::Action::BaseActionUndonable
	{
	  public:
		explicit SetScale( Generic::BaseTransformable & p_transformable, const float p_scale ) :
			_transformable( p_transformable ), _scale( p_scale ), _scaleOld( p_transformable.getTransform().getScale() )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;
		virtual void undo() override;

	  private:
		Generic::BaseTransformable & _transformable;
		const float					 _scale;
		const Mat4f					 _scaleOld;
	};
	class Scale : public Core::Action::BaseAction
	{
	  public:
		explicit Scale( Generic::BaseTransformable & p_transformable, const Vec3f & p_delta ) :
			_transformables { &p_transformable }, _delta( p_delta )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit Scale( std::unordered_set<Generic::BaseTransformable *> & p_transformables, const Vec3f & p_delta ) :
			_transformables( p_transformables ), _delta( p_delta )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		std::unordered_set<Generic::BaseTransformable *> _transformables;
		const Vec3f										 _delta;
	};

	class ApplyTransform : public Core::Action::BaseAction
	{
	  public:
		explicit ApplyTransform( Model::Molecule &										  p_transformable,
								 const Math::Transform &								  p_transform,
								 const Generic::BaseTransformable::TransformComposantMask p_mask
								 = Generic::BaseTransformable::TransformComposantMask::TRANSFORM ) :
			_transform( p_transform ),
			_mask( p_mask ), _transformables { &p_transformable }
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit ApplyTransform( const std::unordered_set<Model::Molecule *> &			  p_transformables,
								 const Math::Transform &								  p_transform,
								 const Generic::BaseTransformable::TransformComposantMask p_mask
								 = Generic::BaseTransformable::TransformComposantMask::TRANSFORM ) :
			_transform( p_transform ),
			_mask( p_mask )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );

			_transformables.reserve( p_transformables.size() );
			for ( Model::Molecule * const molecule : p_transformables )
			{
				_transformables.emplace_back( molecule );
			}
		}

		virtual void execute() override;

	  private:
		const Math::Transform									 _transform;
		const Generic::BaseTransformable::TransformComposantMask _mask;
		std::vector<Generic::BaseTransformable *> _transformables = std::vector<Generic::BaseTransformable *>();
	};

	class SetAutoRotationOrientation : public Core::Action::BaseAction
	{
	  public:
		explicit SetAutoRotationOrientation( const std::unordered_set<Generic::BaseAutoRotate *> p_autoRotateComponent,
											 const Vec3f &										 p_orientation ) :
			_autoRotateComponents( p_autoRotateComponent ),
			_orientation( p_orientation )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		std::unordered_set<Generic::BaseAutoRotate *> _autoRotateComponents;
		const Vec3f									  _orientation;
	};
	class AddToAutoRotationOrientation : public Core::Action::BaseAction
	{
	  public:
		explicit AddToAutoRotationOrientation(
			const std::unordered_set<Generic::BaseAutoRotate *> p_autoRotateComponent,
			const Vec3f &										p_delta ) :
			_autoRotateComponents( p_autoRotateComponent ),
			_delta( p_delta )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		std::unordered_set<Generic::BaseAutoRotate *> _autoRotateComponents;
		const Vec3f									  _delta;
	};

	class SetAutoRotationSpeed : public Core::Action::BaseAction
	{
	  public:
		explicit SetAutoRotationSpeed( const std::unordered_set<Generic::BaseAutoRotate *> p_autoRotateComponent,
									   const float										   p_speed ) :
			_autoRotateComponents( p_autoRotateComponent ),
			_speed( p_speed )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		std::unordered_set<Generic::BaseAutoRotate *> _autoRotateComponents;
		const float									  _speed;
	};
	class SetAutoRotationPlay : public Core::Action::BaseAction
	{
	  public:
		explicit SetAutoRotationPlay( const std::unordered_set<Generic::BaseAutoRotate *> p_autoRotateComponent,
									  const bool										  p_play ) :
			_autoRotateComponents( p_autoRotateComponent ),
			_play( p_play )
		{
		}

		virtual void execute() override;

	  private:
		std::unordered_set<Generic::BaseAutoRotate *> _autoRotateComponents;
		const bool									  _play;
	};
} // namespace VTX::Action::Transformable
#endif
