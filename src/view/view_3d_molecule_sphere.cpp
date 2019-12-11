#include "view_3d_molecule_sphere.hpp"

namespace VTX
{
	namespace View
	{
		View3DMoleculeSphere::~View3DMoleculeSphere()
		{
			glDeleteBuffers( 1, &_atomPositionsVBO );
			glDeleteBuffers( 1, &_atomRadiusVBO );
			glDeleteBuffers( 1, &_atomColorsVBO );
		}

		void View3DMoleculeSphere::_prepare()
		{
			uint atomCount	 = (uint)_model->getAtomPositions().size();
			uint colorCount	 = (uint)_model->getAtomColors().size();
			uint radiusCount = (uint)_model->getAtomRadius().size();

			glGenBuffers( 1, &_atomPositionsVBO );
			glBindBuffer( GL_ARRAY_BUFFER, _atomPositionsVBO );
			glBufferData(
				GL_ARRAY_BUFFER, sizeof( Vec3f ) * atomCount, _model->getAtomPositions().data(), GL_STATIC_DRAW );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );

			glGenBuffers( 1, &_atomColorsVBO );
			glBindBuffer( GL_ARRAY_BUFFER, _atomColorsVBO );
			glBufferData(
				GL_ARRAY_BUFFER, sizeof( Vec3f ) * colorCount, _model->getAtomColors().data(), GL_STATIC_DRAW );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );

			glGenBuffers( 1, &_atomRadiusVBO );
			glBindBuffer( GL_ARRAY_BUFFER, _atomRadiusVBO );
			glBufferData(
				GL_ARRAY_BUFFER, sizeof( float ) * radiusCount, _model->getAtomRadius().data(), GL_STATIC_DRAW );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );

			glGenVertexArrays( 1, &_vao );
			glBindVertexArray( _vao );

			glBindBuffer( GL_ARRAY_BUFFER, _atomPositionsVBO );
			glEnableVertexAttribArray( ATTRIBUTE_LOCATION::ATOM_POSITION );
			glVertexAttribPointer( ATTRIBUTE_LOCATION::ATOM_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof( Vec3f ), 0 );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );

			glBindBuffer( GL_ARRAY_BUFFER, _atomColorsVBO );
			glEnableVertexAttribArray( ATTRIBUTE_LOCATION::ATOM_COLOR );
			glVertexAttribPointer( ATTRIBUTE_LOCATION::ATOM_COLOR, 3, GL_FLOAT, GL_FALSE, sizeof( Vec3f ), 0 );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );

			glBindBuffer( GL_ARRAY_BUFFER, _atomRadiusVBO );
			glEnableVertexAttribArray( ATTRIBUTE_LOCATION::ATOM_RADIUS );
			glVertexAttribPointer( ATTRIBUTE_LOCATION::ATOM_RADIUS, 1, GL_FLOAT, GL_FALSE, sizeof( float ), 0 );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );

			glBindVertexArray( 0 );
		}

		void View3DMoleculeSphere::setupShaders( Shader::GLSLProgramManager & p_programManager )
		{
			Shader::GLSLProgram * program = p_programManager.createProgram( "SphereImpostorGeomShader" );
			program->attachShader( p_programManager.createShader( "sphereImpostorGeom.vert" ) );
			program->attachShader( p_programManager.createShader( "sphereImpostorGeomQuad.geom" ) );
			program->attachShader( p_programManager.createShader( "sphereImpostorDeferred.frag" ) );
			program->link();

			_uViewModelMatrix = glGetUniformLocation( program->getId(), "uMVMatrix" );
			_uProjMatrix	  = glGetUniformLocation( program->getId(), "uProjMatrix" );
			_uRadiusScale	  = glGetUniformLocation( program->getId(), "uRadScale" );

			glUniform1f( _uRadiusScale, _radiusScale );
		}

		void View3DMoleculeSphere::useShaders( Shader::GLSLProgramManager & p_programManager )
		{
			p_programManager.getProgram( "SphereImpostorGeomShader" )->use();
		}

		void View3DMoleculeSphere::draw() { glDrawArrays( GL_POINTS, 0, _model->getAtomCount() ); }
	} // namespace View
} // namespace VTX
