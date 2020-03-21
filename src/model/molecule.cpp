#include "molecule.hpp"
#include "renderer/ray_tracing/materials/diffuse_material.hpp"
#include "renderer/ray_tracing/materials/flat_color_material.hpp"
#include "renderer/ray_tracing/primitives/cylinder.hpp"
#include "renderer/ray_tracing/primitives/sphere.hpp"
#include "view/d3/box.hpp"
#include "view/d3/cylinder.hpp"
#include "view/d3/sphere.hpp"
#include "view/ui/molecule.hpp"
#include "view/ui/molecule_structure.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Model
	{
		Molecule::~Molecule()
		{
			glBindVertexArray( _vao );
			glBindBuffer( GL_ARRAY_BUFFER, _atomPositionsVBO );
			glDisableVertexAttribArray( ATTRIBUTE_LOCATION::ATOM_POSITION );
			glBindBuffer( GL_ARRAY_BUFFER, _atomColorsVBO );
			glDisableVertexAttribArray( ATTRIBUTE_LOCATION::ATOM_COLOR );
			glBindBuffer( GL_ARRAY_BUFFER, _atomRadiusVBO );
			glDisableVertexAttribArray( ATTRIBUTE_LOCATION::ATOM_RADIUS );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
			glBindVertexArray( 0 );

			if ( _atomPositionsVBO != GL_INVALID_VALUE ) glDeleteBuffers( 1, &_atomPositionsVBO );
			if ( _atomRadiusVBO != GL_INVALID_VALUE ) glDeleteBuffers( 1, &_atomRadiusVBO );
			if ( _atomColorsVBO != GL_INVALID_VALUE ) glDeleteBuffers( 1, &_atomColorsVBO );
			if ( _bondsIBO != GL_INVALID_VALUE ) glDeleteBuffers( 1, &_bondsIBO );

			if ( _vao != GL_INVALID_VALUE ) glDeleteVertexArrays( 1, &_vao );

			Generic::clearVector<Atom>( _atoms );
			Generic::clearVector<Residue>( _residues );
			Generic::clearVector<Chain>( _chains );

			// RT clean
			for ( Renderer::BasePrimitive * prim : _rtPrimitives )
			{
				delete prim;
			}
			_rtPrimitives.clear();
			for ( Renderer::BaseMaterial * mtl : _rtMaterials )
			{
				delete mtl;
			}
			_rtMaterials.clear();
		}

		void Molecule::init()
		{
			// Add views.
			BaseModel::init();

			// Create GL buffers.
			_createBuffers();

			// Set color mode.
			setColorMode();

			// Create RT data
			_createRTData();

			// Set default representation.
			setRepresentation();
		}

		void Molecule::_addItems()
		{
			// Add views.
			addItem( (View::BaseView<BaseModel> *)Generic::create<Molecule, View::D3::Sphere>( this ) );
			addItem( (View::BaseView<BaseModel> *)Generic::create<Molecule, View::D3::Cylinder>( this ) );
			addItem( (View::BaseView<BaseModel> *)Generic::create<Molecule, View::D3::Box>( this ) );
			addItem( (View::BaseView<BaseModel> *)Generic::create<Molecule, View::UI::MoleculeStructure>( this ) );
		}

		void Molecule::setRepresentation() { _notifyViews( Event::VTX_EVENT_MODEL::CHANGE_REPRESENTATION ); }

		void Molecule::setColorMode()
		{
			if ( _atomColors.empty() ) _atomColors.resize( _atoms.size() );

			switch ( Setting::Rendering::colorMode )
			{
			case View::MOLECULE_COLOR_MODE::ATOM:
				for ( uint i = 0; i < uint( _atoms.size() ); ++i )
				{
					_atomColors[ i ] = _atoms[ i ]->getColor();
				}
				break;
			case View::MOLECULE_COLOR_MODE::RESIDUE:
				for ( uint i = 0; i < uint( _atoms.size() ); ++i )
				{
					_atomColors[ i ] = _atoms[ i ]->getResiduePtr()->getColor();
				}
				break;
			case View::MOLECULE_COLOR_MODE::CHAIN:
				for ( uint i = 0; i < uint( _atoms.size() ); ++i )
				{
					_atomColors[ i ] = _atoms[ i ]->getChainPtr()->getColor();
				}
				break;
			case View::MOLECULE_COLOR_MODE::PROTEIN:
				for ( uint i = 0; i < uint( _atoms.size() ); ++i )
				{
					_atomColors[ i ] = _color;
				}
				break;
			default: break;
			}

			glBindBuffer( GL_ARRAY_BUFFER, _atomColorsVBO );
			glBufferData( GL_ARRAY_BUFFER, sizeof( Vec3f ) * _atomColors.size(), _atomColors.data(), GL_STATIC_DRAW );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
		}

		void Molecule::print() const
		{
			VTX_INFO( "Molecule: " + _name );
			VTX_INFO( "Chains: " + std::to_string( _chains.size() ) + " / Residues: "
					  + std::to_string( _residues.size() ) + " / Atoms: " + std::to_string( _atoms.size() )
					  + " / Bonds: " + std::to_string( _bonds.size() ) );
		}

		void Molecule::setSelected( const bool p_selected )
		{
			BaseModel::setSelected( p_selected );
			if ( isSelected() )
			{ addItem( (View::BaseView<BaseModel> *)Generic::create<Molecule, View::UI::Molecule>( this ) ); }
			else
			{
				Generic::destroy( removeItem( ID::View::UI_MOLECULE ) );
			}
		}

		void Molecule::_createBuffers()
		{
			glGenBuffers( 1, &_atomPositionsVBO );
			glBindBuffer( GL_ARRAY_BUFFER, _atomPositionsVBO );
			glBufferData( GL_ARRAY_BUFFER,
						  sizeof( Vec3f ) * _atomPositionsFrames[ 0 ].size(),
						  _atomPositionsFrames[ 0 ].data(),
						  GL_STATIC_DRAW );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );

			glGenBuffers( 1, &_atomColorsVBO );
			glBindBuffer( GL_ARRAY_BUFFER, _atomColorsVBO );
			glBufferData( GL_ARRAY_BUFFER, sizeof( Vec3f ) * _atomColors.size(), _atomColors.data(), GL_STATIC_DRAW );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );

			glGenBuffers( 1, &_atomRadiusVBO );
			glBindBuffer( GL_ARRAY_BUFFER, _atomRadiusVBO );
			glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * _atomRadius.size(), _atomRadius.data(), GL_STATIC_DRAW );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );

			// ibo
			glGenBuffers( 1, &_bondsIBO );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _bondsIBO );
			glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( uint ) * _bonds.size(), _bonds.data(), GL_STATIC_DRAW );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

			// vao
			glGenVertexArrays( 1, &_vao );
			glBindVertexArray( _vao );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _bondsIBO );

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

			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

			glBindVertexArray( 0 );
		}

		void Molecule::_createRTData()
		{
			const uint nbAtoms = getAtomCount();
			const uint nbBonds = getBondCount() / 2;
			_rtPrimitives.resize( nbAtoms + nbBonds );

			const std::vector<Vec3f> & positions = getAtomPositionFrame( 0 );

			uint idPrimitive = 0;
			uint cptAtoms	 = 0;
			uint cptBonds	 = 0;

			_rtMaterials.emplace_back( new Renderer::DiffuseMaterial( VEC3F_XYZ ) );

			// Add atoms and bonds of each residue
			for ( uint i = 0; i < getResidueCount(); ++i )
			{
				// TODO: remove material duplication + only allow chain colors
				const Model::Residue & r = getResidue( i );
				_rtMaterials.emplace_back( new Renderer::DiffuseMaterial( r.getChainPtr()->getColor() ) );

				const uint idFirstAtomRes = r.getIdFirstAtom();
				const uint nbAtomsRes	  = r.getAtomCount();
				const uint idFirstBondRes = r.getIdFirstBond();
				const uint nbBondsRes	  = r.getBondCount();

				for ( uint j = idFirstAtomRes; j < idFirstAtomRes + nbAtomsRes; ++j )
				{
					_rtPrimitives[ idPrimitive ]
						= new Renderer::Sphere( positions[ j ], getAtomRadius( j ) * 0.3f, _rtMaterials.back() );
					idPrimitive++;
					cptAtoms++;
				}
				for ( uint j = idFirstBondRes; j < idFirstBondRes + nbBondsRes * 2; j += 2 )
				{
					const Vec3f & a1 = positions[ getBond( j ) ];
					const Vec3f & a2 = positions[ getBond( j + 1 ) ];

					_rtPrimitives[ idPrimitive ] = new Renderer::Cylinder( a1, a2, 0.2f, _rtMaterials.front() );
					idPrimitive++;
					cptBonds++;
				}
			}

			// Add bonds out of residues
			for ( ; cptBonds < nbBonds; cptBonds++ )
			{
				const Vec3f & a1 = positions[ getBond( cptBonds * 2 ) ];
				const Vec3f & a2 = positions[ getBond( cptBonds * 2 + 1 ) ];

				_rtPrimitives[ idPrimitive ] = new Renderer::Cylinder( a1, a2, 0.2f, _rtMaterials.front() );
				idPrimitive++;
			}
		}

		void Molecule::bindBuffers()
		{
			glBindVertexArray( _vao );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _bondsIBO );
		}

		void Molecule::unbindBuffers()
		{
			glBindVertexArray( 0 );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		}

		void Molecule::render()
		{
			bindBuffers();
			_notifyViews( Event::VTX_EVENT_MODEL::RENDER );
			unbindBuffers();
		}
	} // namespace Model
} // namespace VTX
