#include "renderer_deferred.hpp"
#include "base_renderer.hpp"

namespace VTX
{
	namespace Renderer
	{
		// TODO: maybe move some features in the base class.

		void RendererDeferred::start( const Object3D::Scene *						 p_scene,
									  const View::BaseView3DMolecule::REPRESENTATION p_representation )
		{
			stop();
			// Get the views to render by representation.
			_moleculeViews = p_scene->getMoleculeViewsByRepresentation( p_representation );
		}

		void RendererDeferred::stop()
		{
			// Reset each shared_ptr here or not?
			_moleculeViews.clear();
		}

		double RendererDeferred::render( const Object3D::Scene * )
		{
			// Rendering process.

			for ( const std::shared_ptr<View::BaseView3DMolecule> view : _moleculeViews )
			{
				view->render( 0.0 );
			}

			return 0.0;
		};

	} // namespace Renderer
} // namespace VTX
