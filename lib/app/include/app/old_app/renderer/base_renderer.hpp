#ifndef __VTX_BASE_RENDERER__
#define __VTX_BASE_RENDERER__

#include "app/old_app/generic/base_opengl.hpp"
#include "gl/framebuffer.hpp"
#include "app/old_app/model/base_model.hpp"
#include "app/old_app/object3d/scene.hpp"
#include <vector>

namespace VTX::Renderer
{
	enum class MODE : int
	{
		GL,
		RT_CPU,
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

	enum class RENDER_SETTING : int
	{
		SHADING = 0,
		SSAO,
		OUTLINE,
		FOG,
		AA
	};

	class BaseRenderer : public Generic::BaseOpenGL
	{
	  public:
		BaseRenderer()			= default;
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

		virtual void updateRenderSetting( const RENDER_SETTING ) {}

		virtual const Vec2i getPickedIds( const uint, const uint ) const
		{
			return Vec2i( Model::ID_UNKNOWN, Model::ID_UNKNOWN );
		}

	  protected:
		uint			_width			   = 0;
		uint			_height			   = 0;
		GL::Framebuffer _outputFramebuffer = GL::Framebuffer();
	};
} // namespace VTX::Renderer

#endif
