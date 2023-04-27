#ifndef __VTX_RENDERER_GL_GL__
#define __VTX_RENDERER_GL_GL__

#include "buffer.hpp"
#include "pass/blur.hpp"
#include "pass/fxaa.hpp"
#include "pass/geometric.hpp"
#include "pass/linearize_depth.hpp"
#include "pass/outline.hpp"
#include "pass/selection.hpp"
#include "pass/shading.hpp"
#include "pass/ssao.hpp"
#include "vertex_array.hpp"
#include <util/types.hpp>

namespace VTX::Renderer::GL
{
	class GL
	{
	  public:
		GL();
		~GL() = default;

		/*
		inline const Pass::Geometric &		getPassGeometric() const { return _passGeometric; }
		inline const Pass::LinearizeDepth & getPassLinearizeDepth() const { return _passLinearizeDepth; }
		inline const Pass::SSAO &			getPassSSAO() const { return _passSSAO; }
		inline const Pass::Blur &			getPassBlur() const { return _passBlur; }
		inline const Pass::Shading &		getPassShading() const { return _passShading; }
		inline const Pass::Outline &		getPassOutline() const { return _passOutline; }
		inline const Pass::Selection &		getPassSelection() const { return _passSelection; }
		inline const Pass::FXAA &			getPassFXAA() const { return _passFXAA; }
		*/

		inline void setOutput( const GLuint p_output ) {}

		/*
		inline void enableDepthClamp() const { glEnable( GL_DEPTH_CLAMP ); }
		inline void disableDepthClamp() const { glDisable( GL_DEPTH_CLAMP ); }
		inline void enableDepthTest() const { glEnable( GL_DEPTH_TEST ); }
		inline void disableDepthTest() const { glDisable( GL_DEPTH_TEST ); }
		inline void memoryBarrier( const GLbitfield p_barrier ) const { glMemoryBarrier( p_barrier ); }
		inline void flush() const { glFlush(); }
		inline void finish() const { glFinish(); }
		*/
		/*
		inline const VertexArray & getQuadVAO() const { return _quadVAO; }
		inline const Buffer &	   getQuadVBO() const { return _quadVBO; }
		*/

		void init( const size_t p_width, const size_t p_height );
		void resize( const size_t p_width, const size_t p_height );

		void renderFrame();
		// void updateRenderSetting( const RENDER_SETTING );

		// const Vec2i getPickedIds( const uint p_x, const uint p_y ) const;

	  private:
		size_t _width  = 0;
		size_t _height = 0;

		VertexArray _quadVAO = VertexArray();
		Buffer		_quadVBO = Buffer();

		Framebuffer _output = Framebuffer();

		std::vector<std::unique_ptr<Pass::BasePass>> _passes = std::vector<std::unique_ptr<Pass::BasePass>>();

		/*
		Pass::Geometric		 _passGeometric		 = Pass::Geometric();
		Pass::LinearizeDepth _passLinearizeDepth = Pass::LinearizeDepth();
		Pass::SSAO			 _passSSAO			 = Pass::SSAO();
		Pass::Blur			 _passBlur			 = Pass::Blur();
		Pass::Shading		 _passShading		 = Pass::Shading();
		Pass::Outline		 _passOutline		 = Pass::Outline();
		Pass::Selection		 _passSelection		 = Pass::Selection();
		Pass::FXAA			 _passFXAA			 = Pass::FXAA();
		*/
	};
} // namespace VTX::Renderer::GL

#endif
