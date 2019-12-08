#include "renderer_deferred.hpp"
#include "../model/model_molecule.hpp"
#include "../view/base_view_3d_molecule.hpp"
#include "base_renderer.hpp"

namespace VTX
{
	namespace Renderer
	{
		void RendererDeferred::init( Object3D::Scene * const p_scene ) { clear(); }

		void RendererDeferred::clear() {}

		double RendererDeferred::render( const Object3D::Scene * const p_scene )
		{
			for ( Model::ModelMolecule * const molecule : p_scene->getMolecules() )
			{
				View::BaseView3DMolecule * view = (View::BaseView3DMolecule *)molecule->getCurrentView3D();
				view->render( 0 );
			}
			return 0.0;
		};

	} // namespace Renderer
} // namespace VTX
