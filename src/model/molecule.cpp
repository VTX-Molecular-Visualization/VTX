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
		}

		void Molecule::init()
		{
			// Add views.
			BaseModel::init();

			// Create GL buffers.
			_createBuffers();

			// Set frame.
			if ( getFrameCount() > 0 ) { setFrame( 0 ); }

			// Set color mode.
			setColor( Util::Color::randomPastelColor() );

			_fillBufferAtoms();
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
			_fillBufferAtoms();
		}

		void Molecule::_fillBufferAtoms()
		{
			_bufferAtomPosition.clear();
			_bufferAtomRadius.clear();

			for ( uint i = 0; i < uint( _atoms.size() ); ++i )
			{
				if ( _showSolvant == false && _atoms[ i ]->isSolvant() ) { continue; }

				_bufferAtomPosition.emplace_back( _atomPositionsFrames[ _currentFrame ][ i ] );
				_bufferAtomRadius.emplace_back( _atoms[ i ]->getVdwRadius() );
			}

			glBindBuffer( GL_ARRAY_BUFFER, _atomPositionsVBO );
			glBufferData( GL_ARRAY_BUFFER,
						  sizeof( Vec3f ) * _bufferAtomPosition.size(),
						  _bufferAtomPosition.data(),
						  GL_STATIC_DRAW );

			glBindBuffer( GL_ARRAY_BUFFER, 0 );
			glBindBuffer( GL_ARRAY_BUFFER, _atomRadiusVBO );
			glBufferData(
				GL_ARRAY_BUFFER, sizeof( float ) * _bufferAtomRadius.size(), _bufferAtomRadius.data(), GL_STATIC_DRAW );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );

			_fillBufferAtomColors();
		}

		void Molecule::_fillBufferAtomColors()
		{
			_bufferAtomColors.clear();

			for ( uint i = 0; i < uint( _atoms.size() ); ++i )
			{
				if ( _showSolvant == false && _atoms[ i ]->isSolvant() ) { continue; }

				switch ( Setting::Rendering::colorMode )
				{
				case View::MOLECULE_COLOR_MODE::ATOM: _bufferAtomColors.push_back( _atoms[ i ]->getColor() ); break;
				case View::MOLECULE_COLOR_MODE::RESIDUE:
					_bufferAtomColors.push_back( _atoms[ i ]->getResiduePtr()->getColor() );
					break;
				case View::MOLECULE_COLOR_MODE::CHAIN:
					_bufferAtomColors.push_back( _atoms[ i ]->getChainPtr()->getColor() );
					break;
				case View::MOLECULE_COLOR_MODE::PROTEIN: _bufferAtomColors.push_back( _color ); break;

				default: break;
				}
			}

			glBindBuffer( GL_ARRAY_BUFFER, _atomColorsVBO );
			glBufferData(
				GL_ARRAY_BUFFER, sizeof( Vec3f ) * _bufferAtomColors.size(), _bufferAtomColors.data(), GL_STATIC_DRAW );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );
		}

		void Molecule::_fillBufferBonds()
		{
			_bufferBonds.clear();

			uint counterSolvant = 0;
			for ( uint i = 0; i < _bonds.size(); ++i )
			{
				if ( _showSolvant == false )
				{
					if ( _atoms[ _bonds[ i ]->getIndexFirstAtom() ]->isSolvant()
						 || _atoms[ _bonds[ i ]->getIndexSecondAtom() ]->isSolvant() )
					{
						counterSolvant++;
						continue;
					}
				}

				_bufferBonds.emplace_back( _bonds[ i ]->getIndexFirstAtom() - counterSolvant );
				_bufferBonds.emplace_back( _bonds[ i ]->getIndexSecondAtom() - counterSolvant );
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
			// glBindBuffer( GL_ARRAY_BUFFER, _atomPositionsVBO );
			// glBufferData( GL_ARRAY_BUFFER,
			//			  sizeof( Vec3f ) * _atomPositionsFrames[ _currentFrame ].size(),
			//			  _atomPositionsFrames[ _currentFrame ].data(),
			//			  GL_STATIC_DRAW );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );

			glGenBuffers( 1, &_atomColorsVBO );
			// glBindBuffer( GL_ARRAY_BUFFER, _atomColorsVBO );
			// glBufferData( GL_ARRAY_BUFFER, sizeof( Vec3f ) * _atomColors.size(), _atomColors.data(), GL_STATIC_DRAW
			// );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );

			glGenBuffers( 1, &_atomRadiusVBO );
			// glBindBuffer( GL_ARRAY_BUFFER, _atomRadiusVBO );
			// glBufferData( GL_ARRAY_BUFFER, sizeof( float ) * _atomRadius.size(), _atomRadius.data(), GL_STATIC_DRAW
			// );
			glBindBuffer( GL_ARRAY_BUFFER, 0 );

			// ibo
			glGenBuffers( 1, &_bondsIBO );
			// glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _bondsIBO );
			// glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( uint ) * _bonds.size(), _bonds.data(), GL_STATIC_DRAW );
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
			_notifyViews( Event::VTX_EVENT_MODEL::RENDER );
			unbindBuffers();
		}
	} // namespace Model
} // namespace VTX
