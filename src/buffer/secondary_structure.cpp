#include "molecule.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Buffer
	{
		void SecondaryStructure::generate()
		{
			gl()->glGenBuffers( 1, &_vboPositions );
			gl()->glBindBuffer( GL_ARRAY_BUFFER, _vboPositions );
			gl()->glGenBuffers( 1, &_vboDirections );
			gl()->glBindBuffer( GL_ARRAY_BUFFER, 0 );
			gl()->glGenBuffers( 1, &_vboNormals );
			gl()->glBindBuffer( GL_ARRAY_BUFFER, 0 );
			gl()->glGenBuffers( 1, &_vboSecondaryStructures );
			gl()->glBindBuffer( GL_ARRAY_BUFFER, 0 );
			gl()->glGenBuffers( 1, &_vboColors );
			gl()->glBindBuffer( GL_ARRAY_BUFFER, 0 );
			gl()->glGenBuffers( 1, &_ibo );
			gl()->glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

			gl()->glGenVertexArrays( 1, &_vao );
			gl()->glBindVertexArray( _vao );
			gl()->glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ibo );
			gl()->glBindBuffer( GL_ARRAY_BUFFER, 0 );

			gl()->glBindBuffer( GL_ARRAY_BUFFER, _vboPositions );
			gl()->glEnableVertexAttribArray( ATTRIBUTE_LOCATION::CONTROL_POINT_POSITION );
			gl()->glVertexAttribPointer( ATTRIBUTE_LOCATION::CONTROL_POINT_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof( Vec3f ), 0 );
			gl()->glBindBuffer( GL_ARRAY_BUFFER, 0 );

			gl()->glBindBuffer( GL_ARRAY_BUFFER, _vboDirections );
			gl()->glEnableVertexAttribArray( ATTRIBUTE_LOCATION::CONTROL_POINT_DIRECTION );
			gl()->glVertexAttribPointer( ATTRIBUTE_LOCATION::CONTROL_POINT_DIRECTION, 3, GL_FLOAT, GL_FALSE, sizeof( Vec3f ), 0 );
			gl()->glBindBuffer( GL_ARRAY_BUFFER, 0 );

			gl()->glBindBuffer( GL_ARRAY_BUFFER, _vboNormals );
			gl()->glEnableVertexAttribArray( ATTRIBUTE_LOCATION::CONTROL_POINT_NORMAL );
			gl()->glVertexAttribPointer( ATTRIBUTE_LOCATION::CONTROL_POINT_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof( Vec3f ), 0 );
			gl()->glBindBuffer( GL_ARRAY_BUFFER, 0 );

			gl()->glBindBuffer( GL_ARRAY_BUFFER, _vboSecondaryStructures );
			gl()->glEnableVertexAttribArray( ATTRIBUTE_LOCATION::CONTROL_POINT_SECONDARY_STRUCTURE );
			gl()->glVertexAttribPointer( ATTRIBUTE_LOCATION::CONTROL_POINT_SECONDARY_STRUCTURE, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof( uint ), 0 );
			gl()->glBindBuffer( GL_ARRAY_BUFFER, 0 );

			gl()->glBindBuffer( GL_ARRAY_BUFFER, _vboColors );
			gl()->glEnableVertexAttribArray( ATTRIBUTE_LOCATION::CONTROL_POINT_COLOR );
			gl()->glVertexAttribPointer( ATTRIBUTE_LOCATION::CONTROL_POINT_COLOR, 3, GL_FLOAT, GL_FALSE, sizeof( Color::Rgb ), 0 );
			gl()->glBindBuffer( GL_ARRAY_BUFFER, 0 );

			gl()->glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
			gl()->glBindVertexArray( 0 );
		}

		void SecondaryStructure::free()
		{
			if ( _vao != GL_INVALID_VALUE )
			{
				gl()->glBindVertexArray( _vao );
				gl()->glBindBuffer( GL_ARRAY_BUFFER, _vboPositions );
				gl()->glDisableVertexAttribArray( ATTRIBUTE_LOCATION::CONTROL_POINT_POSITION );
				gl()->glBindBuffer( GL_ARRAY_BUFFER, _vboDirections );
				gl()->glDisableVertexAttribArray( ATTRIBUTE_LOCATION::CONTROL_POINT_DIRECTION );
				gl()->glBindBuffer( GL_ARRAY_BUFFER, _vboNormals );
				gl()->glDisableVertexAttribArray( ATTRIBUTE_LOCATION::CONTROL_POINT_NORMAL );
				gl()->glBindBuffer( GL_ARRAY_BUFFER, _vboSecondaryStructures );
				gl()->glDisableVertexAttribArray( ATTRIBUTE_LOCATION::CONTROL_POINT_SECONDARY_STRUCTURE );
				gl()->glBindBuffer( GL_ARRAY_BUFFER, _vboColors );
				gl()->glDisableVertexAttribArray( ATTRIBUTE_LOCATION::CONTROL_POINT_COLOR );
				gl()->glBindBuffer( GL_ARRAY_BUFFER, 0 );
				gl()->glBindVertexArray( 0 );

				gl()->glDeleteVertexArrays( 1, &_vao );
			}

			if ( _vboPositions != GL_INVALID_VALUE )
			{
				gl()->glDeleteBuffers( 1, &_vboPositions );
			}
			if ( _vboDirections != GL_INVALID_VALUE )
			{
				gl()->glDeleteBuffers( 1, &_vboDirections );
			}
			if ( _vboNormals != GL_INVALID_VALUE )
			{
				gl()->glDeleteBuffers( 1, &_vboNormals );
			}
			if ( _vboSecondaryStructures != GL_INVALID_VALUE )
			{
				gl()->glDeleteBuffers( 1, &_vboSecondaryStructures );
			}
			if ( _vboColors != GL_INVALID_VALUE )
			{
				gl()->glDeleteBuffers( 1, &_vboColors );
			}
			if ( _ibo != GL_INVALID_VALUE )
			{
				gl()->glDeleteBuffers( 1, &_ibo );
			}
		}

		void SecondaryStructure::bind()
		{
			gl()->glBindVertexArray( _vao );
			gl()->glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _ibo );
		}
		void SecondaryStructure::unbind()
		{
			gl()->glBindVertexArray( 0 );
			gl()->glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		}

		void SecondaryStructure::setControlPointPositions( const std::vector<Vec3f> & p_positions )
		{
			gl()->glNamedBufferData( _vboPositions, GLsizei( p_positions.size() ) * sizeof( Vec3f ), p_positions.data(), GL_STATIC_DRAW );
		}

		void SecondaryStructure::setControlPointDirections( const std::vector<Vec3f> & p_directions )
		{
			gl()->glNamedBufferData( _vboDirections, GLsizei( p_directions.size() ) * sizeof( Vec3f ), p_directions.data(), GL_STATIC_DRAW );
		}

		void SecondaryStructure::setControlPointNormals( const std::vector<Vec3f> & p_normals )
		{
			gl()->glNamedBufferData( _vboNormals, GLsizei( p_normals.size() ) * sizeof( Vec3f ), p_normals.data(), GL_STATIC_DRAW );
		}

		void SecondaryStructure::setControlPointSecondaryStructure( const std::vector<uint> & p_ss )
		{
			gl()->glNamedBufferData( _vboSecondaryStructures, GLsizei( p_ss.size() ) * sizeof( uint ), p_ss.data(), GL_STATIC_DRAW );
		}

		void SecondaryStructure::setControlPointColors( const std::vector<Color::Rgb> & p_colors )
		{
			gl()->glNamedBufferData( _vboColors, GLsizei( p_colors.size() ) * sizeof( Color::Rgb ), p_colors.data(), GL_STATIC_DRAW );
		}

		void SecondaryStructure::setIndices( const std::vector<uint> & p_indices )
		{
			gl()->glNamedBufferData( _ibo, GLsizei( p_indices.size() ) * sizeof( uint ), p_indices.data(), GL_STATIC_DRAW );
		}

	} // namespace Buffer
} // namespace VTX
