#include "app/application/representation/base_representable.hpp"
#include "app/application/representation/enum_representation.hpp"
#include "app/application/representation/instantiated_representation.hpp"
#include "app/application/representation/representation_library.hpp"
#include "app/application/representation/representation_manager.hpp"
#include "app/component/chemistry/atom.hpp"
#include "app/component/chemistry/category.hpp"
#include "app/component/chemistry/chain.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/component/chemistry/residue.hpp"
#include "app/component/chemistry/secondary_structure.hpp"
#include "app/component/chemistry/solvent_excluded_surface.hpp"
#include "app/mvc.hpp"
#include "app/application/setting.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Application::Representation
{
	BaseRepresentable::~BaseRepresentable()
	{
		RepresentationManager::get().removeInstantiatedRepresentation( *this, false, false );
		_molecule = nullptr;
	}

	void BaseRepresentable::initBaseRepresentable( Core::Model::BaseModel * const		  p_model,
												   BaseRepresentable * const			  p_parent,
												   Component::Chemistry::Molecule * const p_molecule )
	{
		_model = p_model;
		setParent( p_parent );
		setRepresentableMolecule( p_molecule );
	}

	void BaseRepresentable::_linkRepresentationToParent() const
	{
		BaseRepresentable * parent = getParent();
		while ( parent != nullptr )
		{
			parent->_subRepresentations.emplace( _representation );
			parent = parent->getParent();
		}
	}
	void BaseRepresentable::_delinkRepresentationToParent() const
	{
		BaseRepresentable * parent = getParent();
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

			VTX::MVC_MANAGER().deleteModel( _representation );
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

	const std::set<InstantiatedRepresentation *> BaseRepresentable::getSubRepresentations() const
	{
		return _subRepresentations;
	}

	const InstantiatedRepresentation * const BaseRepresentable::getRepresentation() const
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
	InstantiatedRepresentation * const BaseRepresentable::getCustomRepresentation() { return _representation; }

	void BaseRepresentable::computeAllRepresentationData()
	{
		computeRepresentationTargets();
		_molecule->refreshColors();
	}

	void BaseRepresentable::computeRepresentationTargets()
	{
		_molecule->_representationTargets.clear();

		for ( App::Component::Chemistry::Residue * const residue : _molecule->getResidues() )
		{
			// Skip hidden items.
			if ( residue == nullptr || !_isResidueVisible( *residue ) )
			{
				continue;
			}

			const InstantiatedRepresentation * const representation = residue->getRepresentation();

			if ( _molecule->_representationTargets.find( representation ) == _molecule->_representationTargets.end() )
			{
				_molecule->_representationTargets.emplace( representation, RepresentationTarget() );
			}

			RepresentationTarget &	 representationTargets = _molecule->_representationTargets[ representation ];
			const TARGETED_DATA_FLAG dataFlag			   = representation->getFlagDataTargeted();

			if ( dataFlag & TARGETED_DATA_FLAG::ENUM::ATOM )
			{
				representationTargets.appendAtoms( residue->getIndexFirstAtom(), residue->getAtomCount() );
			}
			if ( dataFlag & TARGETED_DATA_FLAG::ENUM::BOND )
			{
				representationTargets.appendBonds( residue->getIndexFirstBond() * 2, residue->getBondCount() * 2 );
			}
			if ( dataFlag & TARGETED_DATA_FLAG::ENUM::RIBBON )
			{
				if ( _molecule->hasSecondaryStructure() == false )
				{
					_molecule->createSecondaryStructure();
				}

				App::Component::Chemistry::SecondaryStructure & secondaryStructure = _molecule->getSecondaryStructure();
				std::map<uint, uint> &							residueToControlPointIndices
					= secondaryStructure.getResidueToControlPointIndice();
				if ( residueToControlPointIndices.find( residue->getIndex() ) != residueToControlPointIndices.end() )
				{
					representationTargets.appendRibbons( residueToControlPointIndices[ residue->getIndex() ], 4 );
				}
			}
			if ( dataFlag & TARGETED_DATA_FLAG::ENUM::SES )
			{
				const App::Component::Chemistry::Category * const category
					= _molecule->getCategoryFromChain( *residue->getChainPtr() );
				const VTX::Core::ChemDB::Category::TYPE categoryEnum = category->getCategoryEnum();

				if ( categoryEnum == VTX::Core::ChemDB::Category::TYPE::POLYMER
					 || categoryEnum == VTX::Core::ChemDB::Category::TYPE::CARBOHYDRATE )
				{
					if ( _molecule->hasSolventExcludedSurface( categoryEnum ) == false )
					{
						_molecule->createSolventExcludedSurface( categoryEnum );
					}

					const App::Component::Chemistry::SolventExcludedSurface & ses
						= _molecule->getSolventExcludedSurface( categoryEnum );
					const std::vector<App::Component::Chemistry::SolventExcludedSurface::Range> & atomsToTriangles
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
			const TARGETED_DATA_FLAG dataFlag = representationTargetPair.first->getFlagDataTargeted();

			// SES need to recompute the mapping between atoms and triangles
			if ( dataFlag & TARGETED_DATA_FLAG::ENUM::SES )
			{
				RepresentationTarget & representationTargets
					= _molecule->_representationTargets[ representationTargetPair.first ];
				representationTargets.resetTriangleSES();

				for ( const App::Component::Chemistry::Residue * const residue : _molecule->getResidues() )
				{
					// Skip hidden items.
					if ( residue == nullptr || !_isResidueVisible( *residue ) )
					{
						continue;
					}

					const InstantiatedRepresentation * const representation = residue->getRepresentation();
					RepresentationTarget & representationTargets = _molecule->_representationTargets[ representation ];

					const App::Component::Chemistry::Category * const category
						= _molecule->getCategoryFromChain( *residue->getChainPtr() );
					const VTX::Core::ChemDB::Category::TYPE categoryEnum = category->getCategoryEnum();

					if ( !_molecule->hasSolventExcludedSurface( categoryEnum ) )
						continue;

					const App::Component::Chemistry::SolventExcludedSurface & ses
						= _molecule->getSolventExcludedSurface( categoryEnum );

					const std::vector<App::Component::Chemistry::SolventExcludedSurface::Range> & atomsToTriangles
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

	bool BaseRepresentable::_isResidueVisible( const App::Component::Chemistry::Residue & p_residue ) const
	{
		const App::Component::Chemistry::Molecule * const molecule = p_residue.getMoleculePtr();

		// Skip hidden items.
		if ( p_residue.isVisible() == false || p_residue.getChainPtr()->isVisible() == false
			 || molecule->isVisible() == false )
			return false;

		const VTX::Core::ChemDB::Atom::TYPE atomType = p_residue.getAtomType();

		if ( molecule->showSolvent() == false && atomType == VTX::Core::ChemDB::Atom::TYPE::SOLVENT )
			return false;

		if ( molecule->showIon() == false && atomType == VTX::Core::ChemDB::Atom::TYPE::ION )
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
} // namespace VTX::App::Application::Representation
