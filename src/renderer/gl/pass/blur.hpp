#ifndef __VTX_PASS_BLUR__
#define __VTX_PASS_BLUR__

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
			class Blur : public BasePass
			{
			  public:
				virtual ~Blur();
				virtual void init( GLSL::ProgramManager &, const uint, const uint ) override;
				virtual void resize( const uint, const uint ) override;
				virtual void render( const Object3D::Scene &, const Renderer::GL & ) override;

				inline const GLuint & getTexture() const { return _texture; }

				void clearTexture();

			  private:
				GLSL::Program * _program				 = nullptr;
				GLuint			_fboFirstPass			 = GL_INVALID_VALUE;
				GLuint			_fbo					 = GL_INVALID_VALUE;
				GLuint			_textureFirstPass		 = GL_INVALID_VALUE;
				GLuint			_texture				 = GL_INVALID_VALUE;
				GLint			_uBlurSizeLoc			 = GL_INVALID_INDEX;
				GLint			_uInvDirectionTexSizeLoc = GL_INVALID_INDEX;
			};
		} // namespace Pass

	} // namespace Renderer
} // namespace VTX

#endif
