#ifndef __VTX_BASE_RENDERER__
#define __VTX_BASE_RENDERER__

#ifdef _MSC_VER
#pragma once
#endif

#include "glsl/program_manager.hpp"
#include "object3d/scene.hpp"
#include <vector>

namespace VTX
{
	namespace Renderer
	{
		enum class MODE : int
		{
			GL
		};

		class BaseRenderer
		{
		  public:
			BaseRenderer()			= default;
			virtual ~BaseRenderer() = default;

			virtual void BaseRenderer::resize( const uint p_width, const uint p_height )
			{
				_width	= p_width;
				_height = p_height;
			}

			virtual void init( const uint, const uint )			= 0;
			virtual void renderFrame( const Object3D::Scene & ) = 0;
			virtual void setShading()							= 0;

			inline GLSL::ProgramManager &		getProgramManager() { return _programManager; }
			inline const GLSL::ProgramManager & getProgramManager() const { return _programManager; }

		  protected:
			uint				 _width			 = 0;
			uint				 _height		 = 0;
			GLSL::ProgramManager _programManager = GLSL::ProgramManager();
		};
	} // namespace Renderer
} // namespace VTX

#endif
