#ifndef __VTX_APP_COMPONENT_RENDER_CAMERA__
#define __VTX_APP_COMPONENT_RENDER_CAMERA__

#include "app/component/render/enum_camera.hpp"
#include "app/component/scene/transform_component.hpp"
#include "app/internal/constants.hpp"
#include <util/callback.hpp>
#include <util/types.hpp>

namespace VTX::App::Component::Render
{
	class Camera
	{
	  public:
		Camera();

		void init();

		inline const Component::Scene::Transform & getTransform() const { return *_transform; }
		inline Component::Scene::Transform &	   getTransform() { return *_transform; }

		inline const Mat4f & getViewMatrix() const { return _viewMatrix; }
		inline const Mat4f & getProjectionMatrix() const { return _projectionMatrix; }

		inline const uint  getScreenWidth() const { return _screenWidth; }
		inline const uint  getScreenHeight() const { return _screenHeight; }
		inline const float getAspectRatio() const { return _aspectRatio; }

		inline const float getNear() const { return _near; }
		inline const float getFar() const { return _far; }
		inline const float getFov() const { return _fov; }

		inline const Vec3f & getTarget() const { return _target; }

		inline const CAMERA_PROJECTION & getProjection() { return _projection; }
		void							 setCameraProjection( const CAMERA_PROJECTION & p_projection );

		const bool isPerspective() const { return _projection == CAMERA_PROJECTION::PERSPECTIVE; }

		void setScreenSize( const uint p_width, const uint p_height );

		void setNear( const float p_near );
		void setFar( const float p_far );
		void setFov( const float p_fov );

		virtual void  setTargetWorld( const Vec3f & p_target );
		virtual void  setTargetLocal( const Vec3f & p_target );
		void		  attachTarget();
		void		  detachTarget();
		virtual float getDistanceToTarget() const;

		void reset( const Vec3f & p_defaultPosition = VEC3F_ZERO );

		void print() const;

		Util::Callback<Mat4f>			  onMatrixViewChange;
		Util::Callback<Mat4f>			  onMatrixProjectionChange;
		Util::Callback<float, float>	  onClipInfosChange;
		Util::Callback<CAMERA_PROJECTION> onProjectionChange;

	  protected:
		uint  _screenWidth	= 1u;
		uint  _screenHeight = 1u;
		float _aspectRatio	= 1.f;
		float _near			= 1e-1f;
		float _far			= 1e4f;
		float _fov			= 60.f;

		Component::Scene::Transform * _transform = nullptr;

		Vec3f _target		 = VEC3F_ZERO;
		bool  _targetIsLocal = false;

		CAMERA_PROJECTION _projection = CAMERA_PROJECTION::PERSPECTIVE;

		Mat4f _viewMatrix		= MAT4F_ID;
		Mat4f _projectionMatrix = MAT4F_ID;

		virtual void _updateViewMatrix();

		void _updateProjectionMatrix();
		void _computePerspectiveProjectionMatrix();
		void _computeOrthographicProjectionMatrix();

	}; // namespace Camera
} // namespace VTX::App::Component::Render
#endif
