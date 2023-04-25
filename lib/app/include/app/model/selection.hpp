#ifndef __VTX_MODEL_SELECTION__
#define __VTX_MODEL_SELECTION__

#include "app/core/event/base_event_receiver_vtx.hpp"
#include "app/core/event/vtx_event.hpp"
#include "app/event/global.hpp"
#include "app/model/base_model.hpp"
#include "app/model/category_enum.hpp"
#include "app/mvc.hpp"
#include "app/old_app/object3d/helper/aabb.hpp"
#include <map>
#include <set>
#include <unordered_set>
#include <util/types.hpp>
#include <vector>

namespace VTX::Model
{
	class Molecule;
	class Category;
	class Chain;
	class Residue;
	class Atom;

	class Selection : public BaseModel, public VTX::App::Core::Event::BaseEventReceiverVTX
	{
		VTX_MODEL

	  public:
		class VecAtomIds : public std::vector<uint>
		{
			friend Selection;

		  public:
			uint getFullySelectedChildCount() const { return _childrenFullySelectedCount; }

		  private:
			void _setFullChildrenCount( const uint p_count ) { _childrenFullySelectedCount = p_count; };
			void _addFullChild() { _childrenFullySelectedCount++; };
			void _removeFullChild() { _childrenFullySelectedCount--; };

			uint _childrenFullySelectedCount = 0;
		};
		class MapResidueIds : public std::map<VTX::Model::ID, VecAtomIds>
		{
			friend Selection;

		  public:
			uint getFullySelectedChildCount() const { return _childrenFullySelectedCount; }

		  private:
			void _setFullChildrenCount( const uint p_count ) { _childrenFullySelectedCount = p_count; };
			void _addFullChild() { _childrenFullySelectedCount++; };
			void _removeFullChild() { _childrenFullySelectedCount--; };

		  private:
			uint _childrenFullySelectedCount = 0;
		};
		class MapChainIds : public std::map<VTX::Model::ID, MapResidueIds>
		{
			friend Selection;

		  public:
			uint getFullySelectedChildCount() const { return _childrenFullySelectedCount; }

		  private:
			void _setFullChildrenCount( const uint p_count ) { _childrenFullySelectedCount = p_count; };
			void _addFullChild() { _childrenFullySelectedCount++; };
			void _removeFullChild() { _childrenFullySelectedCount--; };

			uint _childrenFullySelectedCount = 0;
		};
		using MapMoleculeIds = std::map<VTX::Model::ID, MapChainIds>;

		using PairResidueIds  = std::pair<const uint, VecAtomIds>;
		using PairChainIds	  = std::pair<const uint, MapResidueIds>;
		using PairMoleculeIds = std::pair<const VTX::Model::ID, MapChainIds>;

		inline const std::set<VTX::Model::ID> & getItems() const { return _items; }
		inline std::set<VTX::Model::ID> &		getItems() { return _items; }

		inline const MapMoleculeIds & getMoleculesMap() const { return _moleculesMap; }
		inline MapMoleculeIds &		  getMoleculesMap() { return _moleculesMap; }
		const MapChainIds * const	  getMoleculeMap( const Molecule & );

		bool hasItemOfType( const VTX::ID::VTX_ID & p_id ) const;

		bool hasMolecule() const;
		void selectMolecule( Molecule &, const bool p_appendToSelection = false );
		void selectMolecules( const std::vector<Molecule *> &,
							  const bool			 p_appendToSelection = false,
							  const Molecule * const p_currentObj		 = nullptr );
		void unselectMolecule( Molecule & );
		void unselectMolecules( const std::vector<Molecule *> & );
		void unselectMoleculesWithCheck( const std::vector<Molecule *> & );
		bool isMoleculeSelected( const Molecule & ) const;
		bool isMoleculeFullySelected( const Molecule & ) const;
		uint getMoleculeSelectedCount() const;

		void selectChain( Chain &, const bool p_appendToSelection = false );
		void selectChains( const std::vector<Chain *> &,
						   const bool		   p_appendToSelection = false,
						   const Chain * const p_currentObj		   = nullptr );
		void unselectChain( Chain & );
		void unselectChains( const std::vector<Chain *> & );
		void unselectChainsWithCheck( const std::vector<Chain *> & );
		bool isChainSelected( const Chain & ) const;
		bool isChainFullySelected( const Chain & ) const;
		uint getChainSelectedCount() const;

