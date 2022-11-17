#ifndef __VTX_RENDERER_GL__
#define __VTX_RENDERER_GL__

#include "define.hpp"
#include "pass/blur.hpp"
#include "pass/fxaa.hpp"
#include "pass/geometric.hpp"
#include "pass/linearize_depth.hpp"
#include "pass/outline.hpp"
#include "pass/selection.hpp"
#include "pass/shading.hpp"
#include "pass/ssao.hpp"
#include "wrapper/buffer_data.hpp"
#include "wrapper/framebuffer.hpp"
#include "wrapper/vertex_array.hpp"
#include <GL/gl3w.h>

constexpr unsigned int OPENGL_MAJOR_VERSION = 4;
constexpr unsigned int OPENGL_MINOR_VERSION = 5;

namespace VTX::Renderer::GL
{
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

	class GL
	{
	  public:
		GL();
		~GL();

		inline const size_t					getWidth() const { return _width; }
		inline const size_t					getHeight() const { return _height; }
		inline const Wrapper::Framebuffer & getOutputFramebuffer() const { return _outputFramebuffer; }

		void init( const size_t p_width, const size_t p_height, const GLuint p_outputFramebufferId );
		void renderFrame( const Object3D::Scene & );
		void updateRenderSetting( const RENDER_SETTING );
		void resize( const size_t p_width, const size_t p_height, const GLuint );

		inline void enableDepthClamp() const { glEnable( GL_DEPTH_CLAMP ); }
		inline void disableDepthClamp() const { glDisable( GL_DEPTH_CLAMP ); }
		inline void enableDepthTest() const { glEnable( GL_DEPTH_TEST ); }
		inline void disableDepthTest() const { glDisable( GL_DEPTH_TEST ); }

		inline const Pass::Geometric &		getPassGeometric() const { return *_passGeometric; }
		inline const Pass::LinearizeDepth & getPassLinearizeDepth() const { return *_passLinearizeDepth; }
		inline const Pass::SSAO &			getPassSSAO() const { return *_passSSAO; }
		inline const Pass::Blur &			getPassBlur() const { return *_passBlur; }
		inline const Pass::Shading &		getPassShading() const { return *_passShading; }
		inline const Pass::Outline &		getPassOutline() const { return *_passOutline; }
		inline const Pass::Selection &		getPassSelection() const { return *_passSelection; }
		inline const Pass::FXAA &			getPassFXAA() const { return *_passFXAA; }

		inline const Wrapper::VertexArray & getQuadVAO() const { return _quadVAO; }
		inline const Wrapper::BufferData &	getQuadVBO() const { return _quadVBO; }

		const Vec2i getPickedIds( const size_t p_x, const size_t p_y ) const;

	  private:
		size_t _width  = 0;
		size_t _height = 0;

		Pass::Geometric *	   _passGeometric	   = nullptr;
		Pass::LinearizeDepth * _passLinearizeDepth = nullptr;
		Pass::SSAO *		   _passSSAO		   = nullptr;
		Pass::Blur *		   _passBlur		   = nullptr;
		Pass::Shading *		   _passShading		   = nullptr;
		Pass::Outline *		   _passOutline		   = nullptr;
		Pass::Selection *	   _passSelection	   = nullptr;
		Pass::FXAA *		   _passFXAA		   = nullptr;

		Wrapper::VertexArray _quadVAO;
		Wrapper::BufferData	 _quadVBO;

		Wrapper::Framebuffer _outputFramebuffer = Wrapper::Framebuffer();

		void _initQuadVAO();
	};
} // namespace VTX::Renderer::GL

#endif
