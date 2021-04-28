#include "molecule.hpp"
#include "vtx_app.hpp"

namespace VTX::Buffer
{
	void Molecule::_generate()
	{
		_vboAtomPositions.create();
		_vboAtomColors.create();
		_vboAtomRadii.create();
		_vboAtomVisibilities.create();
		_vboAtomSelections.create();
		_iboBonds.create();

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
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::ATOM_RADIUS, _vboAtomRadii, sizeof( float ) );
		/// TODO: MANDATORY: change namespace hierarchy
		_vao.setAttributeFormat( ATTRIBUTE_LOCATION::ATOM_RADIUS, 1, Renderer::GL::VertexArray::Type::FLOAT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::ATOM_RADIUS, ATTRIBUTE_LOCATION::ATOM_RADIUS );

		// Visibility.
		_vao.enableAttribute( ATTRIBUTE_LOCATION::ATOM_VISIBILITY );
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::ATOM_VISIBILITY, _vboAtomVisibilities, sizeof( uint ) );
		/// TODO: MANDATORY: change namespace hierarchy
		_vao.setAttributeFormat(
			ATTRIBUTE_LOCATION::ATOM_VISIBILITY, 1, Renderer::GL::VertexArray::Type::UNSIGNED_INT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::ATOM_VISIBILITY, ATTRIBUTE_LOCATION::ATOM_VISIBILITY );

		// Selection.
		_vao.enableAttribute( ATTRIBUTE_LOCATION::ATOM_SELECTION );
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::ATOM_SELECTION, _vboAtomSelections, sizeof( uint ) );
		/// TODO: MANDATORY: change namespace hierarchy
		_vao.setAttributeFormat(
			ATTRIBUTE_LOCATION::ATOM_SELECTION, 1, Renderer::GL::VertexArray::Type::UNSIGNED_INT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::ATOM_SELECTION, ATTRIBUTE_LOCATION::ATOM_SELECTION );
	}

	void Molecule::bind() { _vao.bind(); }
	void Molecule::unbind() { _vao.unbind(); }

	void Molecule::setAtomPositions( const std::vector<Vec3f> & p_positions )
	{
		_vboAtomPositions.set<Vec3f>( p_positions, Renderer::GL::Buffer::Usage::STATIC_DRAW );
	}

	void Molecule::setAtomRadius( const std::vector<float> & p_radius )
	{
		_vboAtomRadii.set<float>( p_radius, Renderer::GL::Buffer::Usage::STATIC_DRAW );
	}

	void Molecule::setAtomColors( const std::vector<Color::Rgb> & p_colors )
	{
		_vboAtomColors.set<Color::Rgb>( p_colors, Renderer::GL::Buffer::Usage::STATIC_DRAW );
	}

	void Molecule::setAtomVisibilities( const std::vector<uint> & p_visibilities )
	{
		_vboAtomVisibilities.set<ushort>( p_visibilities, Renderer::GL::Buffer::Usage::STATIC_DRAW );
	}

	void Molecule::setAtomSelections( const std::vector<uint> & p_selections )
	{
		_vboAtomSelections.set<ushort>( p_selections, Renderer::GL::Buffer::Usage::STATIC_DRAW );
	}

	void Molecule::setBonds( const std::vector<uint> & p_bonds )
	{
		_iboBonds.set<uint>( p_bonds, Renderer::GL::Buffer::Usage::STATIC_DRAW );
	}

} // namespace VTX::Buffer
