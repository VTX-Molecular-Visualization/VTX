#ifndef __VTX_RENDERER_GL__
#define __VTX_RENDERER_GL__

#include "buffer.hpp"
#include "pass/blur.hpp"
#include "pass/fxaa.hpp"
#include "pass/geometric.hpp"
#include "pass/linearize_depth.hpp"
#include "pass/outline.hpp"
#include "pass/selection.hpp"
#include "pass/shading.hpp"
#include "pass/ssao.hpp"
#include "renderer/base_renderer.hpp"
#include "vertex_array.hpp"

namespace VTX::Renderer::GL
{
	class GL : public BaseRenderer
	{
	  public:
		GL();
		~GL();

		void init( const uint p_width, const uint p_height, const GLuint p_outputFramebufferId ) override;
		void renderFrame( const Object3D::Scene & ) override;
		void setShading() override;
		void activeSSAO( const bool ) override;
		void activeAA( const bool ) override;
		void resize( const uint, const uint, const GLuint ) override;

		inline const Pass::Geometric &		getPassGeometric() const { return *_passGeometric; }
		inline const Pass::LinearizeDepth & getPassLinearizeDepth() const { return *_passLinearizeDepth; }
		inline const Pass::SSAO &			getPassSSAO() const { return *_passSSAO; }
		inline const Pass::Blur &			getPassBlur() const { return *_passBlur; }
		inline const Pass::Shading &		getPassShading() const { return *_passShading; }
		inline const Pass::Outline &		getPassOutline() const { return *_passOutline; }
		inline const Pass::Selection &		getPassSelection() const { return *_passSelection; }
		inline const Pass::FXAA &			getPassFXAA() const { return *_passFXAA; }

		inline const VertexArray & getQuadVAO() const { return _quadVAO; }
		inline const Buffer &	   getQuadVBO() const { return _quadVBO; }

		const uint getPickingId( const uint, const uint ) const override;

	  private:
		Pass::Geometric *	   _passGeometric	   = nullptr;
		Pass::LinearizeDepth * _passLinearizeDepth = nullptr;
		Pass::SSAO *		   _passSSAO		   = nullptr;
		Pass::Blur *		   _passBlur		   = nullptr;
		Pass::Shading *		   _passShading		   = nullptr;
		Pass::Outline *		   _passOutline		   = nullptr;
		Pass::Selection *	   _passSelection	   = nullptr;
		Pass::FXAA *		   _passFXAA		   = nullptr;

		VertexArray _quadVAO;
		Buffer		_quadVBO;

		void _initQuadVAO();
	};
} // namespace VTX::Renderer::GL

#endif
