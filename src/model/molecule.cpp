#include "molecule.hpp"
#include "util/color.hpp"
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
			glBindBuffer( GL_ARRAY_BUFFER, _atomVisibilitiesVBO );
			glDisableVertexAttribArray( ATTRIBUTE_LOCATION::ATOM_VISIBILITY );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
			glBindVertexArray( 0 );

			if ( _atomPositionsVBO != GL_INVALID_VALUE ) glDeleteBuffers( 1, &_atomPositionsVBO );
			if ( _atomRadiusVBO != GL_INVALID_VALUE ) glDeleteBuffers( 1, &_atomRadiusVBO );
			if ( _atomColorsVBO != GL_INVALID_VALUE ) glDeleteBuffers( 1, &_atomColorsVBO );
			if ( _atomVisibilitiesVBO != GL_INVALID_VALUE ) glDeleteBuffers( 1, &_atomVisibilitiesVBO );
			if ( _bondsIBO != GL_INVALID_VALUE ) glDeleteBuffers( 1, &_bondsIBO );

			if ( _vao != GL_INVALID_VALUE ) glDeleteVertexArrays( 1, &_vao );

			Generic::clearVector<Atom>( _atoms );
			Generic::clearVector<Residue>( _residues );
			Generic::clearVector<Chain>( _chains );
			Generic::clearVector<Bond>( _bonds );
		}

		void Molecule::init()
		{
			// Add views.
			BaseModel::init();

			// Create GL buffers.
			_createBuffers();

			// Fill buffers.
			_fillBufferAtomPositions();
			_fillBufferAtomRadius();
			_fillBufferAtomColors();
			_fillBufferAtomVisibilities();
			_fillBufferBonds();

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

		void Molecule::setColorMode() { _fillBufferAtomColors(); }

		void Molecule::setFrame( const uint p_frameIdx )
		{
			if ( p_frameIdx > getFrameCount() )
			{
				VTX_WARNING( "Frame " + std::to_string( p_frameIdx )
							 + " does not exists / Count: " + std::to_string( getFrameCount() ) );
				return;
			}

			_currentFrame = p_frameIdx;
			_fillBufferAtomPositions();
		}

		void Molecule::_fillBufferAtomPositions()
		{
			_bufferAtomPositions.resize( _atoms.size() );
			for ( uint i = 0; i < uint( _atoms.size() ); ++i )
			{
				_bufferAtomPositions[ i ] = _atomPositionsFrames[ _currentFrame ][ i ];
			}

			glBindBuffer( GL_ARRAY_BUFFER, _atomPositionsVBO );
			glBufferData( GL_ARRAY_BUFFER,
						  sizeof( Vec3f ) * _bufferAtomPositions.size(),
						  _bufferAtomPositions.data(),
						  GL_STATIC_DRAW );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
		}

		void Molecule::_fillBufferAtomRadius()
		{
			_bufferAtomRadius.resize( _atoms.size() );
			for ( uint i = 0; i < uint( _atoms.size() ); ++i )
			{
				_bufferAtomRadius[ i ] = _atoms[ i ]->getVdwRadius();
			}

			glBindBuffer( GL_ARRAY_BUFFER, _atomRadiusVBO );
			glBufferData(
				GL_ARRAY_BUFFER, sizeof( float ) * _bufferAtomRadius.size(), _bufferAtomRadius.data(), GL_STATIC_DRAW );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
		}

		void Molecule::_fillBufferAtomColors()
		{
			_bufferAtomColors.resize( _atoms.size() );

			for ( uint i = 0; i < uint( _atoms.size() ); ++i )
			{
				switch ( Setting::Rendering::colorMode )
				{
				case View::MOLECULE_COLOR_MODE::ATOM: _bufferAtomColors[ i ] = _atoms[ i ]->getColor(); break;
				case View::MOLECULE_COLOR_MODE::RESIDUE:
					_bufferAtomColors[ i ] = _atoms[ i ]->getResiduePtr()->getColor();
					break;
				case View::MOLECULE_COLOR_MODE::CHAIN:
					_bufferAtomColors[ i ] = _atoms[ i ]->getChainPtr()->getColor();
					break;
				case View::MOLECULE_COLOR_MODE::PROTEIN: _bufferAtomColors[ i ] = _color; break;

				default: break;
				}
			}

			glBindBuffer( GL_ARRAY_BUFFER, _atomColorsVBO );
			glBufferData(
				GL_ARRAY_BUFFER, sizeof( Vec3f ) * _bufferAtomColors.size(), _bufferAtomColors.data(), GL_STATIC_DRAW );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
		}

		void Molecule::_fillBufferAtomVisibilities()
		{
			_bufferAtomVisibilities.resize( _atoms.size() );
			for ( uint i = 0; i < uint( _atoms.size() ); ++i )
			{
				if ( _showSolvent == false && _atoms[ i ]->getType() == Atom::ATOM_TYPE::Solvent )
				{ _bufferAtomVisibilities[ i ] = 0u; }
				else if ( _showIon == false && _atoms[ i ]->getType() == Atom::ATOM_TYPE::ION )
				{
					_bufferAtomVisibilities[ i ] = 0u;
				}
				else
				{
					_bufferAtomVisibilities[ i ] = 1u;
				}
			}

			glBindBuffer( GL_ARRAY_BUFFER, _atomVisibilitiesVBO );
			glBufferData( GL_ARRAY_BUFFER,
						  sizeof( uint ) * _bufferAtomVisibilities.size(),
						  _bufferAtomVisibilities.data(),
						  GL_STATIC_DRAW );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
		}

		void Molecule::_fillBufferBonds()
		{
			_bufferBonds.resize( _bonds.size() * 2 );
			for ( uint i = 0; i < _bonds.size(); i++ )
			{
				_bufferBonds[ 2u * i ]		= _bonds[ i ]->getIndexFirstAtom();
				_bufferBonds[ 2u * i + 1u ] = _bonds[ i ]->getIndexSecondAtom();
			}

			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _bondsIBO );
			glBufferData(
				GL_ELEMENT_ARRAY_BUFFER, sizeof( uint ) * _bufferBonds.size(), _bufferBonds.data(), GL_STATIC_DRAW );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		}

		void Molecule::print() const
		{
			VTX_INFO( "Molecule: " + _name );
			VTX_INFO( "Chains: " + std::to_string( _chains.size() ) + " / Residues: "
					  + std::to_string( _residues.size() ) + " / Atoms: " + std::to_string( _atoms.size() )
					  + " / Bonds: " + std::to_string( _bonds.size() ) );

			// Display unknown symbols.
			const std::unordered_set<std::string> & unknownResidueSymbols = getUnknownResidueSymbols();
			if ( unknownResidueSymbols.empty() == false )
			{
				std::string unknownResidueSymbolsStr = "";
				for ( std::string symbol : unknownResidueSymbols )
				{
					unknownResidueSymbolsStr += symbol + " ";
				}
				VTX_WARNING( "Unknown residue symbols : " + unknownResidueSymbolsStr );
			}

			const std::unordered_set<std::string> & unknownAtomSymbols = getUnknownAtomSymbols();
			if ( unknownAtomSymbols.empty() == false )
			{
				std::string unknownAtomSymbolsStr = "";
				for ( std::string symbol : unknownAtomSymbols )
				{
					unknownAtomSymbolsStr += symbol + " ";
				}
				VTX_WARNING( "Unknown atom symbols : " + unknownAtomSymbolsStr );
			}
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
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
			glGenBuffers( 1, &_atomColorsVBO );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
			glGenBuffers( 1, &_atomRadiusVBO );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
			glGenBuffers( 1, &_atomVisibilitiesVBO );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
			glGenBuffers( 1, &_bondsIBO );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

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

			glBindBuffer( GL_ARRAY_BUFFER, _atomVisibilitiesVBO );
			glEnableVertexAttribArray( ATTRIBUTE_LOCATION::ATOM_VISIBILITY );
			glVertexAttribPointer(
				ATTRIBUTE_LOCATION::ATOM_VISIBILITY, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof( uint ), 0 );
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
			_notifyViews( Event::VTX_EVENT_MODEL::RENDER );
			unbindBuffers();
		}
	} // namespace Model
} // namespace VTX
