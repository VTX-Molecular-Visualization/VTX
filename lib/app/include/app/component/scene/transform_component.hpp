#ifndef __VTX_APP_COMPONENT_SCENE_TRANSFORM__
#define __VTX_APP_COMPONENT_SCENE_TRANSFORM__

#include "app/application/system/ecs_system.hpp"
#include "app/core/ecs/base_component.hpp"
#include <util/callback.hpp>
#include <util/math/transform.hpp>

namespace VTX::App::Component::Scene
{
	class Transform : public Core::ECS::BaseComponent
	{
	  public:
		Transform();
		Transform( Util::Math::Transform * p_transformPtr );

		const Util::Math::Transform & getTransform() const { return *_transformReference; };
		Util::Math::Transform &		  getTransform() { return *_transformReference; };

		void setTransform( Util::Math::Transform * p_transformPtr );

		void setPosition( const Vec3f & p_position );
		void setPosition( const Mat4f & p_positionMat );
		void localMove( const Vec3f & p_translation );
		void globalMove( const Vec3f & p_translation );
		void moveFront( const float p_distance );
		void moveRight( const float p_distance );
		void moveUp( const float p_distance );

		void setRotation( const Vec3f & p_euler );
		void setRotation( const Quatf & p_quat );
		void setRotation( const Mat4f & p_rotationMat );

		void localRotate( const Vec3f & p_euler );
		void globalRotate( const float p_angle, const Vec3f & p_axis );
		void rotatePitch( const float p_angle );
		void rotateYaw( const float p_angle );
		void rotateRoll( const float p_angle );

		void setScale( const float & p_scale );
		void setScale( const Vec3f & p_scaleVec );
		void setScale( const Mat4f & p_scaleMat );

		Vec3f getPosition() const;
		Vec3f getFront() const;
		Vec3f getRight() const;
		Vec3f getUp() const;

		Quatf getRotation() const;
		Vec3f getEuler() const;

		Vec3f getScale() const;

		void set( const Vec3f & p_position, const Quatf & p_rotation );
		void set( const Vec3f & p_position, const Quatf & p_rotation, const Vec3f & p_scale );

		void setRotationAround( const Quatf & p_rotation, const Vec3f & p_target, const float p_distance );

		void rotateAround( const Quatf & p_rotation, const Vec3f & p_target, const float p_distance );

		void lookAt( const Vec3f & p_target, const Vec3f & p_up );

		void applyTransform( const Util::Math::Transform & p_transform );

		Util::Callback<Util::Math::Transform &> onTransform;

	  private:
		void _updateRotation();
		void _updateRotation( const Quatf & p_rotation );

		Quatf _rotation = QUATF_ID;

		Util::Math::Transform *				   _transformReference;
		std::unique_ptr<Util::Math::Transform> _transformStorage = nullptr;
	};
} // namespace VTX::App::Component::Scene

#endif
