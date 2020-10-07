#include "molecule.hpp"
#include "color/rgb.hpp"
#include "model/secondary_structure.hpp"
#include "ui/widget_factory.hpp"
#include "util/molecule.hpp"
#include "util/secondary_structure.hpp"
#include "view/d3/box.hpp"
#include "view/d3/cylinder.hpp"
#include "view/d3/sphere.hpp"
#include "view/ui/widget/molecule_structure.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Model
	{
		Molecule::Molecule()
		{
			addItem( ID::View::D3_SPHERE, new View::D3::Sphere( this ) );
			addItem( ID::View::D3_CYLINDER, new View::D3::Cylinder( this ) );
		}

		Molecule::~Molecule()
		{
			if ( _vao != GL_INVALID_VALUE )
			{
				OGL().glBindVertexArray( _vao );
				OGL().glBindBuffer( GL_ARRAY_BUFFER, _atomPositionsVBO );
				OGL().glDisableVertexAttribArray( ATTRIBUTE_LOCATION::ATOM_POSITION );
				OGL().glBindBuffer( GL_ARRAY_BUFFER, _atomColorsVBO );
				OGL().glDisableVertexAttribArray( ATTRIBUTE_LOCATION::ATOM_COLOR );
				OGL().glBindBuffer( GL_ARRAY_BUFFER, _atomRadiusVBO );
				OGL().glDisableVertexAttribArray( ATTRIBUTE_LOCATION::ATOM_RADIUS );
				OGL().glBindBuffer( GL_ARRAY_BUFFER, _atomVisibilitiesVBO );
				OGL().glDisableVertexAttribArray( ATTRIBUTE_LOCATION::ATOM_VISIBILITY );
				OGL().glBindBuffer( GL_ARRAY_BUFFER, 0 );
				OGL().glBindVertexArray( 0 );

				if ( _atomPositionsVBO != GL_INVALID_VALUE )
					OGL().glDeleteBuffers( 1, &_atomPositionsVBO );
				if ( _atomRadiusVBO != GL_INVALID_VALUE )
					OGL().glDeleteBuffers( 1, &_atomRadiusVBO );
				if ( _atomColorsVBO != GL_INVALID_VALUE )
					OGL().glDeleteBuffers( 1, &_atomColorsVBO );
				if ( _atomVisibilitiesVBO != GL_INVALID_VALUE )
					OGL().glDeleteBuffers( 1, &_atomVisibilitiesVBO );
				if ( _bondsIBO != GL_INVALID_VALUE )
					OGL().glDeleteBuffers( 1, &_bondsIBO );

				OGL().glDeleteVertexArrays( 1, &_vao );
			}

			Generic::clearVector( _atoms );
			Generic::clearVector( _residues );
			Generic::clearVector( _chains );
			Generic::clearVector( _bonds );

			if ( _secondaryStructure != nullptr )
			{
				delete _secondaryStructure;
			}
		}

		void Molecule::init()
		{
			VTXApp::get().getMainWindow().getOpenGLWidget().makeCurrent();
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
					Util::SecondaryStructure::computeSecondaryStructure( *this );
				}

				// Create secondary structure mesh.
				createSecondaryStructure();
				_fillBufferAtomVisibilities();

				Util::Molecule::refreshRepresentationState( *this );
			}

			_isInit = true;
			_notifyViews( Event::VTX_EVENT_MODEL::INIT );
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

		void Molecule::setFrame( const uint p_frameIdx )
		{
			if ( p_frameIdx > getFrameCount() )
			{
				VTX_WARNING( "Frame " + std::to_string( p_frameIdx ) + " does not exists / Count: " + std::to_string( getFrameCount() ) );
				return;
			}

			_currentFrame = p_frameIdx;
			_updateBufferAtomPositions();

			if ( _secondaryStructure != nullptr )
			{
				_secondaryStructure->setCurrentFrame();
			}
		}

		void Molecule::_initBufferAtomPositions()
		{
			OGL().glNamedBufferData( _atomPositionsVBO,
									 sizeof( Vec3f ) * GLsizei( _atomPositionsFrames[ _currentFrame ].size() ),
									 _atomPositionsFrames[ _currentFrame ].data(),
									 // static data ? buffer will never be modified : buffer will be updated each X frames
									 GLsizei( _atomPositionsFrames.size() ) == 1 ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW );
		}

		void Molecule::_updateBufferAtomPositions()
		{
			OGL().glNamedBufferSubData(
				_atomPositionsVBO, 0, sizeof( Vec3f ) * GLsizei( _atomPositionsFrames[ _currentFrame ].size() ), _atomPositionsFrames[ _currentFrame ].data() );
		}

		void Molecule::_fillBufferAtomRadius()
		{
			_bufferAtomRadius.resize( _atoms.size() );
			for ( uint i = 0; i < uint( _atoms.size() ); ++i )
			{
				_bufferAtomRadius[ i ] = _atoms[ i ]->getVdwRadius();
			}

			OGL().glNamedBufferData( _atomRadiusVBO, sizeof( float ) * GLsizei( _bufferAtomRadius.size() ), _bufferAtomRadius.data(), GL_STATIC_DRAW );
		}

		void Molecule::_fillBufferAtomColors()
		{
			_bufferAtomColors.resize( _atoms.size() );

			Generic::COLOR_MODE colorMode = _colorMode;
			if ( colorMode == Generic::COLOR_MODE::INHERITED )
			{
				colorMode = VTX_SETTING().colorMode;
			}

			for ( uint i = 0; i < uint( _atoms.size() ); ++i )
			{
				switch ( colorMode )
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
				case Generic::COLOR_MODE::RESIDUE: _bufferAtomColors[ i ] = _atoms[ i ]->getResiduePtr()->getColor(); break;
				case Generic::COLOR_MODE::CHAIN: _bufferAtomColors[ i ] = _atoms[ i ]->getChainPtr()->getColor(); break;
				case Generic::COLOR_MODE::PROTEIN: _bufferAtomColors[ i ] = _color; break;

				default: break;
				}
			}

			OGL().glNamedBufferData( _atomColorsVBO, sizeof( Color::Rgb ) * GLsizei( _bufferAtomColors.size() ), _bufferAtomColors.data(), GL_STATIC_DRAW );
		}

		void Molecule::_fillBufferAtomVisibilities()
		{
			_bufferAtomVisibilities.resize( _atoms.size() );
			for ( uint i = 0; i < uint( _atoms.size() ); ++i )
			{
				Atom * const atom = _atoms[ i ];
				// Solvent hidden.
				if ( _showSolvent == false && atom->getType() == Atom::TYPE::SOLVENT )
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

			OGL().glNamedBufferData( _atomVisibilitiesVBO, sizeof( uint ) * GLsizei( _bufferAtomVisibilities.size() ), _bufferAtomVisibilities.data(), GL_STATIC_DRAW );
		}

		void Molecule::_fillBufferBonds()
		{
			_bufferBonds.resize( _bonds.size() * 2 );
			for ( uint i = 0; i < _bonds.size(); i++ )
			{
				_bufferBonds[ 2u * i ]		= _bonds[ i ]->getIndexFirstAtom();
				_bufferBonds[ 2u * i + 1u ] = _bonds[ i ]->getIndexSecondAtom();
			}

			OGL().glNamedBufferData( _bondsIBO, sizeof( uint ) * GLsizei( _bufferBonds.size() ), _bufferBonds.data(), GL_STATIC_DRAW );
		}

		void Molecule::print() const
		{
			// TODO: add more infos in debug (solvents, ions, ss...).
			VTX_INFO( "Molecule: " + _name );
			VTX_INFO( "Chains: " + std::to_string( _chains.size() ) + " / Residues: " + std::to_string( _residues.size() ) + " / Atoms: " + std::to_string( _atoms.size() )
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
				// addItem( (View::BaseView<BaseModel> *)new View::UI::Molecule( this ) );
			}
			else
			{
				// delete removeItem( ID::View::UI_MOLECULE );
			}
		}

		void Molecule::_createBuffers()
		{
			OGL().glGenBuffers( 1, &_atomPositionsVBO );
			OGL().glBindBuffer( GL_ARRAY_BUFFER, 0 );
			OGL().glGenBuffers( 1, &_atomColorsVBO );
			OGL().glBindBuffer( GL_ARRAY_BUFFER, 0 );
			OGL().glGenBuffers( 1, &_atomRadiusVBO );
			OGL().glBindBuffer( GL_ARRAY_BUFFER, 0 );
			OGL().glGenBuffers( 1, &_atomVisibilitiesVBO );
			OGL().glBindBuffer( GL_ARRAY_BUFFER, 0 );
			OGL().glGenBuffers( 1, &_bondsIBO );
			OGL().glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

			OGL().glGenVertexArrays( 1, &_vao );
			OGL().glBindVertexArray( _vao );

			OGL().glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _bondsIBO );

			OGL().glBindBuffer( GL_ARRAY_BUFFER, _atomPositionsVBO );
			OGL().glEnableVertexAttribArray( ATTRIBUTE_LOCATION::ATOM_POSITION );
			OGL().glVertexAttribPointer( ATTRIBUTE_LOCATION::ATOM_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof( Vec3f ), 0 );
			OGL().glBindBuffer( GL_ARRAY_BUFFER, 0 );

			OGL().glBindBuffer( GL_ARRAY_BUFFER, _atomColorsVBO );
			OGL().glEnableVertexAttribArray( ATTRIBUTE_LOCATION::ATOM_COLOR );
			OGL().glVertexAttribPointer( ATTRIBUTE_LOCATION::ATOM_COLOR, 3, GL_FLOAT, GL_FALSE, sizeof( Color::Rgb ), 0 );
			OGL().glBindBuffer( GL_ARRAY_BUFFER, 0 );

			OGL().glBindBuffer( GL_ARRAY_BUFFER, _atomRadiusVBO );
			OGL().glEnableVertexAttribArray( ATTRIBUTE_LOCATION::ATOM_RADIUS );
			OGL().glVertexAttribPointer( ATTRIBUTE_LOCATION::ATOM_RADIUS, 1, GL_FLOAT, GL_FALSE, sizeof( float ), 0 );
			OGL().glBindBuffer( GL_ARRAY_BUFFER, 0 );

			OGL().glBindBuffer( GL_ARRAY_BUFFER, _atomVisibilitiesVBO );
			OGL().glEnableVertexAttribArray( ATTRIBUTE_LOCATION::ATOM_VISIBILITY );
			OGL().glVertexAttribPointer( ATTRIBUTE_LOCATION::ATOM_VISIBILITY, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof( uint ), 0 );
			OGL().glBindBuffer( GL_ARRAY_BUFFER, 0 );

			OGL().glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
			OGL().glBindVertexArray( 0 );
		}

		void Molecule::render()
		{
			BaseModel3D::render();
			if ( _secondaryStructure != nullptr )
			{
				_secondaryStructure->render();
			}
		}

		void Molecule::bindBuffers()
		{
			OGL().glBindVertexArray( _vao );
			OGL().glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, _bondsIBO );
		}

		void Molecule::unbindBuffers()
		{
			OGL().glBindVertexArray( 0 );
			OGL().glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
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
					const Residue &	 otherResidue = p_molecule.getResidue( otherChain.getIndexFirstResidue() + residueIndex );
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
			if ( _secondaryStructure != nullptr )
			{
				delete _secondaryStructure;
			}

			_secondaryStructure = new SecondaryStructure( this );
			_secondaryStructure->init();
			_secondaryStructure->print();
		}

		void Molecule::toggleSequenceVisibility()
		{
			if ( hasItem( ( ID::View::UI_MOLECULE_SEQUENCE ) ) )
			{
				// delete removeItem( ID::View::UI_MOLECULE_SEQUENCE );
			}
			else
			{
				// addItem( (View::BaseView<BaseModel> *)new View::UI::MoleculeSequence( this ) );
			}
		}

	} // namespace Model
} // namespace VTX
