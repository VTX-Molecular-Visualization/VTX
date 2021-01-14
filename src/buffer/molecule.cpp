#include "molecule.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Buffer
	{
		void Molecule::_generate()
		{
			gl()->glGenBuffers( 1, &_vboAtomPositions );
			gl()->glBindBuffer( GL_ARRAY_BUFFER, 0 );
			gl()->glGenBuffers( 1, &_vboAtomColors );
			gl()->glBindBuffer( GL_ARRAY_BUFFER, 0 );
			gl()->glGenBuffers( 1, &_vboAtomRadius );
			gl()->glBindBuffer( GL_ARRAY_BUFFER, 0 );
			gl()->glGenBuffers( 1, &_vboAtomVisibilities );
			gl()->glBindBuffer( GL_ARRAY_BUFFER, 0 );
			gl()->glGenBuffers( 1, &_vboAtomSelections );
			gl()->glBindBuffer( GL_ARRAY_BUFFER, 0 );
			gl()->glGenBuffers( 1, &_iboBonds );
			gl()->glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

			gl()->glGenVertexArrays( 1, &_vao );
			gl()->glBindVertexArray( _vao );

			gl()->glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _iboBonds );

			gl()->glBindBuffer( GL_ARRAY_BUFFER, _vboAtomPositions );
			gl()->glEnableVertexAttribArray( ATTRIBUTE_LOCATION::ATOM_POSITION );
			gl()->glVertexAttribPointer( ATTRIBUTE_LOCATION::ATOM_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof( Vec3f ), 0 );
			gl()->glBindBuffer( GL_ARRAY_BUFFER, 0 );

			gl()->glBindBuffer( GL_ARRAY_BUFFER, _vboAtomColors );
			gl()->glEnableVertexAttribArray( ATTRIBUTE_LOCATION::ATOM_COLOR );
			gl()->glVertexAttribPointer( ATTRIBUTE_LOCATION::ATOM_COLOR, 3, GL_FLOAT, GL_FALSE, sizeof( Color::Rgb ), 0 );
			gl()->glBindBuffer( GL_ARRAY_BUFFER, 0 );

			gl()->glBindBuffer( GL_ARRAY_BUFFER, _vboAtomRadius );
			gl()->glEnableVertexAttribArray( ATTRIBUTE_LOCATION::ATOM_RADIUS );
			gl()->glVertexAttribPointer( ATTRIBUTE_LOCATION::ATOM_RADIUS, 1, GL_FLOAT, GL_FALSE, sizeof( float ), 0 );
			gl()->glBindBuffer( GL_ARRAY_BUFFER, 0 );

			gl()->glBindBuffer( GL_ARRAY_BUFFER, _vboAtomVisibilities );
			gl()->glEnableVertexAttribArray( ATTRIBUTE_LOCATION::ATOM_VISIBILITY );
			gl()->glVertexAttribIPointer( ATTRIBUTE_LOCATION::ATOM_VISIBILITY, 1, GL_UNSIGNED_SHORT, sizeof( ushort ), 0 );
			gl()->glBindBuffer( GL_ARRAY_BUFFER, 0 );

			gl()->glBindBuffer( GL_ARRAY_BUFFER, _vboAtomSelections );
			gl()->glEnableVertexAttribArray( ATTRIBUTE_LOCATION::ATOM_SELECTION );
			gl()->glVertexAttribIPointer( ATTRIBUTE_LOCATION::ATOM_SELECTION, 1, GL_UNSIGNED_SHORT, sizeof( ushort ), 0 );
			gl()->glBindBuffer( GL_ARRAY_BUFFER, 0 );

			gl()->glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
			gl()->glBindVertexArray( 0 );
		}

		void Molecule::_free()
		{
			if ( _vao != GL_INVALID_VALUE )
			{
				gl()->glBindVertexArray( _vao );
				gl()->glBindBuffer( GL_ARRAY_BUFFER, _vboAtomPositions );
				gl()->glDisableVertexAttribArray( ATTRIBUTE_LOCATION::ATOM_POSITION );
				gl()->glBindBuffer( GL_ARRAY_BUFFER, _vboAtomColors );
				gl()->glDisableVertexAttribArray( ATTRIBUTE_LOCATION::ATOM_COLOR );
				gl()->glBindBuffer( GL_ARRAY_BUFFER, _vboAtomRadius );
				gl()->glDisableVertexAttribArray( ATTRIBUTE_LOCATION::ATOM_RADIUS );
				gl()->glBindBuffer( GL_ARRAY_BUFFER, _vboAtomVisibilities );
				gl()->glDisableVertexAttribArray( ATTRIBUTE_LOCATION::ATOM_VISIBILITY );
				gl()->glBindBuffer( GL_ARRAY_BUFFER, _vboAtomSelections );
				gl()->glDisableVertexAttribArray( ATTRIBUTE_LOCATION::ATOM_SELECTION );
				gl()->glBindBuffer( GL_ARRAY_BUFFER, 0 );
				gl()->glBindVertexArray( 0 );

				gl()->glDeleteVertexArrays( 1, &_vao );
			}

			if ( _vboAtomPositions != GL_INVALID_VALUE )
			{
				gl()->glDeleteBuffers( 1, &_vboAtomPositions );
			}
			if ( _vboAtomRadius != GL_INVALID_VALUE )
			{
				gl()->glDeleteBuffers( 1, &_vboAtomRadius );
			}
			if ( _vboAtomColors != GL_INVALID_VALUE )
			{
				gl()->glDeleteBuffers( 1, &_vboAtomColors );
			}
			if ( _vboAtomVisibilities != GL_INVALID_VALUE )
			{
				gl()->glDeleteBuffers( 1, &_vboAtomVisibilities );
			}
			if ( _vboAtomSelections != GL_INVALID_VALUE )
			{
				gl()->glDeleteBuffers( 1, &_vboAtomSelections );
			}
			if ( _iboBonds != GL_INVALID_VALUE )
			{
				gl()->glDeleteBuffers( 1, &_iboBonds );
			}
		}

		void Molecule::bind()
		{
			gl()->glBindVertexArray( _vao );
			gl()->glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _iboBonds );
		}
		void Molecule::unbind()
		{
			gl()->glBindVertexArray( 0 );
			gl()->glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		}

		void Molecule::setAtomPositions( const std::vector<Vec3f> & p_positions )
		{
			gl()->glNamedBufferData( _vboAtomPositions, sizeof( Vec3f ) * GLsizei( p_positions.size() ), p_positions.data(), GL_STATIC_DRAW );
		}

		void Molecule::setAtomRadius( const std::vector<float> & p_radius )
		{
			gl()->glNamedBufferData( _vboAtomRadius, sizeof( float ) * GLsizei( p_radius.size() ), p_radius.data(), GL_STATIC_DRAW );
		}

		void Molecule::setAtomColors( const std::vector<Color::Rgb> & p_colors )
		{
			gl()->glNamedBufferData( _vboAtomColors, sizeof( Color::Rgb ) * GLsizei( p_colors.size() ), p_colors.data(), GL_STATIC_DRAW );
		}

		void Molecule::setAtomVisibilities( const std::vector<ushort> & p_visibilities )
		{
			gl()->glNamedBufferData( _vboAtomVisibilities, sizeof( ushort ) * GLsizei( p_visibilities.size() ), p_visibilities.data(), GL_STATIC_DRAW );
		}

		void Molecule::setAtomSelections( const std::vector<ushort> & p_selections )
		{
			gl()->glNamedBufferData( _vboAtomSelections, sizeof( ushort ) * GLsizei( p_selections.size() ), p_selections.data(), GL_STATIC_DRAW );
		}

		void Molecule::setBonds( const std::vector<uint> & p_bonds )
		{
			gl()->glNamedBufferData( _iboBonds, sizeof( uint ) * GLsizei( p_bonds.size() ), p_bonds.data(), GL_STATIC_DRAW );
		}

	} // namespace Buffer
} // namespace VTX