		void selectResidue( Residue &, const bool p_appendToSelection = false );
		void selectResidues( const std::vector<Residue *> &,
							 const bool			   p_appendToSelection = false,
							 const Residue * const p_currentObj		   = nullptr );
		void unselectResidue( Residue & );
		void unselectResidues( const std::vector<Residue *> & );
		void unselectResiduesWithCheck( const std::vector<Residue *> & );
		bool isResidueSelected( const Residue & ) const;
		bool isResidueFullySelected( const Residue & ) const;
		uint getResidueSelectedCount() const;

		void selectAtom( Atom &, const bool p_appendToSelection = false );
		void selectAtoms( const std::vector<Atom *> &,
						  const bool		 p_appendToSelection = false,
						  const Atom * const p_currentObj		 = nullptr );
		void unselectAtom( Atom & );
		void unselectAtoms( const std::vector<Atom *> & );
		void unselectAtomsWithCheck( const std::vector<Atom *> & );
		bool isAtomSelected( const Atom & ) const;
		uint getAtomSelectedCount() const;

		void selectCategory( Category &, const bool p_appendToSelection = false );
		void selectCategories( const std::vector<Category *> &,
							   const bool			  p_appendToSelection = false,
							   const Category * const p_currentObj		  = nullptr );
		void unselectCategory( Category & );
		void unselectCategories( const std::vector<Category *> & );
		void unselectCategoriesWithCheck( const std::vector<Category *> & );
		bool isCategorySelected( const Model::Category & ) const;
		bool isCategoryFullySelected( const Model::Category & ) const;

		void selectModels( const std::vector<Model::Molecule *> & p_molecules,
						   const std::vector<Model::Category *> & p_categories,
						   const std::vector<Model::Chain *> &	  p_chains,
						   const std::vector<Model::Residue *> &  p_residues,
						   const std::vector<Model::Atom *> &	  p_atoms,
						   const bool							  p_appendToSelection = false,
						   const Model::BaseModel * const		  p_currentObj		  = nullptr );
		void unselectModels( const std::vector<Model::Molecule *> & p_molecules,
							 const std::vector<Model::Category *> & p_categories,
							 const std::vector<Model::Chain *> &	p_chains,
							 const std::vector<Model::Residue *> &	p_residus,
							 const std::vector<Model::Atom *> &		p_atoms );

		void selectModel( Model::BaseModel & p_model, const bool p_appendToSelection = false );
		void unselectModel( const Model::BaseModel & );
		bool isModelSelected( const Model::BaseModel & ) const;
		bool isModelSelected( const Model::ID & ) const;

		template<typename T, typename = std::enable_if<std::is_base_of<Model::BaseModel, T>::value>>
		void selectModels( const std::vector<T *> & p_models, const bool p_appendToSelection = false )
		{
			if ( !p_appendToSelection )
				_clearWithoutNotify();

			if ( p_models.empty() )
				return;

			for ( T * const model : p_models )
				_items.emplace( model->getId() );

			_setCurrentObject( *p_models.crbegin() );

			_notifyDataChanged();
		}
		template<typename T, typename = std::enable_if<std::is_base_of<Model::BaseModel, T>::value>>
		void unselectModels( const std::vector<T *> & p_models )
		{
			if ( p_models.size() == 0 )
				return;

			for ( const auto it : p_models )
				_unselectModel( *it );

			_notifyDataChanged();
		}
		template<typename T, typename = std::enable_if<std::is_base_of<Model::BaseModel, T>::value>>
		void unselectModelsWithCheck( const std::vector<T *> & p_models )
		{
			if ( p_models.size() == 0 )
				return;

			for ( const auto it : p_models )
			{
				if ( isModelSelected( *it ) )
					_unselectModel( *it );
			}

			_notifyDataChanged();
		}

