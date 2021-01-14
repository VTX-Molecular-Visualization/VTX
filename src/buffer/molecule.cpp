#include "molecule.hpp"
#include "vtx_app.hpp"

namespace VTX::Buffer
{
	void Molecule::generate()
	{
		gl()->glCreateBuffers( 1, &_vboAtomPositions );
		gl()->glCreateBuffers( 1, &_vboAtomColors );
		gl()->glCreateBuffers( 1, &_vboAtomRadius );
		gl()->glCreateBuffers( 1, &_vboAtomVisibilities );
		gl()->glCreateBuffers( 1, &_vboAtomSelections );
		gl()->glCreateBuffers( 1, &_iboBonds );

		gl()->glCreateVertexArrays( 1, &_vao );

		gl()->glVertexArrayElementBuffer( _vao, _iboBonds );

		// Position.
		gl()->glEnableVertexArrayAttrib( _vao, ATTRIBUTE_LOCATION::ATOM_POSITION );
		gl()->glVertexArrayVertexBuffer(
			_vao, ATTRIBUTE_LOCATION::ATOM_POSITION, _vboAtomPositions, 0, sizeof( Vec3f ) );
		gl()->glVertexArrayAttribFormat( _vao, ATTRIBUTE_LOCATION::ATOM_POSITION, 3, GL_FLOAT, GL_FALSE, 0 );
		gl()->glVertexArrayAttribBinding( _vao, ATTRIBUTE_LOCATION::ATOM_POSITION, ATTRIBUTE_LOCATION::ATOM_POSITION );

		// Color.
		gl()->glEnableVertexArrayAttrib( _vao, ATTRIBUTE_LOCATION::ATOM_COLOR );
		gl()->glVertexArrayVertexBuffer(
			_vao, ATTRIBUTE_LOCATION::ATOM_COLOR, _vboAtomColors, 0, sizeof( Color::Rgb ) );
		gl()->glVertexArrayAttribFormat( _vao, ATTRIBUTE_LOCATION::ATOM_COLOR, 3, GL_FLOAT, GL_FALSE, 0 );
		gl()->glVertexArrayAttribBinding( _vao, ATTRIBUTE_LOCATION::ATOM_COLOR, ATTRIBUTE_LOCATION::ATOM_COLOR );

		// Radius.
		gl()->glEnableVertexArrayAttrib( _vao, ATTRIBUTE_LOCATION::ATOM_RADIUS );
		gl()->glVertexArrayVertexBuffer( _vao, ATTRIBUTE_LOCATION::ATOM_RADIUS, _vboAtomRadius, 0, sizeof( float ) );
		gl()->glVertexArrayAttribFormat( _vao, ATTRIBUTE_LOCATION::ATOM_RADIUS, 1, GL_FLOAT, GL_FALSE, 0 );
		gl()->glVertexArrayAttribBinding( _vao, ATTRIBUTE_LOCATION::ATOM_RADIUS, ATTRIBUTE_LOCATION::ATOM_RADIUS );

		// Visbility.
		gl()->glEnableVertexArrayAttrib( _vao, ATTRIBUTE_LOCATION::ATOM_VISIBILITY );
		gl()->glVertexArrayVertexBuffer(
			_vao, ATTRIBUTE_LOCATION::ATOM_VISIBILITY, _vboAtomVisibilities, 0, sizeof( ushort ) );
		gl()->glVertexArrayAttribIFormat( _vao, ATTRIBUTE_LOCATION::ATOM_VISIBILITY, 1, GL_UNSIGNED_SHORT, 0 );
		gl()->glVertexArrayAttribBinding(
			_vao, ATTRIBUTE_LOCATION::ATOM_VISIBILITY, ATTRIBUTE_LOCATION::ATOM_VISIBILITY );

		// Selection.
		gl()->glEnableVertexArrayAttrib( _vao, ATTRIBUTE_LOCATION::ATOM_SELECTION );
		gl()->glVertexArrayVertexBuffer(
			_vao, ATTRIBUTE_LOCATION::ATOM_SELECTION, _vboAtomSelections, 0, sizeof( ushort ) );
		gl()->glVertexArrayAttribIFormat( _vao, ATTRIBUTE_LOCATION::ATOM_SELECTION, 1, GL_UNSIGNED_SHORT, 0 );
		gl()->glVertexArrayAttribBinding(
			_vao, ATTRIBUTE_LOCATION::ATOM_SELECTION, ATTRIBUTE_LOCATION::ATOM_SELECTION );
	}

	void Molecule::free()
	{
		if ( _vao != GL_INVALID_VALUE )
		{
			gl()->glDisableVertexArrayAttrib( _vao, ATTRIBUTE_LOCATION::ATOM_POSITION );
			gl()->glDisableVertexArrayAttrib( _vao, ATTRIBUTE_LOCATION::ATOM_COLOR );
			gl()->glDisableVertexArrayAttrib( _vao, ATTRIBUTE_LOCATION::ATOM_RADIUS );
			gl()->glDisableVertexArrayAttrib( _vao, ATTRIBUTE_LOCATION::ATOM_VISIBILITY );
			gl()->glDisableVertexArrayAttrib( _vao, ATTRIBUTE_LOCATION::ATOM_SELECTION );

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

	void Molecule::bind() { gl()->glBindVertexArray( _vao ); }
	void Molecule::unbind() { gl()->glBindVertexArray( 0 ); }

	void Molecule::setAtomPositions( const std::vector<Vec3f> & p_positions )
	{
		gl()->glNamedBufferData(
			_vboAtomPositions, sizeof( Vec3f ) * GLsizei( p_positions.size() ), p_positions.data(), GL_STATIC_DRAW );
	}

	void Molecule::setAtomRadius( const std::vector<float> & p_radius )
	{
		gl()->glNamedBufferData(
			_vboAtomRadius, sizeof( float ) * GLsizei( p_radius.size() ), p_radius.data(), GL_STATIC_DRAW );
	}

	void Molecule::setAtomColors( const std::vector<Color::Rgb> & p_colors )
	{
		gl()->glNamedBufferData(
			_vboAtomColors, sizeof( Color::Rgb ) * GLsizei( p_colors.size() ), p_colors.data(), GL_STATIC_DRAW );
	}

	void Molecule::setAtomVisibilities( const std::vector<ushort> & p_visibilities )
	{
		gl()->glNamedBufferData( _vboAtomVisibilities,
								 sizeof( ushort ) * GLsizei( p_visibilities.size() ),
								 p_visibilities.data(),
								 GL_STATIC_DRAW );
	}

	void Molecule::setAtomSelections( const std::vector<ushort> & p_selections )
	{
		gl()->glNamedBufferData( _vboAtomSelections,
								 sizeof( ushort ) * GLsizei( p_selections.size() ),
								 p_selections.data(),
								 GL_STATIC_DRAW );
	}

	void Molecule::setBonds( const std::vector<uint> & p_bonds )
	{
		gl()->glNamedBufferData(
			_iboBonds, sizeof( uint ) * GLsizei( p_bonds.size() ), p_bonds.data(), GL_STATIC_DRAW );
	}

} // namespace VTX::Buffer
