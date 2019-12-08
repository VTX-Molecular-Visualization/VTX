#ifndef __VTX_BASE_RENDERER__
#define __VTX_BASE_RENDERER__

#ifdef _MSC_VER
#pragma once
#endif

#include "../object3d/scene.hpp"
#include "../shader/glsl_program_manager.hpp"
#include "../view/base_view_3d_molecule.hpp"
#include <vector>

namespace VTX
{
	namespace Renderer
	{
		enum class MODE : int
		{
			DEFERRED
		};

		enum class SHADING : int
		{
			LAMBERT = 0,
			BLINN_PHONG,
			TOON
		};

		class BaseRenderer
		{
		  public:
			using MoleculeView3DSharedPtr		= std::shared_ptr<View::BaseView3DMolecule>;
			using VectorMoleculeView3DSharedPtr = std::vector<MoleculeView3DSharedPtr>;

			BaseRenderer()	= default;
			~BaseRenderer() = default;

			virtual void   init( Object3D::Scene * const, const View::BaseView3DMolecule::REPRESENTATION ) = 0;
			virtual void   clear()																		   = 0;
			virtual double render( const Object3D::Scene * const )										   = 0;

		  protected:
			VectorMoleculeView3DSharedPtr _moleculeViews  = VectorMoleculeView3DSharedPtr();
			Shader::GLSLProgramManager	  _programManager = Shader::GLSLProgramManager();
		};
	} // namespace Renderer
} // namespace VTX

#endif
