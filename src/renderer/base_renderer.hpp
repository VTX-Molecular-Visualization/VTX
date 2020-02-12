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

			void BaseRenderer::setSize( const int p_width, const int p_height )
			{
				_width	= p_width;
				_height = p_height;
			}

			virtual void init( Object3D::Scene &, uint, uint ) = 0;
			virtual void clear( Object3D::Scene & )			   = 0;
			virtual void render( Object3D::Scene & )		   = 0;
			virtual void setShading()						   = 0;

			inline GLSL::ProgramManager & getProgramManager() { return _programManager; }

		  protected:
			bool			   _isInitialized  = false;
			uint			   _width		   = 0;
			uint			   _height		   = 0;
			GLSL::ProgramManager _programManager = GLSL::ProgramManager();
		};
	} // namespace Renderer
} // namespace VTX

#endif
