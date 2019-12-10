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

			void setSize( const int, const int );

			virtual void init( Object3D::Scene &, uint, uint ) = 0;
			virtual void clear( Object3D::Scene & )			   = 0;
			virtual void render( Object3D::Scene & )		   = 0;

		  protected:
			bool					   _isInitialized = false;
			uint					   _width;
			uint					   _height;
			Shader::GLSLProgramManager _programManager = Shader::GLSLProgramManager();
		};
	} // namespace Renderer
} // namespace VTX

#endif
