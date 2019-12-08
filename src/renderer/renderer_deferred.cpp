#include "renderer_deferred.hpp"
#include "base_renderer.hpp"

namespace VTX
{
	namespace Renderer
	{
		// TODO: maybe move some features in the base class.

		void RendererDeferred::init( Object3D::Scene * const						p_scene,
									 const View::BaseView3DMolecule::REPRESENTATION p_representation )
		{
			clear();
			// Get the views to render by representation.
			_moleculeViews = p_scene->getMoleculeViewsByRepresentation( p_representation );
		}

		void RendererDeferred::clear()
		{
			// Reset each shared_ptr here or not?
			_moleculeViews.clear();
		}

		double RendererDeferred::render( const Object3D::Scene * const )
		{
			for ( MoleculeView3DSharedPtr molecule : _moleculeViews )
			{
				molecule->render( 0.0 );
			}
			return 0.0;
		};

	} // namespace Renderer
} // namespace VTX
