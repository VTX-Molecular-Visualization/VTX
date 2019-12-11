#include "view_3d_molecule_cylinder.hpp"

namespace VTX
{
	namespace View
	{
		void View3DMoleculeCylinder::_prepare()
		{
			uint bondCount = (uint)_model->getBonds().size();

			glGenBuffers( 1, &_bondsIBO );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _bondsIBO );
			glBufferData(
				GL_ELEMENT_ARRAY_BUFFER, sizeof( uint ) * bondCount, _model->getBonds().data(), GL_STATIC_DRAW );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

			glGenVertexArrays( 1, &_vao );
			glBindVertexArray( _vao );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _bondsIBO );
		}

		void View3DMoleculeCylinder::setupShaders( Shader::GLSLProgramManager & p_programManager )
		{
			Shader::GLSLProgram * program = p_programManager.createProgram( "CylinderGeom" );
			program->attachShader( p_programManager.createShader( "cylinderImpostorGeom.vert" ) );
			program->attachShader( p_programManager.createShader( "cylinderImpostorGeom.geom" ) );
			program->attachShader( p_programManager.createShader( "cylinderImpostorDeferred.frag" ) );
			program->link();

			_uViewModelMatrix = glGetUniformLocation( program->getId(), "uMVMatrix" );
			_uProjMatrix	  = glGetUniformLocation( program->getId(), "uProjMatrix" );
			_uRadius		  = glGetUniformLocation( program->getId(), "uCylRad" );
		}

		void View3DMoleculeCylinder::useShaders( Shader::GLSLProgramManager & p_programManager )
		{
			p_programManager.getProgram( "CylinderGeom" )->use();
			glUniform1f( _uRadius, 0.2f );
		}

		void View3DMoleculeCylinder::draw()
		{
			glDrawElements( GL_LINES, _model->getBondCount(), GL_UNSIGNED_INT, (void *)( 0 * sizeof( uint ) ) );
		}
	} // namespace View
} // namespace VTX
