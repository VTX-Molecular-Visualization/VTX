#include "molecule.hpp"
#include "color/rgb.hpp"
#include "model/ribbon.hpp"
#include "util/molecule.hpp"
#include "view/d3/box.hpp"
#include "view/d3/cylinder.hpp"
#include "view/d3/sphere.hpp"
#include "view/d3/triangle_ribbon.hpp"
#include "view/ui/molecule.hpp"
#include "view/ui/molecule_sequence.hpp"
#include "view/ui/molecule_structure.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Model
	{
		Molecule::Molecule()
		{
			addItem( (View::BaseView<BaseModel> *)new View::D3::Sphere( this ) );
			addItem( (View::BaseView<BaseModel> *)new View::D3::Cylinder( this ) );
			addItem( (View::BaseView<BaseModel> *)new View::UI::MoleculeStructure( this ) );
		}

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

			if ( _atomPositionsVBO != GL_INVALID_VALUE )
				glDeleteBuffers( 1, &_atomPositionsVBO );
			if ( _atomRadiusVBO != GL_INVALID_VALUE )
				glDeleteBuffers( 1, &_atomRadiusVBO );
			if ( _atomColorsVBO != GL_INVALID_VALUE )
				glDeleteBuffers( 1, &_atomColorsVBO );
			if ( _atomVisibilitiesVBO != GL_INVALID_VALUE )
				glDeleteBuffers( 1, &_atomVisibilitiesVBO );
			if ( _bondsIBO != GL_INVALID_VALUE )
				glDeleteBuffers( 1, &_bondsIBO );
			if ( _vao != GL_INVALID_VALUE )
				glDeleteVertexArrays( 1, &_vao );

			Generic::clearVector( _atoms );
			Generic::clearVector( _residues );
			Generic::clearVector( _chains );
			Generic::clearVector( _bonds );

			if ( _ribbon != nullptr )
			{
				VTXApp::get().getScene().removeMesh( _ribbon );
				delete _ribbon;
			}
		}

		void Molecule::init()
		{
			// Compute global AABB of atom positions (taking into account each frame).
			_computeGlobalPositionsAABB();

			// Create GL buffers.
			_createBuffers();

			// Fill buffers.
			if ( _atomPositionsFrames.size() > 0 )
			{
				_initBufferAtomPositions();
				_fillBufferAtomRadius();
				_fillBufferAtomColors();
				//_fillBufferAtomVisibilities(); // Done after.
				_fillBufferBonds();

				// Compute secondary structure if not loaded.
				if ( _configuration.isSecondaryStructureLoadedFromFile == false )
				{
					Util::Molecule::computeSecondaryStructure( *this );
				}

				// Create secondary structure mesh.
				createSecondaryStructure();
				refreshVisibility();
			}
		}

		void Molecule::_computeGlobalPositionsAABB()
		{
			for ( AtomPositionsFrame frame : _atomPositionsFrames )
			{
				for ( Vec3f pos : frame )
				{
					_aabb.extend( pos );
				}
			}
		}

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
			_updateBufferAtomPositions();
		}

		void Molecule::_initBufferAtomPositions() const
		{
			glNamedBufferData( _atomPositionsVBO,
							   sizeof( Vec3f ) * _atomPositionsFrames[ _currentFrame ].size(),
							   _atomPositionsFrames[ _currentFrame ].data(),
							   // static data ? buffer will never be modified : buffer will be updated each X frames
							   _atomPositionsFrames.size() == 1 ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW );
		}

		void Molecule::_updateBufferAtomPositions() const
		{
			glNamedBufferSubData( _atomPositionsVBO,
								  0,
								  sizeof( Vec3f ) * _atomPositionsFrames[ _currentFrame ].size(),
								  _atomPositionsFrames[ _currentFrame ].data() );
		}

		void Molecule::_fillBufferAtomRadius()
		{
			_bufferAtomRadius.resize( _atoms.size() );
			for ( uint i = 0; i < uint( _atoms.size() ); ++i )
			{
				_bufferAtomRadius[ i ] = _atoms[ i ]->getVdwRadius();
			}

			glNamedBufferData(
				_atomRadiusVBO, sizeof( float ) * _bufferAtomRadius.size(), _bufferAtomRadius.data(), GL_STATIC_DRAW );
		}

		void Molecule::_fillBufferAtomColors()
		{
			_bufferAtomColors.resize( _atoms.size() );

			for ( uint i = 0; i < uint( _atoms.size() ); ++i )
			{
				switch ( VTX_SETTING().colorMode )
				{
				case Generic::COLOR_MODE::ATOM:
					if ( _atoms[ i ]->getSymbol() == Atom::SYMBOL::A_C )
					{
						_bufferAtomColors[ i ] = _atoms[ i ]->getChainPtr()->getColor();
					}
					else
					{
						_bufferAtomColors[ i ] = _atoms[ i ]->getColor();
					}
					break;
				case Generic::COLOR_MODE::RESIDUE:
					_bufferAtomColors[ i ] = _atoms[ i ]->getResiduePtr()->getColor();

					break;
				case Generic::COLOR_MODE::CHAIN: _bufferAtomColors[ i ] = _atoms[ i ]->getChainPtr()->getColor(); break;
				case Generic::COLOR_MODE::PROTEIN: _bufferAtomColors[ i ] = _color; break;

				default: break;
				}
			}

			glNamedBufferData( _atomColorsVBO,
							   sizeof( Color::Rgb ) * _bufferAtomColors.size(),
							   _bufferAtomColors.data(),
							   GL_STATIC_DRAW );
		}

		void Molecule::_fillBufferAtomVisibilities()
		{
			_bufferAtomVisibilities.resize( _atoms.size() );
			for ( uint i = 0; i < uint( _atoms.size() ); ++i )
			{
				Atom * const atom = _atoms[ i ];
				// Molecule not visible.
				if ( isVisible() == false )
				{
					_bufferAtomVisibilities[ i ] = 0u;
				}
				// Residue not visible.
				else if ( atom->getResiduePtr()->isVisible() == false )
				{
					_bufferAtomVisibilities[ i ] = 0u;
				}
				// Chain not visible.
				else if ( atom->getChainPtr()->isVisible() == false )
				{
					_bufferAtomVisibilities[ i ] = 0u;
				}
				// Solvent hidden.
				else if ( _showSolvent == false && atom->getType() == Atom::TYPE::SOLVENT )
				{
					_bufferAtomVisibilities[ i ] = 0u;
				}
				// Ion hidden.
				else if ( _showIon == false && atom->getType() == Atom::TYPE::ION )
				{
					_bufferAtomVisibilities[ i ] = 0u;
				}
				// Ok!
				else
				{
					_bufferAtomVisibilities[ i ] = 1u;
				}
			}

			glNamedBufferData( _atomVisibilitiesVBO,
							   sizeof( uint ) * _bufferAtomVisibilities.size(),
							   _bufferAtomVisibilities.data(),
							   GL_STATIC_DRAW );
		}

		void Molecule::_fillBufferBonds()
		{
			_bufferBonds.resize( _bonds.size() * 2 );
			for ( uint i = 0; i < _bonds.size(); i++ )
			{
				_bufferBonds[ 2u * i ]		= _bonds[ i ]->getIndexFirstAtom();
				_bufferBonds[ 2u * i + 1u ] = _bonds[ i ]->getIndexSecondAtom();
			}

			glNamedBufferData( _bondsIBO, sizeof( uint ) * _bufferBonds.size(), _bufferBonds.data(), GL_STATIC_DRAW );
		}

		void Molecule::print() const
		{
			// TODO: add more infos in debug (solvents, ions, ss...).
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

			VTX_DEBUG( "Sizeof molecule: " + std::to_string( sizeof( *this ) ) );
			VTX_DEBUG( "Sizeof chain: " + std::to_string( sizeof( *_chains[ 0 ] ) ) );
			VTX_DEBUG( "Sizeof residue: " + std::to_string( sizeof( *_residues[ 0 ] ) ) );
			VTX_DEBUG( "Sizeof atom: " + std::to_string( sizeof( *_atoms[ 0 ] ) ) );
			VTX_DEBUG( "Sizeof bond: " + std::to_string( sizeof( *_bonds[ 0 ] ) ) );
		}

		void Molecule::setSelected( const bool p_selected )
		{
			BaseSelectable::setSelected( p_selected );
			if ( isSelected() )
			{
				addItem( (View::BaseView<BaseModel> *)new View::UI::Molecule( this ) );
			}
			else
			{
				delete removeItem( ID::View::UI_MOLECULE );
			}
		}

		void Molecule::refreshVisibility()
		{
			_fillBufferAtomVisibilities();
			if ( _ribbon != nullptr )
			{
				_ribbon->refreshVisibility();
			}
			// Refresh representation state to remove invisible items.
			Util::Molecule::refreshRepresentationState( *this );
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
			glVertexAttribPointer( ATTRIBUTE_LOCATION::ATOM_COLOR, 3, GL_FLOAT, GL_FALSE, sizeof( Color::Rgb ), 0 );
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

		bool Molecule::mergeTopology( const Molecule & p_molecule )
		{
			if ( p_molecule.getAtomCount() > getAtomCount() )
			{
				VTX_ERROR( "Too many atoms in topology model" );
				return false;
			}
			else if ( getAtomCount() > p_molecule.getAtomCount() )
			{
				VTX_WARNING( "Topology model has less atoms than dynamic one (maybe solvent not present)" );
			}

			// Clear topology.
			Generic::clearVector( _chains );
			Generic::clearVector( _residues );

			// Create models.
			_chains.resize( p_molecule.getChainCount() );
			for ( uint i = 0; i < p_molecule.getChainCount(); ++i )
			{
				getChains()[ i ] = new Chain();
			}
			_residues.resize( p_molecule.getResidueCount() );
			for ( uint i = 0; i < p_molecule.getResidueCount(); ++i )
			{
				getResidues()[ i ] = new Residue();
			}

			setName( p_molecule.getName() );
			// Loop over other molecule chains.
			for ( uint chainIndex = 0; chainIndex < p_molecule.getChainCount(); ++chainIndex )
			{
				const Chain &  otherChain = p_molecule.getChain( chainIndex );
				Model::Chain & modelChain = getChain( chainIndex );
				modelChain.setIndex( otherChain.getIndex() );
				modelChain.setMoleculePtr( this );
				modelChain.setName( otherChain.getName() );
				modelChain.setIndexFirstResidue( otherChain.getIndexFirstResidue() );
				modelChain.setResidueCount( otherChain.getResidueCount() );

				for ( uint residueIndex = 0; residueIndex < otherChain.getResidueCount(); ++residueIndex )
				{
					const Residue & otherResidue
						= p_molecule.getResidue( otherChain.getIndexFirstResidue() + residueIndex );
					Model::Residue & modelResidue = getResidue( otherChain.getIndexFirstResidue() + residueIndex );
					modelResidue.setIndex( otherResidue.getIndex() );
					modelResidue.setMoleculePtr( this );
					modelResidue.setChainPtr( &modelChain );
					modelResidue.setIndexFirstAtom( otherResidue.getIndexFirstAtom() );
					modelResidue.setAtomCount( otherResidue.getAtomCount() );
					modelResidue.setSymbol( otherResidue.getSymbol() );

					for ( uint atomIndex = 0; atomIndex < otherResidue.getAtomCount(); ++atomIndex )
					{
						const Atom &  otherAtom = p_molecule.getAtom( otherResidue.getIndexFirstAtom() + atomIndex );
						Model::Atom & modelAtom = getAtom( otherResidue.getIndexFirstAtom() + atomIndex );
						modelAtom.setIndex( otherAtom.getIndex() );
						modelAtom.setResiduePtr( &modelResidue );
						modelAtom.setChainPtr( &modelChain );
					}
				}
			}

			// Loop over other atoms.
			if ( getAtomCount() > p_molecule.getAtomCount() )
			{
				addChain();
				addResidue();
				Model::Chain &	 modelChain	  = getChain( getChainCount() - 1 );
				Model::Residue & modelResidue = getResidue( getResidueCount() - 1 );
				modelChain.setIndex( getChainCount() - 1 );
				modelChain.setIndexFirstResidue( getResidueCount() - 1 );
				modelChain.setResidueCount( 1 );
				modelResidue.setIndex( getResidueCount() - 1 );
				modelResidue.setChainPtr( &modelChain );
				modelResidue.setIndexFirstAtom( p_molecule.getAtomCount() );
				modelResidue.setAtomCount( getAtomCount() - p_molecule.getAtomCount() );

				for ( uint atomIndex = 0; atomIndex < modelResidue.getAtomCount(); ++atomIndex )
				{
					Model::Atom & modelAtom = getAtom( modelResidue.getIndexFirstAtom() + atomIndex );
					modelAtom.setChainPtr( &modelChain );
					modelAtom.setResiduePtr( &modelResidue );
				}
			}

			return true;
		}

		void Molecule::createSecondaryStructure()
		{
			if ( _ribbon != nullptr )
			{
				VTXApp::get().getScene().removeMesh( _ribbon );
				delete _ribbon;
			}

			_ribbon = new Ribbon( this );
			_ribbon->init();
			_ribbon->print();
			VTXApp::get().getScene().addMesh( _ribbon );
		}

		void Molecule::toggleSequenceVisibility()
		{
			if ( hasItem( ( ID::View::UI_MOLECULE_SEQUENCE ) ) )
			{
				delete removeItem( ID::View::UI_MOLECULE_SEQUENCE );
			}
			else
			{
				addItem( (View::BaseView<BaseModel> *)new View::UI::MoleculeSequence( this ) );
			}
		}

	} // namespace Model
} // namespace VTX
