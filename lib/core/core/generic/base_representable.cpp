#include "base_representable.hpp"
#include "manager/mvc/mvc_manager.hpp"
#include "model/chemistry/atom.hpp"
#include "model/chemistry/chain.hpp"
#include "model/chemistry/molecule.hpp"
#include "model/chemistry/residue.hpp"
#include "model/chemistry/secondary_structure.hpp"
#include "model/chemistry/solvent_excluded_surface.hpp"
#include "model/representation/representation_library.hpp"
#include "model/representation/representation_manager.hpp"

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
			if ( hasCustomRepresentation() )
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

		bool BaseRepresentable::hasParent() const { return _parent != nullptr; }

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
					if ( _molecule->hasSecondaryStructure() == false )
					{
						_molecule->createSecondaryStructure();
					}

					Model::SecondaryStructure & secondaryStructure = _molecule->getSecondaryStructure();
					std::map<uint, uint> &		residueToControlPointIndices
						= secondaryStructure.getResidueToControlPointIndice();
					if ( residueToControlPointIndices.find( residue->getIndex() )
						 != residueToControlPointIndices.end() )
					{
						representationTargets.appendRibbons( residueToControlPointIndices[ residue->getIndex() ], 4 );
					}
				}
				if ( (bool)( dataFlag & VTX::Representation::FlagDataTargeted::SES ) )
				{
					if ( _molecule->hasSolventExcludedSurface() == false )
					{
						_molecule->createSolventExcludedSurface();
					}

					const Model::SolventExcludedSurface &	   ses = _molecule->getSolventExcludedSurface();
					const std::vector<std::pair<uint, uint>> & atomsToTriangles = ses.getAtomsToTriangles();
					for ( uint atomIdx = residue->getIndexFirstAtom();
						  atomIdx < residue->getIndexFirstAtom() + residue->getAtomCount();
						  ++atomIdx )
					{
						representationTargets.appendTrianglesSES( atomsToTriangles[ atomIdx ].first,
																  atomsToTriangles[ atomIdx ].second );
					}
				}
			}

			// Compile all targets for gl draw calls.
			for ( auto & pair : _molecule->_representationTargets )
			{
				pair.second.generate();
			}
		}

		void BaseRepresentable::refreshRepresentationTargets()
		{
			if ( _molecule == nullptr )
				return;

			for ( auto representationTargetPair : _molecule->_representationTargets )
			{
				const VTX::Representation::FlagDataTargeted dataFlag
					= representationTargetPair.first->getFlagDataTargeted();

				// SES need to recompute the mapping between atoms and triangles
				if ( (bool)( dataFlag & VTX::Representation::FlagDataTargeted::SES ) )
				{
					RepresentationTarget & representationTargets
						= _molecule->_representationTargets[ representationTargetPair.first ];
					representationTargets.resetTriangleSES();
				}
			}

			// TODO finish that (/!\ triangleSESMap is clear at the end of computation. Find a way to manage that
			for ( const Model::Residue * const residue : _molecule->getResidues() )
			{
				// Skip hidden items.
				if ( residue == nullptr || !_isResidueVisible( *residue ) )
				{
					continue;
				}

				const InstantiatedRepresentation * const representation = residue->getRepresentation();
				RepresentationTarget & representationTargets = _molecule->_representationTargets[ representation ];
				const VTX::Representation::FlagDataTargeted dataFlag = representation->getFlagDataTargeted();

				if ( (bool)( dataFlag & VTX::Representation::FlagDataTargeted::SES ) )
				{
					const Model::SolventExcludedSurface & ses = _molecule->getSolventExcludedSurface();

					const std::vector<std::pair<uint, uint>> & atomsToTriangles = ses.getAtomsToTriangles();
					for ( uint atomIdx = residue->getIndexFirstAtom();
						  atomIdx < residue->getIndexFirstAtom() + residue->getAtomCount();
						  ++atomIdx )
					{
						representationTargets.appendTrianglesSES( atomsToTriangles[ atomIdx ].first,
																  atomsToTriangles[ atomIdx ].second );
					}
				}
			}

			for ( auto representationTargetPair : _molecule->_representationTargets )
			{
				// Generate does nothing if the TargetRange has not been reset before.
				_molecule->_representationTargets[ representationTargetPair.first ].generate();
			}
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
