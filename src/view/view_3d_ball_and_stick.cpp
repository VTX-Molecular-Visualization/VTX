#include "view_3d_ball_and_stick.hpp"

namespace VTX
{
	namespace View
	{
		void View3DBallAndStick::setupShaders( Shader::GLSLProgramManager & p_programManager )
		{
			VTX_INFO( "Initializing shaders" );

			// Balls.
			Shader::GLSLProgram * program = p_programManager.createProgram( "SphereImpostorGeomShader" );
			program->attachShader( p_programManager.createShader( "sphereImpostorGeom.vert" ) );
			program->attachShader( p_programManager.createShader( "sphereImpostorGeomQuad.geom" ) );
			program->attachShader( p_programManager.createShader( "sphereImpostorDeferred.frag" ) );
			program->link();

			_uViewMatrix	  = glGetUniformLocation( program->getId(), "uMVMatrix" );
			_uProjMatrix	  = glGetUniformLocation( program->getId(), "uProjMatrix" );
			_uBallRadiusScale = glGetUniformLocation( program->getId(), "uRadScale" );

			// Sticks.
			program = p_programManager.createProgram( "CylinderGeom" );
			program->attachShader( p_programManager.createShader( "cylinderImpostorGeom.vert" ) );
			program->attachShader( p_programManager.createShader( "cylinderImpostorGeom.geom" ) );
			program->attachShader( p_programManager.createShader( "cylinderImpostorDeferred.frag" ) );
			program->link();

			_uViewMatrix	 = glGetUniformLocation( program->getId(), "uMVMatrix" );
			_uProjMatrix	 = glGetUniformLocation( program->getId(), "uProjMatrix" );
			_uCylinderRadius = glGetUniformLocation( program->getId(), "uCylRad" );
		}

		void View3DBallAndStick::useShaders( Shader::GLSLProgramManager & p_programManager )
		{
			p_programManager.getProgram( "SphereImpostorGeomShader" )->use();
			p_programManager.getProgram( "CylinderGeom" )->use();
		}

		void View3DBallAndStick::setCameraMatrices( const Mat4f p_viewMatrix, const Mat4f p_projMatrix ) const
		{
			glUniformMatrix4fv( _uViewMatrix, 1, GL_FALSE, glm::value_ptr( p_viewMatrix ) );
			glUniformMatrix4fv( _uProjMatrix, 1, GL_FALSE, glm::value_ptr( p_projMatrix ) );

			glUniformMatrix4fv( _uViewMatrix2, 1, GL_FALSE, glm::value_ptr( p_viewMatrix ) );
			glUniformMatrix4fv( _uProjMatrix2, 1, GL_FALSE, glm::value_ptr( p_projMatrix ) );

			// Set at each frame or one time is enough?
			glUniform1f( _uBallRadiusScale, 1.0f );
			glUniform1f( _uCylinderRadius, 0.2f );
		}

		void View3DBallAndStick::render( const uint p_time )
		{
			// Draw balls.
			glDrawArrays( GL_POINTS, 0, _model->getAtomCount() );

			// Draw sticks.
			glDrawElements( GL_LINES, _model->getBondCount(), GL_UNSIGNED_INT, (void *)( 0 * sizeof( uint ) ) );
		}

	} // namespace View
} // namespace VTX
