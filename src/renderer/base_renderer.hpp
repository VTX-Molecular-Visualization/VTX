#ifndef __VTX_BASE_RENDERER__
#define __VTX_BASE_RENDERER__

#ifdef _MSC_VER
#pragma once
#endif

#include "generic/base_opengl.hpp"
#include "gl/program_manager.hpp"
#include "object3d/scene.hpp"
#include <vector>

namespace VTX
{
	namespace Renderer
	{
		enum class MODE : int
		{
			GL,
#ifdef CUDA_DEFINED
			RT_CPU,

#ifdef OPTIX_DEFINED
			RT_OPTIX,
#endif
#endif
			COUNT
		};

		class BaseRenderer : public Generic::BaseOpenGL
		{
		  public:
			BaseRenderer( OpenGLFunctions * const p_gl ) : BaseOpenGL( p_gl ) { _programManager = new GLSL::ProgramManager( p_gl ); }
			virtual ~BaseRenderer() { delete _programManager; }

			inline const uint getWidth() const { return _width; }
			inline const uint getHeight() const { return _height; }

			virtual void resize( const uint p_width, const uint p_height )
			{
				_width	= p_width;
				_height = p_height;
			}

			virtual void		   init( const uint, const uint )		  = 0;
			virtual void		   renderFrame( const Object3D::Scene & ) = 0;
			virtual void		   setShading()							  = 0;
			virtual const GLuint & getRenderedTexture() const			  = 0;

			// TODO: why ? because SSAO and AA in RT.
			virtual void activeSSAO( const bool ) {}
			virtual void activeOutline( const bool ) {}
			virtual void activeFog( const bool ) {}
			virtual void activeAA( const bool ) {}

			inline GLSL::ProgramManager &		getProgramManager() { return *_programManager; }
			inline const GLSL::ProgramManager & getProgramManager() const { return *_programManager; }

		  protected:
			uint				   _width		   = 0;
			uint				   _height		   = 0;
			GLSL::ProgramManager * _programManager = nullptr;
		};
	} // namespace Renderer
} // namespace VTX

#endif
