#ifndef __VTX_PASS_FXAA__
#define __VTX_PASS_FXAA__

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
			class FXAA : public BasePass
			{
			  public:
				virtual void init( GLSL::ProgramManager &, const uint, const uint ) override;
				virtual void resize( const uint, const uint ) override;
				virtual void clean() override;
				virtual void render( const Object3D::Scene &, const Renderer::GL & ) override;

				inline const GLuint & getTexture() const { return _texture; }
				inline const GLuint & getFbo() const { return _fbo; }

			  private:
				GLSL::Program * _program = nullptr;
				GLuint			_fbo	 = GL_INVALID_VALUE;
				GLuint			_texture = GL_INVALID_VALUE;
			};
		} // namespace Pass

	} // namespace Renderer
} // namespace VTX

#endif
