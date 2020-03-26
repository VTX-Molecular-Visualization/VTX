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
			LAMBERT = 0,
			BLINN_PHONG,
			TOON
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

				inline const GLuint & getShadingTexture() const { return _shadingTexture; }

			  private:
				GLSL::Program * _diffuseShading	   = nullptr;
				GLSL::Program * _blinnPhongShading = nullptr;
				GLSL::Program * _toonShading	   = nullptr;
				GLuint			_fboShading		   = GL_INVALID_VALUE;
				GLuint			_shadingTexture	   = GL_INVALID_VALUE;

				GLSL::Program * _currentShading = nullptr;
			};
		} // namespace Pass

	} // namespace Renderer
} // namespace VTX

#endif