		template<typename T, typename = std::enable_if<std::is_base_of<Model::BaseModel, T>::value>>
		std::vector<T *> getItemsOfType( const VTX::ID::VTX_ID & p_modelTypeID )
		{
			std::vector<T *> models = std::vector<T *>();
			for ( const VTX::Model::ID & modelID : getItems() )
			{
				if ( VTX::MVC_MANAGER().getModelTypeID( modelID ) == p_modelTypeID )
				{
					T & model = VTX::MVC_MANAGER().getModel<T>( modelID );
					models.emplace_back( &model );
				}
			}

			return models;
		};

		bool isEmpty() const;
		void clear();

		void moveDataTo( Selection & p_target );

		void receiveEvent( const App::Core::Event::VTXEvent & p_event ) override;

		void						   getItemTypes( std::set<VTX::ID::VTX_ID> & p_types ) const;
		Object3D::Helper::AABB		   getAABB() const;
		const Model::BaseModel * const getCurrentObject() const;

		template<typename T, typename = std::enable_if<std::is_base_of<Model::BaseModel, T>::value>>
		void getItemsOfType( const VTX::ID::VTX_ID & p_itemType, std::set<T *> & p_items ) const
		{
			for ( const VTX::Model::ID & itemID : _items )
			{
				if ( VTX::MVC_MANAGER().getModelTypeID( itemID ) == p_itemType )
				{
					T & item = VTX::MVC_MANAGER().getModel<T>( itemID );
					p_items.emplace( &item );
				}
			}
		}
		template<typename T, typename = std::enable_if<std::is_base_of<Model::BaseModel, T>::value>>
		void getItemsOfType( const VTX::ID::VTX_ID & p_itemType, std::unordered_set<T *> & p_items ) const
		{
			for ( const VTX::Model::ID & itemID : _items )
			{
				if ( VTX::MVC_MANAGER().getModelTypeID( itemID ) == p_itemType )
				{
					T & item = VTX::MVC_MANAGER().getModel<T>( itemID );
					p_items.emplace( &item );
				}
			}
		}

	  protected:
		Selection() : BaseModel( VTX::ID::Model::MODEL_SELECTION )
		{
			_registerEvent( App::Event::Global::MOLECULE_REMOVED );
		}
		~Selection() = default;

		void _notifyDataChanged();

	  private:
		std::set<VTX::Model::ID> _items		   = std::set<VTX::Model::ID>();
		MapMoleculeIds			 _moleculesMap = MapMoleculeIds();

		std::map<VTX::Model::ID, Object3D::Helper::AABB> _mapSelectionAABB
			= std::map<VTX::Model::ID, Object3D::Helper::AABB>();
		const Model::BaseModel * _currentObject = nullptr;

		void _selectMolecule( const Molecule & );
		void _unselectMolecule( const Molecule & );
		void _selectCategory( const Category & );
		void _unselectCategory( const Category & );
		void _selectChain( const Chain & );
		void _unselectChain( const Chain & );
		void _selectResidue( const Residue & );
		void _unselectResidue( const Residue & );
		void _selectAtom( const Atom & );
		void _unselectAtom( const Atom & );

		void _unselectModel( const Model::BaseModel & );

		bool _addMolecule( const Molecule & );
		bool _addChain( const Chain & );
		bool _addResidue( const Residue & );
		bool _addAtom( const Atom & );

		void _addMoleculeContent( const Molecule & );
		void _addChainContent( const Chain & );
		void _addResidueContent( const Residue & );

		void _removeMolecule( const Molecule & );
		void _removeChain( const Chain & );
		void _removeResidue( const Residue & );
		void _removeAtom( const Atom & );

		void _clearWithoutNotify();

		void _refreshMoleculeSelection( Molecule * const );
		void _recomputeAABB();

		void _referenceAtom( const Atom & p_atom );
		void _referenceFullResidue( const Residue & p_residue );
		void _referenceFullChain( const Chain & p_chain );

		void _unreferenceAtom( const Atom & p_atom );
		void _unreferenceFullResidue( const Residue & p_residue );
		void _unreferenceFullChain( const Chain & p_chain );

		void _emplaceMolecule( const Molecule & );

		void _setCurrentObject( const Model::BaseModel * const p_model, const bool p_notify = true );
		void _clearCurrentObject( const bool p_notify = true );
	};

} // namespace VTX::Model
#endif
