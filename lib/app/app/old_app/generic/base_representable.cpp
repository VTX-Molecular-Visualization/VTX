#include "base_representable.hpp"
#include "old_app/model/atom.hpp"
#include "old_app/model/category.hpp"
#include "old_app/model/chain.hpp"
#include "old_app/model/molecule.hpp"
#include "old_app/model/representation/representation_library.hpp"
#include "old_app/model/residue.hpp"
#include "old_app/model/secondary_structure.hpp"
#include "old_app/model/solvent_excluded_surface.hpp"
#include "old_app/mvc/mvc_manager.hpp"
#include "old_app/representation/representation_manager.hpp"
#include "old_app/setting.hpp"
#include "old_app/vtx_app.hpp"

namespace VTX
{
	namespace Generic
	{
		BaseRepresentable::~BaseRepresentable()
		{
			Representation::RepresentationManager::get().removeInstantiatedRepresentation( *this, false, false );
			_molecule = nullptr;
		}

		void BaseRepresentable::initBaseRepresentable( Model::BaseModel * const			  p_model,
													   Generic::BaseRepresentable * const p_parent,
													   Model::Molecule * const			  p_molecule )
		{
			_model = p_model;
			setParent( p_parent );
			setRepresentableMolecule( p_molecule );
		}

		void BaseRepresentable::_linkRepresentationToParent() const
		{
			Generic::BaseRepresentable * parent = getParent();
			while ( parent != nullptr )
			{
				parent->_subRepresentations.emplace( _representation );
				parent = parent->getParent();
			}
		}
		void BaseRepresentable::_delinkRepresentationToParent() const
		{
			Generic::BaseRepresentable * parent = getParent();
			while ( parent != nullptr )
			{
				parent->_subRepresentations.erase( _representation );
				parent = parent->getParent();
			}
		}

		void BaseRepresentable::applyRepresentation( InstantiatedRepresentation * const p_representation,
													 const bool							p_recompute,
													 const bool							p_notify )
		{
			setRepresentation( p_representation );

			if ( p_recompute )
				computeAllRepresentationData();

			if ( p_notify )
				_callRepresentationChange();
		}

		void BaseRepresentable::setRepresentation( InstantiatedRepresentation * const p_representation )
		{
			if ( hasCustomRepresentation() )
				removeRepresentation( false );

			_representation = p_representation;
			p_representation->setTarget( this );

			_linkRepresentationToParent();
		}

		void BaseRepresentable::removeRepresentation( const bool p_notify )
		{
			if ( _representation != nullptr )
			{
				_delinkRepresentationToParent();

				MVC::MvcManager::get().deleteModel( _representation );
				_representation = nullptr;

				if ( p_notify )
					_callRepresentationChange();
			}
		}

		bool BaseRepresentable::hasParent() const { return _parent != nullptr; }

		void BaseRepresentable::setParent( BaseRepresentable * const p_parent )
		{
			if ( _parent != p_parent && _parent != nullptr && _representation != nullptr )
			{
				_delinkRepresentationToParent();
				_parent = p_parent;
				_linkRepresentationToParent();
			}
			else
			{
				_parent = p_parent;
			}
		}

		bool BaseRepresentable::hasCustomRepresentation() const { return _representation != nullptr; }

		const std::set<Model::Representation::InstantiatedRepresentation *> BaseRepresentable::getSubRepresentations()
			const
		{
			return _subRepresentations;
		}

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
					const Model::Category * const category = _molecule->getCategoryFromChain( *residue->getChainPtr() );
					const CATEGORY_ENUM			  categoryEnum = category->getCategoryEnum();

					if ( categoryEnum == CATEGORY_ENUM::POLYMER || categoryEnum == CATEGORY_ENUM::CARBOHYDRATE )
					{
						if ( _molecule->hasSolventExcludedSurface( categoryEnum ) == false )
						{
							_molecule->createSolventExcludedSurface( categoryEnum );
						}

						const Model::SolventExcludedSurface & ses
							= _molecule->getSolventExcludedSurface( categoryEnum );
						const std::vector<Model::SolventExcludedSurface::Range> & atomsToTriangles
							= ses.getAtomsToTriangles();
						for ( uint atomIdx = residue->getIndexFirstAtom();
							  atomIdx < residue->getIndexFirstAtom() + residue->getAtomCount();
							  ++atomIdx )
						{
							representationTargets.appendTrianglesSES(
								categoryEnum, atomsToTriangles[ atomIdx ].first, atomsToTriangles[ atomIdx ].count );
						}
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

			for ( const auto & representationTargetPair : _molecule->_representationTargets )
			{
				const VTX::Representation::FlagDataTargeted dataFlag
					= representationTargetPair.first->getFlagDataTargeted();

				// SES need to recompute the mapping between atoms and triangles
				if ( (bool)( dataFlag & VTX::Representation::FlagDataTargeted::SES ) )
				{
					RepresentationTarget & representationTargets
						= _molecule->_representationTargets[ representationTargetPair.first ];
					representationTargets.resetTriangleSES();

					for ( const Model::Residue * const residue : _molecule->getResidues() )
					{
						// Skip hidden items.
						if ( residue == nullptr || !_isResidueVisible( *residue ) )
						{
							continue;
						}

						const InstantiatedRepresentation * const representation = residue->getRepresentation();
						RepresentationTarget &					 representationTargets
							= _molecule->_representationTargets[ representation ];

						const Model::Category * const category
							= _molecule->getCategoryFromChain( *residue->getChainPtr() );
						const CATEGORY_ENUM categoryEnum = category->getCategoryEnum();

						if ( !_molecule->hasSolventExcludedSurface( categoryEnum ) )
							continue;

						const Model::SolventExcludedSurface & ses
							= _molecule->getSolventExcludedSurface( categoryEnum );

						const std::vector<Model::SolventExcludedSurface::Range> & atomsToTriangles
							= ses.getAtomsToTriangles();
						for ( uint atomIdx = residue->getIndexFirstAtom();
							  atomIdx < residue->getIndexFirstAtom() + residue->getAtomCount();
							  ++atomIdx )
						{
							representationTargets.appendTrianglesSES(
								categoryEnum, atomsToTriangles[ atomIdx ].first, atomsToTriangles[ atomIdx ].count );
						}
					}

					_molecule->_representationTargets[ representationTargetPair.first ].generate();
				}
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

		void BaseRepresentable::_callRepresentationChange( const bool p_callChangeOnParents )
		{
			_onRepresentationChange();

			if ( p_callChangeOnParents )
			{
				BaseRepresentable * parent = getParent();

				while ( parent != nullptr )
				{
					parent->_onRepresentationChange();
					parent = parent->getParent();
				}
			}
		}

	} // namespace Generic
} // namespace VTX
