#ifndef __VTX_BASE_RENDERER__
#define __VTX_BASE_RENDERER__

#include "../object3d/scene.hpp"

namespace VTX
{
	namespace Renderer
	{
		enum MODE
		{
			FORWARD = 0,
			DEFERRED
		};

		enum class SHADING
		{
			LAMBERT,
			BLINN_PHONG,
			TOON
		};

		class BaseRenderer
		{
		  public:
			virtual double render( const Object3D::Scene & ) = 0;
		};
	} // namespace Renderer
} // namespace VTX

#endif
