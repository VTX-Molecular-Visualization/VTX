#ifndef __VTX_BASE_RENDERER__
#define __VTX_BASE_RENDERER__

#ifdef _MSC_VER
#pragma once
#endif

#include "generic/base_opengl.hpp"
#include "gl/framebuffer.hpp"
#include "object3d/scene.hpp"
#include <vector>

namespace VTX::Renderer
{
	enum class MODE : int
	{
		GL,
		COUNT
	};

	enum class SHADING : int
	{
		DIFFUSE = 0,
		GLOSSY,
		TOON,
		FLAT_COLOR,
		COUNT
	};

	class BaseRenderer : public Generic::BaseOpenGL
	{
	  public:
		BaseRenderer( OpenGLFunctions * const p_gl ) : BaseOpenGL( p_gl ), _outputFramebuffer( p_gl ) {}
		virtual ~BaseRenderer() = default;

		inline const uint			   getWidth() const { return _width; }
		inline const uint			   getHeight() const { return _height; }
		inline const GL::Framebuffer & getOutputFramebuffer() const { return _outputFramebuffer; }

		virtual void resize( const uint p_width, const uint p_height, const GLuint p_outputFramebufferId )
		{
			_outputFramebuffer.assign( p_outputFramebufferId );
			_width	= p_width;
			_height = p_height;
		}

		virtual void init( const uint, const uint, const GLuint ) = 0;
		virtual void renderFrame( const Object3D::Scene & )		  = 0;
		virtual void setShading()								  = 0;

		// TODO: why ? because SSAO and AA in RT.
		virtual void activeSSAO( const bool ) {}
		virtual void activeOutline( const bool ) {}
		virtual void activeFog( const bool ) {}
		virtual void activeAA( const bool ) {}

	  protected:
		uint			_width	= 0;
		uint			_height = 0;
		GL::Framebuffer _outputFramebuffer;
	};
} // namespace VTX::Renderer

#endif
