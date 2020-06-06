#ifndef __VTX_PASS_SHADING__
#define __VTX_PASS_SHADING__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_pass.hpp"

namespace VTX
{
	namespace Renderer
	{
		enum class SHADING : int
		{
			DIFFUSE = 0,
			GLOSSY,
			TOON,
			FLAT_COLOR
		};

		namespace Pass
		{
			class Shading : public BasePass
			{
			  public:
				virtual void init( GLSL::ProgramManager &, const uint, const uint ) override;
				virtual void clean() override;
				virtual void render( const Object3D::Scene &, const Renderer::GL & ) override;
				void		 set();

				inline const GLuint & getTexture() const { return _texture; }
				inline const GLuint & getFbo() const { return _fbo; }

			  private:
				GLSL::Program * _diffuseShading = nullptr;
				GLSL::Program * _glossyShading	= nullptr;
				GLSL::Program * _toonShading	= nullptr;
				GLSL::Program * _flatShading	= nullptr;
				GLuint			_fbo			= GL_INVALID_VALUE;
				GLuint			_texture		= GL_INVALID_VALUE;

				GLSL::Program * _currentShading = nullptr;
			};
		} // namespace Pass

	} // namespace Renderer
} // namespace VTX

#endif
