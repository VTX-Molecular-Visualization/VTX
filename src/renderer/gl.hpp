#ifndef __VTX_RENDERER_GL__
#define __VTX_RENDERER_GL__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_renderer.hpp"
#include "pass/blur.hpp"
#include "pass/fxaa.hpp"
#include "pass/geometric.hpp"
#include "pass/shading.hpp"
#include "pass/ssao.hpp"

namespace VTX
{
	namespace Renderer
	{
		class GL : public BaseRenderer
		{
		  public:
			GL() = default;
			~GL();

			virtual void init( const uint, const uint ) override;
			virtual void renderFrame( const Object3D::Scene & ) override;
			virtual void setShading() override;
			virtual void resize( const uint, const uint ) override;

			inline const Pass::Geometric & getPassGeometric() const { return *_passGeometric; }
			inline const Pass::SSAO &	   getPassSSAO() const { return *_passSSAO; }
			inline const Pass::Blur &	   getPassBlur() const { return *_passBlur; }
			inline const Pass::Shading &   getPassShading() const { return *_passShading; }
			inline const Pass::FXAA &	   getPassFXAA() const { return *_passFXAA; }

			inline const GLuint & getQuadVAO() const { return _quadVAO; }
			inline const GLuint & getQuadVBO() const { return _quadVBO; }

		  private:
			Pass::Geometric * _passGeometric = new Pass::Geometric();
			Pass::SSAO *	  _passSSAO		 = new Pass::SSAO();
			Pass::Blur *	  _passBlur		 = new Pass::Blur();
			Pass::Shading *	  _passShading	 = new Pass::Shading();
			Pass::FXAA *	  _passFXAA		 = new Pass::FXAA();

			GLuint _quadVAO = GL_INVALID_VALUE;
			GLuint _quadVBO = GL_INVALID_VALUE;

			void _initQuadVAO();
		};
	} // namespace Renderer
} // namespace VTX

#endif
