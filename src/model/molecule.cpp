#include "molecule.hpp"
#include "color/rgb.hpp"
#include "generic/factory.hpp"
#include "model/secondary_structure.hpp"
#include "ui/widget_factory.hpp"
#include "util/molecule.hpp"
#include "util/secondary_structure.hpp"
#include "view/d3/box.hpp"
#include "view/d3/cylinder.hpp"
#include "view/d3/sphere.hpp"
//#include "view/ui/widget/molecule_scene_view.hpp"
#include "id.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Model
	{
		Molecule::Molecule() : BaseModel3D( ID::Model::MODEL_MOLECULE ) {}

		Molecule::~Molecule()
		{
			MVC::MvcManager::get().deleteAllModels( _atoms );
			MVC::MvcManager::get().deleteAllModels( _residues );
			MVC::MvcManager::get().deleteAllModels( _chains );
			MVC::MvcManager::get().deleteAllModels( _bonds );

			if ( _secondaryStructure != nullptr )
			{
				MVC::MvcManager::get().deleteModel( _secondaryStructure );
			}
		}

		void Molecule::_init()
		{
			// Compute global AABB of atom positions (taking into account each frame).
			_computeGlobalPositionsAABB();

			// Fill buffers.
			if ( _atomPositionsFrames.size() > 0 )
			{
				_fillBufferAtomPositions();
				_fillBufferAtomRadius();
				_fillBufferAtomColors();
				_fillBufferAtomVisibilities();
				_fillBufferBonds();

				// Compute secondary structure if not loaded.
				if ( _configuration.isSecondaryStructureLoadedFromFile == false )
				{
					Util::SecondaryStructure::computeSecondaryStructure( *this );
				}

				// Create secondary structure mesh.
				createSecondaryStructure();

				Util::Molecule::refreshRepresentationState( *this );
			}
		}

		void Molecule::instantiateDefaultViews() {}

		void Molecule::_instanciate3DViews()
		{
			MVC::MvcManager::get().addViewOnModel( this, ID::View::D3_SPHERE, new View::D3::Sphere( this ) );
			MVC::MvcManager::get().addViewOnModel( this, ID::View::D3_CYLINDER, new View::D3::Cylinder( this ) );
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
			_fillBufferAtomPositions();

			if ( _secondaryStructure != nullptr )
			{
				_secondaryStructure->setCurrentFrame();
			}
		}

		void Molecule::_fillBufferAtomPositions() { _buffer->setAtomPositions( _atomPositionsFrames[ _currentFrame ] ); }

		void Molecule::_fillBufferAtomRadius()
		{
			_bufferAtomRadius.resize( _atoms.size() );
			for ( uint i = 0; i < uint( _atoms.size() ); ++i )
			{
				_bufferAtomRadius[ i ] = _atoms[ i ]->getVdwRadius();
			}

			_buffer->setAtomRadius( _bufferAtomRadius );
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

			_buffer->setAtomVisibilities( _bufferAtomVisibilities );
		}

		void Molecule::_fillBufferBonds()
		{
			_bufferBonds.resize( _bonds.size() * 2 );
			for ( uint i = 0; i < _bonds.size(); i++ )
			{
				_bufferBonds[ 2u * i ]		= _bonds[ i ]->getIndexFirstAtom();
				_bufferBonds[ 2u * i + 1u ] = _bonds[ i ]->getIndexSecondAtom();
			}

			_buffer->setBonds( _bufferBonds );
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
				getChains()[ i ] = MVC::MvcManager::get().instantiate<Chain>();
			}
			_residues.resize( p_molecule.getResidueCount() );
			for ( uint i = 0; i < p_molecule.getResidueCount(); ++i )
			{
				getResidues()[ i ] = MVC::MvcManager::get().instantiate<Residue>();
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

			_secondaryStructure = MVC::MvcManager::get().instantiate<SecondaryStructure, Molecule * const>( this );
			_secondaryStructure->init( getBuffer()->gl() );
			_secondaryStructure->print();
		}

		void Molecule::toggleSequenceVisibility()
		{
			if ( MVC::MvcManager::get().hasView( this, ID::View::UI_MOLECULE_SEQUENCE ) )
			{
				//	delete MVC::MvcManager::get().removeViewOnModel( this, ID::View::UI_MOLECULE_SEQUENCE );
			}
			else
			{
				//	 MVC::MvcManager::get().addViewOnModel<View::UI::Widget::MoleculeStructure>( this, ID::View::UI_MOLECULE_SEQUENCE );
			}
		}

	} // namespace Model
} // namespace VTX
