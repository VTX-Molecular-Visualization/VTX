#ifndef __VTX_APP_APPLICATION_SELECTION_SELECTION__
#define __VTX_APP_APPLICATION_SELECTION_SELECTION__

#include "app/component/chemistry/_fwd.hpp"
#include "app/component/object3d/helper/aabb.hpp"
#include "app/core/event/base_event_receiver_vtx.hpp"
#include "app/core/event/vtx_event.hpp"
#include "app/core/model/base_model.hpp"
#include "app/event/global.hpp"
#include "app/mvc.hpp"
#include <map>
#include <set>
#include <unordered_set>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Application::Selection
{
	class SelectionModel : public App::Core::Model::BaseModel, public VTX::App::Core::Event::BaseEventReceiverVTX
	{
		VTX_MODEL

	  public:
		class VecAtomIds : public std::vector<uint>
		{
			friend SelectionModel;

		  public:
			uint getFullySelectedChildCount() const { return _childrenFullySelectedCount; }

		  private:
			void _setFullChildrenCount( const uint p_count ) { _childrenFullySelectedCount = p_count; };
			void _addFullChild() { _childrenFullySelectedCount++; };
			void _removeFullChild() { _childrenFullySelectedCount--; };

			uint _childrenFullySelectedCount = 0;
		};
		class MapResidueIds : public std::map<VTX::App::Core::Model::ID, VecAtomIds>
		{
			friend SelectionModel;

		  public:
			uint getFullySelectedChildCount() const { return _childrenFullySelectedCount; }

		  private:
			void _setFullChildrenCount( const uint p_count ) { _childrenFullySelectedCount = p_count; };
			void _addFullChild() { _childrenFullySelectedCount++; };
			void _removeFullChild() { _childrenFullySelectedCount--; };

		  private:
			uint _childrenFullySelectedCount = 0;
		};
		class MapChainIds : public std::map<VTX::App::Core::Model::ID, MapResidueIds>
		{
			friend SelectionModel;

		  public:
			uint getFullySelectedChildCount() const { return _childrenFullySelectedCount; }

		  private:
			void _setFullChildrenCount( const uint p_count ) { _childrenFullySelectedCount = p_count; };
			void _addFullChild() { _childrenFullySelectedCount++; };
			void _removeFullChild() { _childrenFullySelectedCount--; };

			uint _childrenFullySelectedCount = 0;
		};
		using MapMoleculeIds = std::map<VTX::App::Core::Model::ID, MapChainIds>;

		using PairResidueIds  = std::pair<const uint, VecAtomIds>;
		using PairChainIds	  = std::pair<const uint, MapResidueIds>;
		using PairMoleculeIds = std::pair<const VTX::App::Core::Model::ID, MapChainIds>;

		inline const std::set<VTX::App::Core::Model::ID> & getItems() const { return _items; }
		inline std::set<VTX::App::Core::Model::ID> &	   getItems() { return _items; }

		inline const MapMoleculeIds & getMoleculesMap() const { return _moleculesMap; }
		inline MapMoleculeIds &		  getMoleculesMap() { return _moleculesMap; }
		const MapChainIds * const	  getMoleculeMap( const App::Component::Chemistry::Molecule & );

		bool hasItemOfType( const VTX::ID::VTX_ID & p_id ) const;

		bool hasMolecule() const;
		void selectMolecule( App::Component::Chemistry::Molecule &, const bool p_appendToSelection = false );
		void selectMolecules( const std::vector<App::Component::Chemistry::Molecule *> &,
							  const bool										p_appendToSelection = false,
							  const App::Component::Chemistry::Molecule * const p_currentObj		= nullptr );
		void unselectMolecule( App::Component::Chemistry::Molecule & );
		void unselectMolecules( const std::vector<App::Component::Chemistry::Molecule *> & );
		void unselectMoleculesWithCheck( const std::vector<App::Component::Chemistry::Molecule *> & );
		bool isMoleculeSelected( const App::Component::Chemistry::Molecule & ) const;
		bool isMoleculeFullySelected( const App::Component::Chemistry::Molecule & ) const;
		uint getMoleculeSelectedCount() const;

		void selectChain( App::Component::Chemistry::Chain &, const bool p_appendToSelection = false );
		void selectChains( const std::vector<App::Component::Chemistry::Chain *> &,
						   const bool									  p_appendToSelection = false,
						   const App::Component::Chemistry::Chain * const p_currentObj		  = nullptr );
		void unselectChain( App::Component::Chemistry::Chain & );
		void unselectChains( const std::vector<App::Component::Chemistry::Chain *> & );
		void unselectChainsWithCheck( const std::vector<App::Component::Chemistry::Chain *> & );
		bool isChainSelected( const App::Component::Chemistry::Chain & ) const;
		bool isChainFullySelected( const App::Component::Chemistry::Chain & ) const;
		uint getChainSelectedCount() const;

		void selectResidue( App::Component::Chemistry::Residue &, const bool p_appendToSelection = false );
		void selectResidues( const std::vector<App::Component::Chemistry::Residue *> &,
							 const bool										  p_appendToSelection = false,
							 const App::Component::Chemistry::Residue * const p_currentObj		  = nullptr );
		void unselectResidue( App::Component::Chemistry::Residue & );
		void unselectResidues( const std::vector<App::Component::Chemistry::Residue *> & );
		void unselectResiduesWithCheck( const std::vector<App::Component::Chemistry::Residue *> & );
		bool isResidueSelected( const App::Component::Chemistry::Residue & ) const;
		bool isResidueFullySelected( const App::Component::Chemistry::Residue & ) const;
		uint getResidueSelectedCount() const;

		void selectAtom( App::Component::Chemistry::Atom &, const bool p_appendToSelection = false );
		void selectAtoms( const std::vector<App::Component::Chemistry::Atom *> &,
						  const bool									p_appendToSelection = false,
						  const App::Component::Chemistry::Atom * const p_currentObj		= nullptr );
		void unselectAtom( App::Component::Chemistry::Atom & );
		void unselectAtoms( const std::vector<App::Component::Chemistry::Atom *> & );
		void unselectAtomsWithCheck( const std::vector<App::Component::Chemistry::Atom *> & );
		bool isAtomSelected( const App::Component::Chemistry::Atom & ) const;
		uint getAtomSelectedCount() const;

		void selectCategory( App::Component::Chemistry::Category &, const bool p_appendToSelection = false );
		void selectCategories( const std::vector<App::Component::Chemistry::Category *> &,
							   const bool										 p_appendToSelection = false,
							   const App::Component::Chemistry::Category * const p_currentObj		 = nullptr );
		void unselectCategory( App::Component::Chemistry::Category & );
		void unselectCategories( const std::vector<App::Component::Chemistry::Category *> & );
		void unselectCategoriesWithCheck( const std::vector<App::Component::Chemistry::Category *> & );
		bool isCategorySelected( const App::Component::Chemistry::Category & ) const;
		bool isCategoryFullySelected( const App::Component::Chemistry::Category & ) const;

		void selectModels( const std::vector<App::Component::Chemistry::Molecule *> & p_molecules,
						   const std::vector<App::Component::Chemistry::Category *> & p_categories,
						   const std::vector<App::Component::Chemistry::Chain *> &	  p_chains,
						   const std::vector<App::Component::Chemistry::Residue *> &  p_residues,
						   const std::vector<App::Component::Chemistry::Atom *> &	  p_atoms,
						   const bool												  p_appendToSelection = false,
						   const App::Core::Model::BaseModel * const				  p_currentObj		  = nullptr );
		void unselectModels( const std::vector<App::Component::Chemistry::Molecule *> & p_molecules,
							 const std::vector<App::Component::Chemistry::Category *> & p_categories,
							 const std::vector<App::Component::Chemistry::Chain *> &	p_chains,
							 const std::vector<App::Component::Chemistry::Residue *> &	p_residus,
							 const std::vector<App::Component::Chemistry::Atom *> &		p_atoms );

		void selectModel( App::Core::Model::BaseModel & p_model, const bool p_appendToSelection = false );
		void unselectModel( const App::Core::Model::BaseModel & );
		bool isModelSelected( const App::Core::Model::BaseModel & ) const;
		bool isModelSelected( const App::Core::Model::ID & ) const;

		template<typename T, typename = std::enable_if<std::is_base_of<App::Core::Model::BaseModel, T>::value>>
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
		template<typename T, typename = std::enable_if<std::is_base_of<App::Core::Model::BaseModel, T>::value>>
		void unselectModels( const std::vector<T *> & p_models )
		{
			if ( p_models.size() == 0 )
				return;

			for ( const auto it : p_models )
				_unselectModel( *it );

			_notifyDataChanged();
		}
		template<typename T, typename = std::enable_if<std::is_base_of<App::Core::Model::BaseModel, T>::value>>
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

		template<typename T, typename = std::enable_if<std::is_base_of<App::Core::Model::BaseModel, T>::value>>
		std::vector<T *> getItemsOfType( const VTX::ID::VTX_ID & p_modelTypeID )
		{
			std::vector<T *> models = std::vector<T *>();
			for ( const VTX::App::Core::Model::ID & modelID : getItems() )
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

		void moveDataTo( SelectionModel & p_target );

		void receiveEvent( const App::Core::Event::VTXEvent & p_event ) override;

		void									  getItemTypes( std::set<VTX::ID::VTX_ID> & p_types ) const;
		App::Component::Object3D::Helper::AABB	  getAABB() const;
		const App::Core::Model::BaseModel * const getCurrentObject() const;

		template<typename T, typename = std::enable_if<std::is_base_of<App::Core::Model::BaseModel, T>::value>>
		void getItemsOfType( const VTX::ID::VTX_ID & p_itemType, std::set<T *> & p_items ) const
		{
			for ( const VTX::App::Core::Model::ID & itemID : _items )
			{
				if ( VTX::MVC_MANAGER().getModelTypeID( itemID ) == p_itemType )
				{
					T & item = VTX::MVC_MANAGER().getModel<T>( itemID );
					p_items.emplace( &item );
				}
			}
		}
		template<typename T, typename = std::enable_if<std::is_base_of<App::Core::Model::BaseModel, T>::value>>
		void getItemsOfType( const VTX::ID::VTX_ID & p_itemType, std::unordered_set<T *> & p_items ) const
		{
			for ( const VTX::App::Core::Model::ID & itemID : _items )
			{
				if ( VTX::MVC_MANAGER().getModelTypeID( itemID ) == p_itemType )
				{
					T & item = VTX::MVC_MANAGER().getModel<T>( itemID );
					p_items.emplace( &item );
				}
			}
		}

	  protected:
		SelectionModel() : BaseModel( VTX::ID::Model::MODEL_SELECTION )
		{
			_registerEvent( App::Event::Global::MOLECULE_REMOVED );
		}
		~SelectionModel() = default;

		void _notifyDataChanged();

	  private:
		std::set<VTX::App::Core::Model::ID> _items		  = std::set<VTX::App::Core::Model::ID>();
		MapMoleculeIds						_moleculesMap = MapMoleculeIds();

		std::map<VTX::App::Core::Model::ID, App::Component::Object3D::Helper::AABB> _mapSelectionAABB
			= std::map<VTX::App::Core::Model::ID, App::Component::Object3D::Helper::AABB>();
		const App::Core::Model::BaseModel * _currentObject = nullptr;

		void _selectMolecule( const App::Component::Chemistry::Molecule & );
		void _unselectMolecule( const App::Component::Chemistry::Molecule & );
		void _selectCategory( const App::Component::Chemistry::Category & );
		void _unselectCategory( const App::Component::Chemistry::Category & );
		void _selectChain( const App::Component::Chemistry::Chain & );
		void _unselectChain( const App::Component::Chemistry::Chain & );
		void _selectResidue( const App::Component::Chemistry::Residue & );
		void _unselectResidue( const App::Component::Chemistry::Residue & );
		void _selectAtom( const App::Component::Chemistry::Atom & );
		void _unselectAtom( const App::Component::Chemistry::Atom & );

		void _unselectModel( const App::Core::Model::BaseModel & );

		bool _addMolecule( const App::Component::Chemistry::Molecule & );
		bool _addChain( const App::Component::Chemistry::Chain & );
		bool _addResidue( const App::Component::Chemistry::Residue & );
		bool _addAtom( const App::Component::Chemistry::Atom & );

		void _addMoleculeContent( const App::Component::Chemistry::Molecule & );
		void _addChainContent( const App::Component::Chemistry::Chain & );
		void _addResidueContent( const App::Component::Chemistry::Residue & );

		void _removeMolecule( const App::Component::Chemistry::Molecule & );
		void _removeChain( const App::Component::Chemistry::Chain & );
		void _removeResidue( const App::Component::Chemistry::Residue & );
		void _removeAtom( const App::Component::Chemistry::Atom & );

		void _clearWithoutNotify();

		void _refreshMoleculeSelection( App::Component::Chemistry::Molecule * const );
		void _recomputeAABB();

		void _referenceAtom( const App::Component::Chemistry::Atom & p_atom );
		void _referenceFullResidue( const App::Component::Chemistry::Residue & p_residue );
		void _referenceFullChain( const App::Component::Chemistry::Chain & p_chain );

		void _unreferenceAtom( const App::Component::Chemistry::Atom & p_atom );
		void _unreferenceFullResidue( const App::Component::Chemistry::Residue & p_residue );
		void _unreferenceFullChain( const App::Component::Chemistry::Chain & p_chain );

		void _emplaceMolecule( const App::Component::Chemistry::Molecule & );

		void _setCurrentObject( const App::Core::Model::BaseModel * const p_model, const bool p_notify = true );
		void _clearCurrentObject( const bool p_notify = true );
	};

} // namespace VTX::App::Application::Selection
#endif
