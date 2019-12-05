#ifndef __VTX_BASE_RENDERER__
#define __VTX_BASE_RENDERER__

#include "../object3d/scene.hpp"
#include "../shader/glsl_program_manager.hpp"
#include "../view/base_view_3d_molecule.hpp"

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

			virtual void   start( const Object3D::Scene *, const View::BaseView3DMolecule::REPRESENTATION ) = 0;
			virtual void   stop()																			= 0;
			virtual double render( const Object3D::Scene * )												= 0;

		  protected:
			Shader::GLSLProgramManager							   _programManager = Shader::GLSLProgramManager();
			std::vector<std::shared_ptr<View::BaseView3DMolecule>> _moleculeViews
				= std::vector<std::shared_ptr<View::BaseView3DMolecule>>();
		};
	} // namespace Renderer
} // namespace VTX

#endif
