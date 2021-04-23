#include "molecule.hpp"
#include "vtx_app.hpp"

namespace VTX::Buffer
{
	void Molecule::_generate()
	{
		gl()->glCreateBuffers( 1, &_vboAtomPositions );
		gl()->glCreateBuffers( 1, &_vboAtomColors );
		gl()->glCreateBuffers( 1, &_vboAtomRadius );
		gl()->glCreateBuffers( 1, &_vboAtomVisibilities );
		gl()->glCreateBuffers( 1, &_vboAtomSelections );
		gl()->glCreateBuffers( 1, &_iboBonds );

		_vao.create();

		_vao.bindElementBuffer( _iboBonds );

		// Position.
		_vao.enableAttribute( ATTRIBUTE_LOCATION::ATOM_POSITION );
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::ATOM_POSITION, _vboAtomPositions, sizeof( Vec3f ) );
		/// TODO: MANDATORY: change namespace hierarchy
		_vao.setAttributeFormat( ATTRIBUTE_LOCATION::ATOM_POSITION, 3, Renderer::GL::VertexArray::Type::FLOAT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::ATOM_POSITION, ATTRIBUTE_LOCATION::ATOM_POSITION );

		// Color.
		_vao.enableAttribute( ATTRIBUTE_LOCATION::ATOM_COLOR );
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::ATOM_COLOR, _vboAtomColors, sizeof( Color::Rgb ) );
		/// TODO: MANDATORY: change namespace hierarchy
		_vao.setAttributeFormat( ATTRIBUTE_LOCATION::ATOM_COLOR, 3, Renderer::GL::VertexArray::Type::FLOAT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::ATOM_COLOR, ATTRIBUTE_LOCATION::ATOM_COLOR );

		// Radius.
		_vao.enableAttribute( ATTRIBUTE_LOCATION::ATOM_RADIUS );
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::ATOM_RADIUS, _vboAtomRadius, sizeof( float ) );
		/// TODO: MANDATORY: change namespace hierarchy
		_vao.setAttributeFormat( ATTRIBUTE_LOCATION::ATOM_RADIUS, 1, Renderer::GL::VertexArray::Type::FLOAT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::ATOM_RADIUS, ATTRIBUTE_LOCATION::ATOM_RADIUS );

		// Visbility.
		_vao.enableAttribute( ATTRIBUTE_LOCATION::ATOM_VISIBILITY );
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::ATOM_VISIBILITY, _vboAtomVisibilities, sizeof( ushort ) );
		/// TODO: MANDATORY: change namespace hierarchy
		_vao.setAttributeFormat(
			ATTRIBUTE_LOCATION::ATOM_VISIBILITY, 1, Renderer::GL::VertexArray::Type::UNSIGNED_SHORT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::ATOM_VISIBILITY, ATTRIBUTE_LOCATION::ATOM_VISIBILITY );

		// Selection.
		_vao.enableAttribute( ATTRIBUTE_LOCATION::ATOM_SELECTION );
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::ATOM_SELECTION, _vboAtomSelections, sizeof( ushort ) );
		/// TODO: MANDATORY: change namespace hierarchy
		_vao.setAttributeFormat(
			ATTRIBUTE_LOCATION::ATOM_SELECTION, 1, Renderer::GL::VertexArray::Type::UNSIGNED_SHORT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::ATOM_SELECTION, ATTRIBUTE_LOCATION::ATOM_SELECTION );
	}

	void Molecule::_free()
	{
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

	void Molecule::bind() { _vao.bind(); }
	void Molecule::unbind() { _vao.unbind(); }

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
