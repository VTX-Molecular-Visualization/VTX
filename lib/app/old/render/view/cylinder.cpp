#include "app/old/render/view/cylinder.hpp"

namespace VTX::App::Old::Render::View
{
	Renderer::GL::Program * const Cylinder::_createProgram()
	{
		return VTX_PROGRAM_MANAGER().createProgram( "Cylinder",
													{ FilePath( "cylinder/cylinder.vert" ),
													  FilePath( "cylinder/cylinder.geom" ),
													  FilePath( "cylinder/cylinder.frag" ) } );
	}

	void Cylinder::_init() {}

	void Cylinder::render( const App::Old::Component::Render::Camera & p_camera ) const
	{
		BaseView3D::render( p_camera );

		for ( const std::pair<const App::Old::Application::Representation::InstantiatedRepresentation * const,
							  App::Old::Application::Representation::RepresentationTarget> & representationData :
			  _model->getMolecule()->getRepresentationData() )
		{
			if ( representationData.first->hasToDrawCylinder() )
			{
				const App::Old::Application::Representation::Primitive::Cylinder & cylinderData
					= representationData.first->getCylinderData();

				/// TODO: put a mask
				_program->setFloat( "u_cylRad", cylinderData.radius );
				_program->setUInt( "u_colorBlendingMode", uint( cylinderData.colorBlendingMode ) );
				_program->setBool( "u_isPerspective", p_camera.isPerspective() );

				const App::Old::Application::Representation::TargetRange<void *> & target
					= representationData.second.getBonds();
				/// TODO: use glDrawRangeElements?
				if ( target.indices.size() > 0 )
				{
					_model->getBuffer()->getVao().multiDrawElement( Renderer::GL::VertexArray::DrawMode::LINES,
																	(GLsizei *)( &target.counts[ 0 ] ),
																	Renderer::GL::VertexArray::Type::UNSIGNED_INT,
																	(GLvoid **)( &target.indices[ 0 ] ),
																	GLsizei( target.indices.size() ) );
				}
			}
		}
	}
} // namespace VTX::App::Old::Render::View
