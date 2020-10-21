#ifndef __VTX_PASS_LINEARIZE_DEPTH__
#define __VTX_PASS_LINEARIZE_DEPTH__

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
			class LinearizeDepth : public BasePass
			{
			  public:
				LinearizeDepth( OpenGLFunctions * const p_gl ) : BasePass( p_gl ) {}
				virtual ~LinearizeDepth();
				virtual void init( GLSL::ProgramManager &, const uint, const uint ) override;
				virtual void resize( const uint, const uint ) override;
				virtual void render( const Object3D::Scene &, const Renderer::GL & ) override;

				inline const GLuint & getTexture() const { return _texture; }

			  private:
				GLSL::Program * _program	  = nullptr;
				GLuint			_fbo		  = GL_INVALID_VALUE;
				GLuint			_texture	  = GL_INVALID_VALUE;
				GLint			_uClipInfoLoc = GL_INVALID_INDEX;
			};
		} // namespace Pass

	} // namespace Renderer
} // namespace VTX

#endif
