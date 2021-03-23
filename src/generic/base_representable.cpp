#include "base_representable.hpp"
#include "model/atom.hpp"
#include "model/chain.hpp"
#include "model/molecule.hpp"
#include "model/representation/representation_library.hpp"
#include "model/residue.hpp"
#include "model/secondary_structure.hpp"
#include "mvc/mvc_manager.hpp"
#include "representation/representation_manager.hpp"
#include "setting.hpp"
#include "vtx_app.hpp"

namespace VTX
{
	namespace Generic
	{
		BaseRepresentable::~BaseRepresentable()
		{
			removeRepresentation();
			_molecule = nullptr;
		}

		void BaseRepresentable::applyRepresentation( InstantiatedRepresentation * const p_representation,
													 const bool							p_recompute )
		{
			setRepresentation( p_representation );
			if ( p_recompute )
				computeAllRepresentationData();
		}

		void BaseRepresentable::setRepresentation( InstantiatedRepresentation * const p_representation )
		{
			removeRepresentation();

			_representation = p_representation;
			p_representation->setTarget( this );
		}

		void BaseRepresentable::removeRepresentation()
		{
			if ( _representation != nullptr )
				MVC::MvcManager::get().deleteModel( _representation );

			_representation = nullptr;
		}

		void BaseRepresentable::applyDefaultRepresentation()
		{
			Model::Representation::InstantiatedRepresentation * const defaultRepresentation
				= Representation::RepresentationManager::get().instantiateDefaultRepresentation();
			defaultRepresentation->setTarget( getMolecule() );

			applyRepresentation( defaultRepresentation );
		}
		void BaseRepresentable::setParent( BaseRepresentable * const p_parent ) { _parent = p_parent; }

		bool BaseRepresentable::hasCustomRepresentation() const { return _representation != nullptr; }

		const Model::Representation::InstantiatedRepresentation * const BaseRepresentable::getRepresentation() const
		{
			if ( _representation == nullptr )
				return _parent->getRepresentation();
			else
				return _representation;
		}
		Model::Representation::InstantiatedRepresentation * const BaseRepresentable::getCustomRepresentation()
		{
			return _representation;
		}

		void BaseRepresentable::computeAllRepresentationData()
		{
			computeRepresentationTargets();
			computeColorBuffer();
			_molecule->getSecondaryStructure().refreshColors();
		}

		void BaseRepresentable::computeRepresentationTargets()
		{
			_molecule->_representationTargets.clear();

			const Model::SecondaryStructure & secondaryStructure = _molecule->getSecondaryStructure();
			const std::map<uint, uint>		  residueToControlPointIndices
				= secondaryStructure.getResidueToControlPointIndice();

			for ( Model::Residue * const residue : _molecule->getResidues() )
			{
				// Skip hidden items.
				if ( residue == nullptr || !_isResidueVisible( *residue ) )
					continue;

				const Model::Representation::InstantiatedRepresentation * const representation
					= residue->getRepresentation();

				if ( _molecule->_representationTargets.find( representation )
					 == _molecule->_representationTargets.end() )
					_molecule->_representationTargets.emplace( representation,
															   VTX::Representation::RepresentationTarget() );

				VTX::Representation::RepresentationTarget & representationTargets
					= _molecule->_representationTargets.at( representation );
				const VTX::Representation::FlagDataTargeted dataFlag = representation->getFlagDataTargeted();

				if ( (bool)( dataFlag & VTX::Representation::FlagDataTargeted::ATOM ) )
				{
					const uint			  nextAtom	 = residue->getIndexFirstAtom() + residue->getAtomCount();
					std::pair<uint, uint> rangeAtoms = std::pair( residue->getIndexFirstAtom(), 0 );
					for ( uint i = residue->getIndexFirstAtom(); i < nextAtom; i++ )
					{
						if ( _molecule->getAtom( i ) == nullptr || !_molecule->getAtom( i )->isVisible() )
						{
							if ( rangeAtoms.second > 0 )
								representationTargets.appendAtoms( rangeAtoms );
							rangeAtoms.first  = i + 1;
							rangeAtoms.second = 0;
						}
						else
						{
							rangeAtoms.second++;
						}
					}

					if ( rangeAtoms.second > 0 )
						representationTargets.appendAtoms( rangeAtoms );
				}
				if ( (bool)( dataFlag & VTX::Representation::FlagDataTargeted::BOND ) )
				{
					const std::pair<uint, uint> rangeBonds
						= std::pair( residue->getIndiceFirstBond(), residue->getBondIndiceCount() );

					representationTargets.appendBonds(
						rangeBonds, residue->getIndexExtraBondStart(), residue->getIndexExtraBondEnd() );
				}
				if ( (bool)( dataFlag & VTX::Representation::FlagDataTargeted::RIBBON ) )
				{
					std::pair<uint, uint> rangeRibbons = std::pair( 0, 0 );
					if ( residueToControlPointIndices.find( residue->getIndex() )
						 != residueToControlPointIndices.end() )
						rangeRibbons = std::pair( residueToControlPointIndices.at( residue->getIndex() ), 4 );

					representationTargets.appendRibbons( rangeRibbons );
				}
			}
		}

