#include "renderer_deferred.hpp"
#include "../model/model_molecule.hpp"
#include "../view/base_view_3d_molecule.hpp"
#include "base_renderer.hpp"

namespace VTX
{
	namespace Renderer
	{
		void RendererDeferred::init( Object3D::Scene * const p_scene )
		{
			VTX_INFO( "Initializing renderer..." );
			clear( p_scene );

			for ( Model::ModelMolecule * const molecule : p_scene->getMolecules() )
			{
				View::BaseView3DMolecule * view = (View::BaseView3DMolecule *)molecule->getCurrentView3D();

				// Create program.
				Shader::GLSLProgram * program = _programManager.createProgram( view->getProgramName() );
				for ( std::string shader : view->getShaderNames() )
				{
					program->attachShader( _programManager.createShader( shader ) );
				}

				program->link();
				view->setupUniforms( program->getId() );
				program->use();
			}

			_isInitialized = true;
		}

		void RendererDeferred::clear( Object3D::Scene * const p_scene )
		{
			for ( Model::ModelMolecule * const molecule : p_scene->getMolecules() )
			{
				View::BaseView3DMolecule * view = (View::BaseView3DMolecule *)molecule->getCurrentView3D();

				// Delete program.
				_programManager.deleteProgram( view->getProgramName() );
			}
			_isInitialized = false;
		}

		void RendererDeferred::render( const Object3D::Scene * const p_scene, const uint p_time )
		{
			if ( _isInitialized == false ) { return; }

			for ( Model::ModelMolecule * const molecule : p_scene->getMolecules() )
			{
				View::BaseView3DMolecule * view = (View::BaseView3DMolecule *)molecule->getCurrentView3D();
				view->render( 0 );
			}
		};

	} // namespace Renderer
} // namespace VTX
