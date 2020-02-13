#include "molecule.hpp"
#include "util/type.hpp"
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
			return;

			if ( _vao != GL_INVALID_VALUE ) glDeleteBuffers( 1, &_vao );
			if ( _atomPositionsVBO != GL_INVALID_VALUE ) glDeleteBuffers( 1, &_atomPositionsVBO );
			if ( _atomRadiusVBO != GL_INVALID_VALUE ) glDeleteBuffers( 1, &_atomRadiusVBO );
			if ( _atomColorsVBO != GL_INVALID_VALUE ) glDeleteBuffers( 1, &_atomColorsVBO );
			if ( _bondsIBO != GL_INVALID_VALUE ) glDeleteBuffers( 1, &_bondsIBO );
		}

		void Molecule::init()
		{
			// Add views.
			BaseModel::init();

			// Create GL buffers.
			_createBuffers();

			// Set color mode.
			setColorMode();

			// Set default representation.
			setRepresentation();
		}

		void Molecule::_addItems()
		{
			// Add views.

			addItem( (View::BaseView<BaseModel> *)Generic::create<Molecule, View::D3::Sphere>( this ) );
			addItem( (View::BaseView<BaseModel> *)Generic::create<Molecule, View::D3::Cylinder>( this ) );
			addItem( (View::BaseView<BaseModel> *)Generic::create<Molecule, View::UI::MoleculeStructure>( this ) );
		}

		void Molecule::setRepresentation() { _notifyViews( Event::EVENT_MODEL::CHANGE_REPRESENTATION ); }

		void Molecule::setColorMode()
		{
			_atomColors.clear();

			switch ( Setting::Rendering::colorMode )
			{
			case View::MOLECULE_COLOR_MODE::ATOM:
				for ( Atom & atom : _atoms )
				{
					_atomColors.emplace_back(
						Vec3f( atom.getColor()[ 0 ], atom.getColor()[ 1 ], atom.getColor()[ 2 ] ) );
				}
				break;
			case View::MOLECULE_COLOR_MODE::RESIDUE:
				for ( Atom & atom : _atoms )
				{
					_atomColors.emplace_back( atom.getResiduePtr()->getColor()[ 0 ],
											  atom.getResiduePtr()->getColor()[ 1 ],
											  atom.getResiduePtr()->getColor()[ 2 ] );
				}
				break;
			case View::MOLECULE_COLOR_MODE::CHAIN:
				for ( Atom & atom : _atoms )
				{
					_atomColors.emplace_back( atom.getChainPtr()->getColor()[ 0 ],
											  atom.getChainPtr()->getColor()[ 1 ],
											  atom.getChainPtr()->getColor()[ 2 ] );
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
				Generic::destroy<View::UI::Molecule>( (View::UI::Molecule *)removeItem( ID::View::UI_MOLECULE ) );
			}
		}

		void Molecule::setSelectedChain( Chain * const p_chain )
		{
			if ( _selectedChain != nullptr ) { _selectedChain->setSelected( false ); }
			try
			{
				_selectedChain = p_chain;
				_selectedChain->setSelected( true );
			}
			catch ( const std::exception )
			{
				VTX_WARNING( "Failed to select chain" );
				_selectedChain = nullptr;
			}
		}

		void Molecule::setSelectedResidue( Residue * const p_residue )
		{
			if ( _selectedResidue != nullptr ) { _selectedResidue->setSelected( false ); }
			try
			{
				_selectedResidue = p_residue;
				_selectedResidue->setSelected( true );
				setSelectedChain( _selectedResidue->getChainPtr() );
			}
			catch ( const std::exception )
			{
				VTX_WARNING( "Failed to select residue" );
				_selectedResidue = nullptr;
			}
		}

		void Molecule::setSelectedAtom( Atom * const p_atom )
		{
			if ( _selectedAtom != nullptr ) { _selectedAtom->setSelected( false ); }
			try
			{
				_selectedAtom = p_atom;
				_selectedAtom->setSelected( true );
				setSelectedResidue( _selectedAtom->getResiduePtr() );
			}
			catch ( const std::exception )
			{
				VTX_WARNING( "Failed to select atom" );
				_selectedAtom = nullptr;
			}
		}

		void Molecule::resetSelectedChain()
		{
			if ( _selectedChain != nullptr )
			{
				_selectedChain->setSelected( false );
				_selectedChain = nullptr;
			}

			resetSelectedResidue();
		}

		void Molecule::resetSelectedResidue()
		{
			if ( _selectedResidue != nullptr )
			{
				_selectedResidue->setSelected( false );
				_selectedResidue = nullptr;
			}

			resetSelectedAtom();
		}

		void Molecule::resetSelectedAtom()
		{
			if ( _selectedAtom != nullptr )
			{
				_selectedAtom->setSelected( false );
				_selectedAtom = nullptr;
			}
		}

		void Molecule::_createBuffers()
		{
			glGenBuffers( 1, &_atomPositionsVBO );
			glBindBuffer( GL_ARRAY_BUFFER, _atomPositionsVBO );
			glBufferData(
				GL_ARRAY_BUFFER, sizeof( Vec3f ) * _atomPositions.size(), _atomPositions.data(), GL_STATIC_DRAW );
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
			_notifyViews( Event::EVENT_MODEL::RENDER );
			unbindBuffers();
		}
	} // namespace Model
} // namespace VTX
