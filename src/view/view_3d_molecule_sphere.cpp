#include "view_3d_molecule_sphere.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace View
	{
		void View3DMoleculeSphere::initItem()
		{
			GL::GLSLProgramManager & pm		 = VTXApp::get().getProgramManager();
			GL::GLSLProgram *		 program = pm.createProgram( "SphereImpostorGeomShader" );
			program->attachShader( pm.createShader( "sphereImpostorGeom.vert" ) );
			program->attachShader( pm.createShader( "sphereImpostorGeomQuad.geom" ) );
			program->attachShader( pm.createShader( "sphereImpostorDeferred.frag" ) );
			program->link();

			_uViewModelMatrix = glGetUniformLocation( program->getId(), "uMVMatrix" );
			_uProjMatrix	  = glGetUniformLocation( program->getId(), "uProjMatrix" );
			_uRadiusScale	  = glGetUniformLocation( program->getId(), "uRadScale" );
		}

		void View3DMoleculeSphere::notify( Event::EVENT_MODEL p_event )
		{
			BaseView3DMolecule::notify( p_event );

			if ( p_event == Event::EVENT_MODEL::CHANGE_REPRESENTATION )
			{
				switch ( Setting::Rendering::representation )
				{
				case MOLECULE_REPRESENTATION::BALL_AND_STICK:
					_radiusScale = 0.3f;
					_isActive	 = true;
					break;
				case MOLECULE_REPRESENTATION::VAN_DER_WAALS:
					_radiusScale = 1.0f;
					_isActive	 = true;
					break;
				case MOLECULE_REPRESENTATION::STICK:
					//_radiusScale = 0.1f;
					_isActive = false;
					break;
				default: _isActive = false; break;
				}
			}
		};

		void View3DMoleculeSphere::render()
		{
			VTXApp::get().getProgramManager().getProgram( "SphereImpostorGeomShader" )->use();
			_setCameraUniforms( VTXApp::get().getScene().getCamera() );
			glUniform1f( _uRadiusScale, _radiusScale );
			glDrawArrays( GL_POINTS, 0, _getModel().getAtomCount() );
		}
	} // namespace View
} // namespace VTX
