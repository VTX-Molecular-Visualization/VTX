#ifndef __VTX_BASE_PASS__
#define __VTX_BASE_PASS__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include "object3d/scene.hpp"
#include "renderer/gl/program_manager.hpp"
#include <QOpenGLFunctions_4_5_Compatibility>

namespace VTX
{
	namespace Renderer
	{
		class GL;

		namespace Pass
		{
			class BasePass : public QOpenGLFunctions_4_5_Compatibility
			{
			  public:
				BasePass() = default;
				virtual ~BasePass() {};

				virtual void init( GLSL::ProgramManager &, const uint, const uint )	 = 0;
				virtual void resize( const uint, const uint )						 = 0;
				virtual void render( const Object3D::Scene &, const Renderer::GL & ) = 0;
			};
		} // namespace Pass
	}	  // namespace Renderer
} // namespace VTX

#endif
