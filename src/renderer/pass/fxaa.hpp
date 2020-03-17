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
				virtual void clear() override;
				virtual void render( const Object3D::Scene &, const Renderer::GL & ) override;

			  private:
				GLSL::Program * _aaShader = nullptr;
			};
		} // namespace Pass

	} // namespace Renderer
} // namespace VTX

#endif
