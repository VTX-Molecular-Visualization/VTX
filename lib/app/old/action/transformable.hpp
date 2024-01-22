#ifndef __VTX_APP_OLD_ACTION_TRANSFORMABLE__
#define __VTX_APP_OLD_ACTION_TRANSFORMABLE__

#include "app/old/action.hpp"
#include "app/old/component/chemistry/molecule.hpp"
#include "app/old/component/generic/base_auto_rotate.hpp"
#include "app/old/component/generic/base_transformable.hpp"
#include "app/old/core/action/base_action.hpp"
#include "app/old/core/action/base_action_undonable.hpp"
#include "app/old/internal/math/transform.hpp"
#include <unordered_set>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Old::Action::Transformable
{
	class SetTranslation : public App::Old::Core::Action::BaseActionUndonable
	{
	  public:
		explicit SetTranslation( Component::Generic::BaseTransformable & p_transformable,
								 const Vec3f &							 p_translation ) :
			_transformable( p_transformable ),
			_translation( p_translation ), _translationOld( p_transformable.getTransform().getTranslation() )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;
		virtual void undo() override;

	  private:
		Component::Generic::BaseTransformable & _transformable;
		const Vec3f								_translation;
		const Mat4f								_translationOld;
	};
	class Translate : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit Translate( Component::Generic::BaseTransformable & p_transformable, const Vec3f & p_delta ) :
			_transformables { &p_transformable }, _delta( p_delta )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit Translate( std::unordered_set<Component::Generic::BaseTransformable *> & p_transformables,
							const Vec3f &												  p_delta ) :
			_transformables( p_transformables ),
			_delta( p_delta )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		std::unordered_set<Component::Generic::BaseTransformable *> _transformables;
		const Vec3f													_delta;
	};

	class SetRotation : public App::Old::Core::Action::BaseActionUndonable
	{
	  public:
		explicit SetRotation( Component::Generic::BaseTransformable & p_transformable, const Vec3f & p_euler ) :
			_transformable( p_transformable ), _euler( p_euler ),
			_eulerOld( p_transformable.getTransform().getEulerAngles() )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;
		virtual void undo() override;

	  private:
		Component::Generic::BaseTransformable & _transformable;
		const Vec3f								_euler;
		const Vec3f								_eulerOld;
	};
	class Rotate : public App::Old::Core::Action::BaseAction
	{
	  private:
		enum class RotationType
		{
			Axis_Angle,
			Euler
		};

	  public:
		explicit Rotate( Component::Generic::BaseTransformable & p_transformable,
						 const float							 p_angle,
						 const Vec3f &							 p_axis ) :
			_transformables { &p_transformable },
			_angle( p_angle ), _axis( p_axis ), _rotationType( RotationType::Axis_Angle )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit Rotate( const std::unordered_set<Component::Generic::BaseTransformable *> & p_transformables,
						 const float														 p_angle,
						 const Vec3f &														 p_axis ) :
			_transformables( p_transformables ),
			_angle( p_angle ), _axis( p_axis ), _rotationType( RotationType::Axis_Angle )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		explicit Rotate( Component::Generic::BaseTransformable & p_transformable, const Vec3f & p_euler ) :
			_transformables { &p_transformable }, _axis( p_euler ), _angle( 0 ), _rotationType( RotationType::Euler )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit Rotate( const std::unordered_set<Component::Generic::BaseTransformable *> & p_transformables,
						 const Vec3f &														 p_euler ) :
			_transformables( p_transformables ),
			_axis( p_euler ), _angle( 0 ), _rotationType( RotationType::Euler )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		std::unordered_set<Component::Generic::BaseTransformable *> _transformables;
		RotationType												_rotationType;
		const float													_angle;
		const Vec3f													_axis;
	};

	class SetScale : public App::Old::Core::Action::BaseActionUndonable
	{
	  public:
		explicit SetScale( Component::Generic::BaseTransformable & p_transformable, const float p_scale ) :
			_transformable( p_transformable ), _scale( p_scale ), _scaleOld( p_transformable.getTransform().getScale() )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;
		virtual void undo() override;

	  private:
		Component::Generic::BaseTransformable & _transformable;
		const float								_scale;
		const Mat4f								_scaleOld;
	};
	class Scale : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit Scale( Component::Generic::BaseTransformable & p_transformable, const Vec3f & p_delta ) :
			_transformables { &p_transformable }, _delta( p_delta )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit Scale( std::unordered_set<Component::Generic::BaseTransformable *> & p_transformables,
						const Vec3f &												  p_delta ) :
			_transformables( p_transformables ),
			_delta( p_delta )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		std::unordered_set<Component::Generic::BaseTransformable *> _transformables;
		const Vec3f													_delta;
	};

	class ApplyTransform : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit ApplyTransform( App::Old::Component::Chemistry::Molecule &			p_transformable,
								 const App::Old::Internal::Math::Transform &			p_transform,
								 const Internal::Math::TRANSFORM_COMPOSANT_MASK p_mask
								 = Internal::Math::TRANSFORM_COMPOSANT_MASK::ENUM::TRANSFORM ) :
			_transform( p_transform ),
			_mask( p_mask ), _transformables { &p_transformable }
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}
		explicit ApplyTransform( const std::unordered_set<App::Old::Component::Chemistry::Molecule *> & p_transformables,
								 const App::Old::Internal::Math::Transform &							   p_transform,
								 const Internal::Math::TRANSFORM_COMPOSANT_MASK					   p_mask
								 = Internal::Math::TRANSFORM_COMPOSANT_MASK::ENUM::TRANSFORM ) :
			_transform( p_transform ),
			_mask( p_mask )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );

			_transformables.reserve( p_transformables.size() );
			for ( App::Old::Component::Chemistry::Molecule * const molecule : p_transformables )
			{
				_transformables.emplace_back( molecule );
			}
		}

		virtual void execute() override;

	  private:
		const App::Old::Internal::Math::Transform				 _transform;
		const Internal::Math::TRANSFORM_COMPOSANT_MASK		 _mask;
		std::vector<Component::Generic::BaseTransformable *> _transformables
			= std::vector<Component::Generic::BaseTransformable *>();
	};

	class SetAutoRotationOrientation : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit SetAutoRotationOrientation(
			const std::unordered_set<Component::Generic::BaseAutoRotate *> p_autoRotateComponent,
			const Vec3f &												   p_orientation ) :
			_autoRotateComponents( p_autoRotateComponent ),
			_orientation( p_orientation )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		std::unordered_set<Component::Generic::BaseAutoRotate *> _autoRotateComponents;
		const Vec3f												 _orientation;
	};
	class AddToAutoRotationOrientation : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit AddToAutoRotationOrientation(
			const std::unordered_set<Component::Generic::BaseAutoRotate *> p_autoRotateComponent,
			const Vec3f &												   p_delta ) :
			_autoRotateComponents( p_autoRotateComponent ),
			_delta( p_delta )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		std::unordered_set<Component::Generic::BaseAutoRotate *> _autoRotateComponents;
		const Vec3f												 _delta;
	};

	class SetAutoRotationSpeed : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit SetAutoRotationSpeed(
			const std::unordered_set<Component::Generic::BaseAutoRotate *> p_autoRotateComponent,
			const float													   p_speed ) :
			_autoRotateComponents( p_autoRotateComponent ),
			_speed( p_speed )
		{
			_tag = Core::Action::ACTION_TAG( _tag | Core::Action::ACTION_TAG::MODIFY_SCENE );
		}

		virtual void execute() override;

	  private:
		std::unordered_set<Component::Generic::BaseAutoRotate *> _autoRotateComponents;
		const float												 _speed;
	};
	class SetAutoRotationPlay : public App::Old::Core::Action::BaseAction
	{
	  public:
		explicit SetAutoRotationPlay(
			const std::unordered_set<Component::Generic::BaseAutoRotate *> p_autoRotateComponent,
			const bool													   p_play ) :
			_autoRotateComponents( p_autoRotateComponent ),
			_play( p_play )
		{
		}

		virtual void execute() override;

	  private:
		std::unordered_set<Component::Generic::BaseAutoRotate *> _autoRotateComponents;
		const bool												 _play;
	};
} // namespace VTX::App::Old::Action::Transformable
#endif