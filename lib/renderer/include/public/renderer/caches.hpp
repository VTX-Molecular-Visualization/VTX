#ifndef __VTX_RENDERER_CACHES__
#define __VTX_RENDERER_CACHES__

#include "renderer/camera.hpp"
#include "renderer/types.hpp"
#include <util/types.hpp>

namespace VTX::Renderer::Caches
{
	struct Camera
	{
		VTX::Renderer::Camera camera;
		Vec3f				  position;
		Mat4f				  matView;
		Mat4f				  matProj;
	};

	struct System
	{
		Mat4f	   transform;
		ModelIndex modelIndex;
	};
} // namespace VTX::Renderer::Caches

#endif
