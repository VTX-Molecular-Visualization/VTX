#ifndef __VTX_PASS_GEOMETRIC__
#define __VTX_PASS_GEOMETRIC__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_pass.hpp"

namespace VTX
{
	namespace Renderer
	{
		namespace Pass
		{
			class Geometric : public BasePass
			{
			  public:
				virtual ~Geometric();
				virtual void init( GLSL::ProgramManager &, const uint, const uint ) override;
				virtual void resize( const uint, const uint ) override;
				virtual void render( const Object3D::Scene &, const Renderer::GL & ) override;

				inline const GLuint & getColorNormalCompressedTexture() const { return _colorNormalCompressedTexture; }
				inline const GLuint & getCamSpacePositionsTexture() const { return _camSpacePositionsTexture; }
				inline const GLuint & getDepthTexture() const { return _depthTexture; }

			  private:
				GLuint _fbo							 = GL_INVALID_VALUE;
				GLuint _colorNormalCompressedTexture = GL_INVALID_VALUE;
				GLuint _camSpacePositionsTexture	 = GL_INVALID_VALUE;
				GLuint _depthTexture				 = GL_INVALID_VALUE;
			};
		} // namespace Pass

	} // namespace Renderer
} // namespace VTX

#endif
