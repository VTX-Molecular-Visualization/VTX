#include "molecule.hpp"
#include "color/rgb.hpp"
#include "generic/factory.hpp"
#include "id.hpp"
#include "model/secondary_structure.hpp"
#include "tool/logger.hpp"
#include "ui/widget_factory.hpp"
#include "util/secondary_structure.hpp"
#include "view/d3/box.hpp"
#include "view/d3/cylinder.hpp"
#include "view/d3/sphere.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Model
	{
		Molecule::Molecule() : BaseModel3D( ID::Model::MODEL_MOLECULE ) {}

		Molecule::~Molecule()
		{
			MVC::MvcManager::get().deleteAllModels( _atoms );
			MVC::MvcManager::get().deleteAllModels( _bonds );
			MVC::MvcManager::get().deleteAllModels( _residues );
			MVC::MvcManager::get().deleteAllModels( _chains );

			if ( _secondaryStructure != nullptr )
				MVC::MvcManager::get().deleteModel( _secondaryStructure );
		}

		void Molecule::_init()
		{
			// Compute global AABB of atom positions (taking into account each frame).
			_computeGlobalPositionsAABB();

			// Fill buffers.
			if ( _atomPositionsFrames.size() > 0 )
			{
				_buffer->setAtomPositions( _atomPositionsFrames[ _currentFrame ] );
				_buffer->setAtomRadius( _bufferAtomRadius );
				_buffer->refreshAtomBufferCacheSize( (uint)_atoms.size() );
				_fillBufferAtomColors();
				_buffer->setAtomVisibilities( _bufferAtomVisibilities );
				_buffer->setAtomSelections( _bufferAtomSelection );
				_buffer->setBonds( _bufferBonds );

				// Compute secondary structure if not loaded.
				if ( _configuration.isSecondaryStructureLoadedFromFile == false )
				{
					Util::SecondaryStructure::computeSecondaryStructure( *this );
				}

				// Create secondary structure mesh.
				createSecondaryStructure();

				_setRepresentableMolecule( this );
				computeRepresentationTargets();
			}
		}

		void Molecule::_instanciate3DViews()
		{
			_addRenderable( MVC::MvcManager::get().instanciateView<View::D3::Sphere>( this, ID::View::D3_SPHERE ) );
			_addRenderable( MVC::MvcManager::get().instanciateView<View::D3::Cylinder>( this, ID::View::D3_CYLINDER ) );
		}

		void Molecule::_computeGlobalPositionsAABB()
		{
			for ( AtomPositionsFrame frame : _atomPositionsFrames )
			{
				for ( const Vec3f & pos : frame )
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
			_buffer->setAtomPositions( _atomPositionsFrames[ _currentFrame ] );

			if ( _secondaryStructure != nullptr )
			{
				_secondaryStructure->setCurrentFrame();
			}
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

			_buffer->setAtomColors( _bufferAtomColors );
		}

		void Molecule::_fillBufferAtomVisibilities()
		{
			_bufferAtomVisibilities.clear();
			_bufferAtomVisibilities.resize( _atoms.size(), 1u );
			for ( uint i = 0; i < uint( _atoms.size() ); ++i )
			{
				const Atom * const atom = _atoms[ i ];
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
			}

			_buffer->setAtomVisibilities( _bufferAtomVisibilities );
		}

		void Molecule::_fillBufferAtomSelections( const Model::Selection::MapChainIds * const p_selection )
		{
			_bufferAtomSelection.clear();
			_bufferAtomSelection.resize( _atoms.size(), 0u );
			if ( p_selection != nullptr )
			{
				for ( const std::pair<uint, Model::Selection::MapResidueIds> & pairChain : *p_selection )
				{
					for ( const std::pair<uint, Model::Selection::VecAtomIds> & pairResidue : pairChain.second )
					{
						for ( const uint & atomIndex : pairResidue.second )
						{
							_bufferAtomSelection[ atomIndex ] = 1u;
						}
					}
				}
			}

			_buffer->setAtomSelections( _bufferAtomSelection );
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

		void Molecule::render()
		{
			BaseModel3D::render();
			if ( _secondaryStructure != nullptr )
			{
				_secondaryStructure->render();
			}
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
			MVC::MvcManager::get().deleteAllModels( _chains );
			MVC::MvcManager::get().deleteAllModels( _residues );

			// Create models.
			_chains.resize( p_molecule.getChainCount() );
			for ( uint i = 0; i < p_molecule.getChainCount(); ++i )
			{
				getChains()[ i ] = MVC::MvcManager::get().instantiateModel<Chain>();
			}
			_residues.resize( p_molecule.getResidueCount() );
			for ( uint i = 0; i < p_molecule.getResidueCount(); ++i )
			{
				getResidues()[ i ] = MVC::MvcManager::get().instantiateModel<Residue>();
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
				MVC::MvcManager::get().deleteModel( _secondaryStructure );
			}

			_secondaryStructure = MVC::MvcManager::get().instantiateModel<SecondaryStructure, Molecule * const>( this );
			_secondaryStructure->init( getBuffer()->gl() );
			_secondaryStructure->print();
		}

		void Molecule::setVisible( const bool p_visible )
		{
			if ( isVisible() != p_visible )
			{
				BaseVisible::setVisible( p_visible );

				_notifyViews( new Event::VTXEvent( Event::Model::MOLECULE_VISIBILITY ) );
			}
		}

	} // namespace Model
} // namespace VTX
