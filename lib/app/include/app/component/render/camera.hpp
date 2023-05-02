#ifndef __VTX_APP_COMPONENT_RENDER_CAMERA__
#define __VTX_APP_COMPONENT_RENDER_CAMERA__

#include "app/component/render/enum_camera.hpp"
#include "app/old_app/define.hpp"
#include <util/math.hpp>
#include <util/types.hpp>

namespace VTX::App::Component::Render
{
	class Camera
	{
	  public:
		Camera();

		inline const Vec3f & getPosition() const { return _position; }
		inline const Quatf & getRotation() const { return _rotation; }

		inline const Mat4f & getViewMatrix() const { return _viewMatrix; }
		inline const Mat4f & getProjectionMatrix() const { return _projectionMatrix; }

		inline const uint  getScreenWidth() const { return _screenWidth; }
		inline const uint  getScreenHeight() const { return _screenHeight; }
		inline const float getAspectRatio() const { return _aspectRatio; }

		inline const Vec3f & getFront() const { return _front; }
		inline const Vec3f & getRight() const { return _right; }
		inline const Vec3f & getUp() const { return _up; }

		inline const float getNear() const { return _near; }
		inline const float getFar() const { return _far; }
		inline const float getFov() const { return _fov; }

		inline const Vec3f & getTarget() const { return _target; }

		inline const CAMERA_PROJECTION & getProjection() { return _projection; }
		void							 setCameraProjection( const CAMERA_PROJECTION & p_projection );

		const bool isPerspective() const { return _projection == CAMERA_PROJECTION::PERSPECTIVE; }

		inline void setScreenSize( const uint p_width, const uint p_height )
		{
			_screenWidth  = p_width;
			_screenHeight = p_height;
			_aspectRatio  = (float)_screenWidth / (float)_screenHeight;
			_updateProjectionMatrix();
		}

		inline void setPosition( const Vec3f & p_position )
		{
			_position = p_position;
			_updateViewMatrix();
		}

		inline void setRotation( const Quatf & p_rotation )
		{
			_rotation = Util::Math::normalize( p_rotation );
			_updateRotation();
		}

		inline void setRotation( const Vec3f & p_rotation )
		{
			_rotation = Util::Math::normalize( Util::Math::eulerToQuaternion( p_rotation ) );
			_updateRotation();
		}

		inline void set( const Vec3f & p_position, const Quatf & p_rotation )
		{
			_position = p_position;
			setRotation( p_rotation );
		}

		void setFrontRightUp( const Vec3f & p_front, const Vec3f & p_right, const Vec3f & p_up );

		void setNear( const float p_near );
		void setFar( const float p_far );
		void setFov( const float p_fov );

		virtual void move( const Vec3f & );
		virtual void moveFront( const float );
		virtual void moveRight( const float );
		virtual void moveUp( const float );

		virtual void  setTarget( const Vec3f & p_target );
		virtual float getDistanceToTarget() const;

		void rotate( const Vec3f & );
		void rotatePitch( const float );
		void rotateYaw( const float );
		void rotateRoll( const float );

		void		 setRotationAround( const Quatf & p_rotation, const Vec3f & p_target, const float p_distance );
		virtual void rotateAround( const Quatf &, const Vec3f &, const float );
		void		 lookAt( const Vec3f &, const Vec3f & );

		void reset( const Vec3f & p_defaultPosition = VEC3F_ZERO );

		void print() const;

	  protected:
		uint  _screenWidth	= 1u;
		uint  _screenHeight = 1u;
		float _aspectRatio	= 1.f;
		float _near			= 1e-1f;
		float _far			= 1e4f;
		float _fov			= 60.f;

		Vec3f _position = VEC3F_ZERO;
		Quatf _rotation = QUATF_ID;

		Vec3f _front = CAMERA_FRONT_DEFAULT;
		Vec3f _right = CAMERA_RIGHT_DEFAULT;
		Vec3f _up	 = CAMERA_UP_DEFAULT;

		Vec3f			  _target	  = VEC3F_ZERO;
		CAMERA_PROJECTION _projection = CAMERA_PROJECTION::PERSPECTIVE;

		Mat4f _viewMatrix;
		Mat4f _projectionMatrix;

		void		 _updateRotation();
		virtual void _updateViewMatrix();

		void _updateProjectionMatrix();
		void _computePerspectiveProjectionMatrix();
		void _computeOrthographicProjectionMatrix();

	}; // namespace Camera
} // namespace VTX::App::Component::Render
#endif
