#ifndef __VTX_ORTHOGRAPHIC_CAMERA__
#define __VTX_ORTHOGRAPHIC_CAMERA__

#include "camera.hpp"

namespace VTX
{
	namespace Object3D
	{
		class PerspectiveCamera;

		class OrthographicCamera : public Camera
		{
		  public:
			OrthographicCamera();

			float getZoom() const { return _zoom; }
			void  setZoom( const float p_zoom ) { _zoom = Util::Math::max( p_zoom, 0.001f ); }
			void  zoom( const float p_delta ) { _zoom = Util::Math::max( _zoom + p_delta, 0.001f ); }

			void fromPerspective( PerspectiveCamera * p_perspectiveCamera );

			const bool isPerspective() const override { return false; }

		  private:
			float _zoom = 1.f;

			void _updateViewMatrix() override;
			void _updateProjectionMatrix() override;
		}; // namespace OrthographicCamera
	}	   // namespace Object3D
} // namespace VTX
#endif
