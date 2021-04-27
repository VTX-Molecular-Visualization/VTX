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
			Representation::RepresentationManager::get().removeInstantiatedRepresentation( *this, false, false );
			_molecule = nullptr;
		}

		void BaseRepresentable::applyRepresentation( InstantiatedRepresentation * const p_representation,
													 const bool							p_recompute,
													 const bool							p_notify )
		{
			setRepresentation( p_representation );

			if ( p_recompute )
				computeAllRepresentationData();

			if ( p_notify )
				_onRepresentationChange();
		}

		void BaseRepresentable::setRepresentation( InstantiatedRepresentation * const p_representation )
		{
			removeRepresentation( false );

			_representation = p_representation;
			p_representation->setTarget( this );
		}

		void BaseRepresentable::removeRepresentation( const bool p_notify )
		{
			if ( _representation != nullptr )
			{
				MVC::MvcManager::get().deleteModel( _representation );
				_representation = nullptr;

				if ( p_notify )
					_onRepresentationChange();
			}
		}

		bool BaseRepresentable::hasParent() { return _parent != nullptr; }

		void BaseRepresentable::setParent( BaseRepresentable * const p_parent ) { _parent = p_parent; }

		bool BaseRepresentable::hasCustomRepresentation() const { return _representation != nullptr; }

		const Model::Representation::InstantiatedRepresentation * const BaseRepresentable::getRepresentation() const
		{
			if ( _representation == nullptr )
			{
				if ( _parent == nullptr )
				{
					return nullptr;
				}
				else
				{
					return _parent->getRepresentation();
				}
			}
			else
			{
				return _representation;
			}
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

			for ( const Model::Residue * const residue : _molecule->getResidues() )
			{
				// Skip hidden items.
				if ( residue == nullptr || !_isResidueVisible( *residue ) )
					continue;

				const Model::Representation::InstantiatedRepresentation * const currentRepresentation
					= residue->getRepresentation();

				COLOR_MODE colorMode = currentRepresentation->getColorMode();

				if ( colorMode == Generic::COLOR_MODE::INHERITED )
				{
					const COLOR_MODE & chainColorMode = residue->getChainPtr()->getRepresentation()->getColorMode();
					if ( chainColorMode != Generic::COLOR_MODE::INHERITED )
					{
						colorMode = chainColorMode;
					}
					else
					{
						const COLOR_MODE & moleculeColorMode = _molecule->getRepresentation()->getColorMode();
						if ( moleculeColorMode != Generic::COLOR_MODE::INHERITED )
							colorMode = moleculeColorMode;
						else
							colorMode = Setting::COLOR_MODE_DEFAULT;
					}
				}

				bool	   colorCarbon = false;
				Color::Rgb color;

				switch ( colorMode )
				{
				case Generic::COLOR_MODE::ATOM_CHAIN: colorCarbon = true; [[fallthrough]];
				case Generic::COLOR_MODE::CHAIN: color = residue->getChainPtr()->getColor(); break;

				case Generic::COLOR_MODE::ATOM_PROTEIN: colorCarbon = true; [[fallthrough]];
				case Generic::COLOR_MODE::PROTEIN: color = _molecule->getColor(); break;

				case Generic::COLOR_MODE::ATOM_CUSTOM: colorCarbon = true; [[fallthrough]];
				case Generic::COLOR_MODE::CUSTOM: color = currentRepresentation->getColor(); break;

				case Generic::COLOR_MODE::RESIDUE:
					colorCarbon = false;
					color		= residue->getColor();
					break;
				}

				for ( uint i = residue->getIndexFirstAtom(); i < residue->getIndexFirstAtom() + residue->getAtomCount();
					  i++ )
				{
					const Model::Atom * const atom = _molecule->getAtom( i );

					if ( atom == nullptr )
						continue;

					if ( colorCarbon && atom->getSymbol() != Model::Atom::SYMBOL::A_C )
					{
						p_colorBuffer[ i ] = atom->getColor();
					}
					else
					{
						p_colorBuffer[ i ] = color;
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
