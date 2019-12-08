#ifndef __VTX_BASE_RENDERER__
#define __VTX_BASE_RENDERER__

#ifdef _MSC_VER
#pragma once
#endif

#include "../object3d/scene.hpp"
#include "../shader/glsl_program_manager.hpp"
#include <vector>

namespace VTX
{
	namespace Renderer
	{
		enum class MODE : int
		{
			DEFERRED
		};

		enum class SHADING : int
		{
			LAMBERT = 0,
			BLINN_PHONG,
			TOON
		};

		class BaseRenderer
		{
		  public:
			BaseRenderer()	= default;
			~BaseRenderer() = default;

			virtual void   init( Object3D::Scene * const )		   = 0;
			virtual void   clear()								   = 0;
			virtual double render( const Object3D::Scene * const ) = 0;

		  protected:
			Shader::GLSLProgramManager _programManager = Shader::GLSLProgramManager();
		};
	} // namespace Renderer
} // namespace VTX

#endif
