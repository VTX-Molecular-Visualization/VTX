#ifndef __VTX_RENDERER_PROXY_CAMERA__
#define __VTX_RENDERER_PROXY_CAMERA__

#include <util/callback.hpp>
#include <util/types.hpp>

namespace VTX::Renderer::Proxy
{

	struct Camera
	{
		const Mat4f * const matrixView;
		const Mat4f * const matrixProjection;

		const Vec3f & cameraPosition;
		const Vec2i & mousePosition;
		const float	  cameraNear;
		const float	  cameraFar;
		const bool	  isPerspective;

		Util::Callback<>			 onMatrixView;
		Util::Callback<>			 onMatrixProjection;
		Util::Callback<Vec3f>		 onCameraPosition;
		Util::Callback<Vec2i>		 onMousePosition;
		Util::Callback<float, float> onCameraNearFar;
		Util::Callback<bool>		 onPerspective;
	};

} // namespace VTX::Renderer::Proxy

#endif
