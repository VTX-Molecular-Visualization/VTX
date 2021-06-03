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
			_molecule->refreshColors();
		}

		void BaseRepresentable::computeRepresentationTargets()
		{
			_molecule->_representationTargets.clear();

			Model::SecondaryStructure & secondaryStructure		= _molecule->getSecondaryStructure();
			std::map<uint, uint> & residueToControlPointIndices = secondaryStructure.getResidueToControlPointIndice();

			for ( Model::Residue * const residue : _molecule->getResidues() )
			{
				// Skip hidden items.
				if ( residue == nullptr || !_isResidueVisible( *residue ) )
				{
					continue;
				}

				const InstantiatedRepresentation * const representation = residue->getRepresentation();

				if ( _molecule->_representationTargets.find( representation )
					 == _molecule->_representationTargets.end() )
				{
					_molecule->_representationTargets.emplace( representation, RepresentationTarget() );
				}

				RepresentationTarget & representationTargets = _molecule->_representationTargets[ representation ];
				const VTX::Representation::FlagDataTargeted dataFlag = representation->getFlagDataTargeted();

				if ( (bool)( dataFlag & VTX::Representation::FlagDataTargeted::ATOM ) )
				{
					representationTargets.appendAtoms( residue->getIndexFirstAtom(), residue->getAtomCount() );
				}
				if ( (bool)( dataFlag & VTX::Representation::FlagDataTargeted::BOND ) )
				{
					representationTargets.appendBonds( residue->getIndexFirstBond() * 2, residue->getBondCount() * 2 );
				}
				if ( (bool)( dataFlag & VTX::Representation::FlagDataTargeted::RIBBON ) )
				{
					if ( residueToControlPointIndices.find( residue->getIndex() )
						 != residueToControlPointIndices.end() )
					{
						representationTargets.appendRibbons( residueToControlPointIndices[ residue->getIndex() ], 4 );
					}
				}
			}

			// Compile all targets for gl draw calls.
			for ( std::map<const InstantiatedRepresentation *, RepresentationTarget>::iterator & it
				  = _molecule->_representationTargets.begin();
				  it != _molecule->_representationTargets.end();
				  it++ )
			{
				it->second.compile();
			}

			VTX_DEBUG( "computeRepresentationTargets" );
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
