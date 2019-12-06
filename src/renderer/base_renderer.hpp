#ifndef __VTX_BASE_RENDERER__
#define __VTX_BASE_RENDERER__

#ifdef _MSC_VER
#pragma once
#endif

#include "../object3d/scene.hpp"
#include "../shader/glsl_program_manager.hpp"

namespace VTX
{
	namespace Renderer
	{
		enum MODE
		{
			DEFERRED
		};

		enum SHADING
		{
			LAMBERT = 0,
			BLINN_PHONG,
			TOON
		};

		class BaseRenderer
		{
		  public:
			BaseRenderer()	= default;
			~BaseRenderer() = default;

			virtual void   init( Object3D::Scene * const, const View::BaseView3DMolecule::REPRESENTATION ) = 0;
			virtual void   clear()																		   = 0;
			virtual double render( const Object3D::Scene * const )										   = 0;

		  protected:
			Shader::GLSLProgramManager _programManager = Shader::GLSLProgramManager();
			// std::vector<std::shared_ptr<View::BaseView3DMolecule>> _moleculeViews
			//	= std::vector<std::shared_ptr<View::BaseView3DMolecule>>();
		};
	} // namespace Renderer
} // namespace VTX

#endif