		void BaseRepresentable::computeColorBuffer()
		{
			std::vector<Color::Rgb> p_colorBuffer = _molecule->getBufferAtomColors();

			if ( p_colorBuffer.size() == 0 )
				return;

			for ( Model::Residue * const residue : _molecule->getResidues() )
			{
				// Skip hidden items.
				if ( residue == nullptr || !_isResidueVisible( *residue ) )
					continue;

				const Model::Representation::InstantiatedRepresentation * const currentRepresentation
					= residue->getRepresentation();

				for ( uint i = residue->getIndexFirstAtom(); i < residue->getIndexFirstAtom() + residue->getAtomCount();
					  i++ )
				{
					const Model::Atom * const atom = _molecule->getAtom( i );

					if ( atom == nullptr )
						continue;

					switch ( currentRepresentation->getColorMode() )
					{
					case Generic::COLOR_MODE::ATOM_CHAIN:
						if ( atom->getSymbol() == Model::Atom::SYMBOL::A_C )
							p_colorBuffer[ i ] = atom->getChainPtr()->getColor();
						else
							p_colorBuffer[ i ] = atom->getColor();
						break;
					case Generic::COLOR_MODE::ATOM_PROTEIN:
						if ( atom->getSymbol() == Model::Atom::SYMBOL::A_C )
							p_colorBuffer[ i ] = _molecule->getColor();
						else
							p_colorBuffer[ i ] = atom->getColor();
						break;
					case Generic::COLOR_MODE::ATOM_CUSTOM:
						if ( atom->getSymbol() == Model::Atom::SYMBOL::A_C )
							p_colorBuffer[ i ] = currentRepresentation->getColor();
						else
							p_colorBuffer[ i ] = atom->getColor();
						break;
					case Generic::COLOR_MODE::RESIDUE: p_colorBuffer[ i ] = atom->getResiduePtr()->getColor(); break;
					case Generic::COLOR_MODE::CHAIN: p_colorBuffer[ i ] = atom->getChainPtr()->getColor(); break;
					case Generic::COLOR_MODE::PROTEIN: p_colorBuffer[ i ] = _molecule->getColor(); break;
					case Generic::COLOR_MODE::CUSTOM: p_colorBuffer[ i ] = currentRepresentation->getColor(); break;

					default: break;
					}
				}
			}

			_molecule->getBuffer()->setAtomColors( p_colorBuffer );
		}

		bool BaseRepresentable::_isResidueVisible( const Model::Residue & p_residue ) const
		{
			const Model::Molecule * const molecule = p_residue.getMoleculePtr();

			// Skip hidden items.
			if ( p_residue.isVisible() == false || p_residue.getChainPtr()->isVisible() == false
				 || molecule->isVisible() == false )
				return false;

			const Model::Atom::TYPE atomType = p_residue.getAtomType();

			if ( molecule->showSolvent() == false && atomType == Model::Atom::TYPE::SOLVENT )
				return false;

			if ( molecule->showIon() == false && atomType == Model::Atom::TYPE::ION )
				return false;

			return true;
		}

	} // namespace Generic
} // namespace VTX
