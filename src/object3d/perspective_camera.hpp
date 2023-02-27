#ifndef __VTX_PERSPECTIVE_CAMERA__
#define __VTX_PERSPECTIVE_CAMERA__

#include "camera.hpp"
#include "orthographic_camera.hpp"

namespace VTX
{
	namespace Object3D
	{
		class PerspectiveCamera : public Camera
		{
		  public:
			PerspectiveCamera();

			inline float getBaseDistance() const { return _baseDistance; }
			const bool	 isPerspective() const override { return true; }

			void fromOrthographic( OrthographicCamera * p_orthographicCamera );
			void move( const Vec3f & );
			void moveFront( const float );
			void moveRight( const float );
			void moveUp( const float );

			void setTarget( const Vec3f & p_target ) override;
			void setBaseDistance( const float p_distance ) { _baseDistance = p_distance; }

		  private:
			float _baseDistance = 0.f;

			void _updateViewMatrix() override;
			void _updateProjectionMatrix() override;
		}; // namespace PerspectiveCamera
	}	   // namespace Object3D
} // namespace VTX
#endif
