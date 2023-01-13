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
		_vboAtomIds.create();
		_iboBonds.create();

		_vao.create();

		_vao.bindElementBuffer( _iboBonds );

		// Position.
		_vao.enableAttribute( ATTRIBUTE_LOCATION::ATOM_POSITION );
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::ATOM_POSITION, _vboAtomPositions, sizeof( Vec3f ) );
		_vao.setAttributeFormat( ATTRIBUTE_LOCATION::ATOM_POSITION, 3, Renderer::GL::VertexArray::Type::FLOAT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::ATOM_POSITION, ATTRIBUTE_LOCATION::ATOM_POSITION );

		// Color.
		_vao.enableAttribute( ATTRIBUTE_LOCATION::ATOM_COLOR );
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::ATOM_COLOR, _vboAtomColors, sizeof( Color::Rgba ) );
		_vao.setAttributeFormat( ATTRIBUTE_LOCATION::ATOM_COLOR, 4, Renderer::GL::VertexArray::Type::FLOAT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::ATOM_COLOR, ATTRIBUTE_LOCATION::ATOM_COLOR );

		// Radius.
		_vao.enableAttribute( ATTRIBUTE_LOCATION::ATOM_RADIUS );
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::ATOM_RADIUS, _vboAtomRadii, sizeof( float ) );
		_vao.setAttributeFormat( ATTRIBUTE_LOCATION::ATOM_RADIUS, 1, Renderer::GL::VertexArray::Type::FLOAT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::ATOM_RADIUS, ATTRIBUTE_LOCATION::ATOM_RADIUS );

		// Visibility.
		_vao.enableAttribute( ATTRIBUTE_LOCATION::ATOM_VISIBILITY );
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::ATOM_VISIBILITY, _vboAtomVisibilities, sizeof( uint ) );
		_vao.setAttributeFormat(
			ATTRIBUTE_LOCATION::ATOM_VISIBILITY, 1, Renderer::GL::VertexArray::Type::UNSIGNED_INT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::ATOM_VISIBILITY, ATTRIBUTE_LOCATION::ATOM_VISIBILITY );

		// Selection.
		_vao.enableAttribute( ATTRIBUTE_LOCATION::ATOM_SELECTION );
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::ATOM_SELECTION, _vboAtomSelections, sizeof( uint ) );
		_vao.setAttributeFormat( ATTRIBUTE_LOCATION::ATOM_SELECTION, 1, Renderer::GL::VertexArray::Type::UNSIGNED_INT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::ATOM_SELECTION, ATTRIBUTE_LOCATION::ATOM_SELECTION );

		// Id.
		_vao.enableAttribute( ATTRIBUTE_LOCATION::ATOM_ID );
		_vao.setVertexBuffer( ATTRIBUTE_LOCATION::ATOM_ID, _vboAtomIds, sizeof( Model::ID ) );
		_vao.setAttributeFormat( ATTRIBUTE_LOCATION::ATOM_ID, 1, Renderer::GL::VertexArray::Type::UNSIGNED_INT );
		_vao.setAttributeBinding( ATTRIBUTE_LOCATION::ATOM_ID, ATTRIBUTE_LOCATION::ATOM_ID );
	}

	void Molecule::setAtomPositions( const std::vector<Vec3f> & p_positions, const bool p_isDynamic )
	{
		_updateBuffer(
			_vboAtomPositions,
			p_positions,
			p_isDynamic ? Renderer::GL::Buffer::Usage::DYNAMIC_DRAW : Renderer::GL::Buffer::Usage::STATIC_DRAW );
	}

	void Molecule::setAtomRadius( const std::vector<float> & p_radius ) { _updateBuffer( _vboAtomRadii, p_radius ); }

	void Molecule::setAtomColors( const std::vector<Color::Rgba> & p_colors )
	{
		_updateBuffer( _vboAtomColors, p_colors );
	}

	void Molecule::setAtomVisibilities( const std::vector<uint> & p_visibilities )
	{
		_updateBuffer( _vboAtomVisibilities, p_visibilities );
	}

	void Molecule::setAtomSelections( const std::vector<uint> & p_selections )
	{
		_updateBuffer( _vboAtomSelections, p_selections );
	}

	void Molecule::setAtomIds( const std::vector<Model::ID> & p_ids ) { _updateBuffer( _vboAtomIds, p_ids ); }

	void Molecule::setBonds( const std::vector<uint> & p_bonds ) { _updateBuffer( _iboBonds, p_bonds ); }

} // namespace VTX::Buffer
