#ifndef __VTX_APP_APPLICATION_SELECTION_SELECTION__
#define __VTX_APP_APPLICATION_SELECTION_SELECTION__

#include "app/application/selection/concepts.hpp"
#include "app/application/selection/selection_data.hpp"
#include "app/application/system/ecs_system.hpp"
#include "app/component/scene/selectable.hpp"
#include <concepts>
#include <map>
#include <set>
#include <util/callback.hpp>
#include <util/concepts.hpp>
#include <util/math/aabb.hpp>

namespace VTX::App::Application::Selection
{
	enum class AssignmentType : int
	{
		SET,
		APPEND
	};

	class Selection
	{
	  public:
		using SelectionDataSet = std::set<std::unique_ptr<SelectionData>>;

	  public:
		template<Core::ECS::ConceptComponent C>
		struct Iterator
		{
			Iterator( SelectionDataSet::const_iterator p_setIt, const SelectionDataSet::const_iterator p_endIt ) :
				_endIt( p_endIt ), _it( p_setIt )
			{
				_getValid();
			}

			C & operator*() const
			{
				const Component::Scene::Selectable & selectionComponent = ( *_it )->getSelectionComponent();
				return MAIN_REGISTRY().getComponent<C>( selectionComponent );
			}
			C * operator->()
			{
				const Component::Scene::Selectable & selectionComponent = ( *_it )->getSelectionComponent();
				return &MAIN_REGISTRY().getComponent<C>( selectionComponent );
			}

			// Prefix increment
			Iterator<C> & operator++()
			{
				_it++;
				_getValid();

				return *this;
			}

			// Postfix increment
			Iterator<C> operator++( int )
			{
				Iterator<C> tmp = Iterator<C>( _it, _endIt );

				_it++;
				_getValid();

				return tmp;
			}

			friend bool operator==( const Iterator<C> & a, const Iterator<C> & b ) { return a._it == b._it; };
			friend bool operator!=( const Iterator<C> & a, const Iterator<C> & b ) { return a._it != b._it; };

		  private:
			SelectionDataSet::const_iterator	   _it;
			const SelectionDataSet::const_iterator _endIt;

			void _getValid()
			{
				while ( _it != _endIt && !MAIN_REGISTRY().hasComponent<C>( ( *_it )->getSelectionComponent() ) )
				{
					_it++;
				}
			}
		};

	  public:
		static Selection add( const Selection & p_lhs, const Selection & p_rhs );
		static Selection remove( const Selection & p_lhs, const Selection & p_rhs );
		static Selection intersection( const Selection & p_lhs, const Selection & p_rhs );
		static Selection exclusive( const Selection & p_lhs, const Selection & p_rhs );
		static Selection inverse( const Selection & p_selection );

	  public:
		Selection();
		Selection( const Selection & p_source );
		~Selection();

		bool isEqualsTo( const Selection & p_other ) const;

		inline const SelectionDataSet & getAll() const { return _items; }
		inline SelectionDataSet &		getAll() { return _items; }

		SelectionDataSet::iterator begin() { return _items.begin(); }
		template<Core::ECS::ConceptComponent C>
		inline Iterator<C> begin() const
		{
			return Iterator<C>( _items.cbegin(), _items.cend() );
		}

		SelectionDataSet::iterator end() { return _items.end(); }
		template<Core::ECS::ConceptComponent C>
		inline Iterator<C> end() const
		{
			return Iterator<C>( _items.cend(), _items.cend() );
		}

		template<Core::ECS::ConceptComponent C>
		bool hasItemOfType() const
		{
			return begin<C>() != end<C>();
		}

		SelectionData & select(
			const Component::Scene::Selectable & p_selectableComponent,
			const AssignmentType				 p_assignment = AssignmentType::SET
		);
		SelectionData & select(
			const Component::Scene::Selectable & p_selectableComponent,
			const SelectionData &				 p_selectionData,
			const AssignmentType				 p_assignment = AssignmentType::SET
		);

		template<SelectionDataConcept T>
		T & select(
			const Component::Scene::Selectable & p_selectableComponent,
			const AssignmentType				 p_assignment = AssignmentType::SET
		)
		{
			return dynamic_cast<T &>( select( p_selectableComponent, p_assignment ) );
		}

		template<SelectionDataConcept T>
		T & select(
			const Component::Scene::Selectable & p_selectableComponent,
			const SelectionData &				 p_selectionData,
			const AssignmentType				 p_assignment = AssignmentType::SET
		)
		{
			return dynamic_cast<T &>( select( p_selectableComponent, p_selectionData, p_assignment ) );
		}

		template<Core::ECS::ConceptComponent C>
		SelectionData & select( const C & p_component, const AssignmentType p_assignment = AssignmentType::SET )
		{
			const Component::Scene::Selectable & selectableComponent
				= MAIN_REGISTRY().getComponent<Component::Scene::Selectable>( p_component );

			return select( selectableComponent, p_assignment );
		}
		template<Core::ECS::ConceptComponent C>
		SelectionData & select(
			const C &			  p_component,
			const SelectionData & p_selectionData,
			const AssignmentType  p_assignment = AssignmentType::SET
		)
		{
			const Component::Scene::Selectable & selectableComponent
				= MAIN_REGISTRY().getComponent<Component::Scene::Selectable>( p_component );

			return select( selectableComponent, p_selectionData, p_assignment );
		}

		template<SelectionDataConcept T, Core::ECS::ConceptComponent C>
		T & select( const C & p_component, const AssignmentType p_assignment = AssignmentType::SET )
		{
			const Component::Scene::Selectable & selectableComponent
				= MAIN_REGISTRY().getComponent<Component::Scene::Selectable>( p_component );

			return select<T>( selectableComponent, p_assignment );
		}
		template<SelectionDataConcept T, Core::ECS::ConceptComponent C>
		T & select(
			const C &			  p_component,
			const SelectionData & p_selectionData,
			const AssignmentType  p_assignment = AssignmentType::SET
		)
		{
			const Component::Scene::Selectable & selectableComponent
				= MAIN_REGISTRY().getComponent<Component::Scene::Selectable>( p_component );

			return select<T>( selectableComponent, p_selectionData, p_assignment );
		}

		void selectAll(
			const std::initializer_list<const Component::Scene::Selectable *> & p_items,
			const AssignmentType												p_assignment = AssignmentType::SET
		);

		template<ContainerOfType<const Component::Scene::Selectable *> C>
		void selectAll( const C & p_items, const AssignmentType p_assignment = AssignmentType::SET )
		{
			if ( p_assignment == AssignmentType::SET )
				clear();

			for ( const Component::Scene::Selectable * const item : p_items )
				select( *item, AssignmentType::APPEND );
		}

		template<ContainerOfType<const SelectionData *> C>
		void selectAll( const C & p_data, const AssignmentType p_assignment = AssignmentType::SET )
		{
			if ( p_assignment == AssignmentType::SET )
				clear();

			for ( const SelectionData * dataPtr : p_data )
			{
				select( dataPtr->getSelectionComponent(), *dataPtr, AssignmentType::APPEND );
			}
		}
		template<ContainerOfType<const Component::Scene::Selectable *> C1, ContainerOfType<const SelectionData *> C2>
		void selectAll( const C1 & p_items, const C2 & p_data, const AssignmentType p_assignment = AssignmentType::SET )
		{
			assert( p_items.size() == p_data.size() );

			if ( p_assignment == AssignmentType::SET )
				clear();

			typename C1::iterator itItem = p_items.begin();
			typename C2::iterator itData = p_data.begin();

			while ( itItem != p_items.end() && itData != p_data.end() )
			{
				select( **itItem, **itData, AssignmentType::APPEND );

				itItem++;
				itData++;
			}
		}

		void unselect( const Component::Scene::Selectable & p_selectableComponent );

		template<ContainerOfType<const Component::Scene::Selectable *> C>
		void unselectAll( const C & p_items )
		{
			for ( const Component::Scene::Selectable * const item : p_items )
				unselect( *item );
		}

		bool isSelected( const Component::Scene::Selectable & p_item ) const;

		template<Core::ECS::ConceptComponent C>
		bool isSelected( const C & p_component ) const
		{
			return isSelected( MAIN_REGISTRY().getComponent<Component::Scene::Selectable>( p_component ) );
		}

		bool areSelected( const std::initializer_list<const Component::Scene::Selectable *> & p_items ) const;

		template<Core::ECS::ConceptComponent C>
		bool areSelected( const std::initializer_list<C *> & p_items ) const
		{
			if ( p_items.size() == 0 )
				return false;

			for ( const C * item : p_items )
			{
				const Component::Scene::Selectable & selectableComponent
					= MAIN_REGISTRY().getComponent<Component::Scene::Selectable>( item );

				if ( !isSelected( selectableComponent ) )
					return false;
			}

			return true;
		}

		template<ContainerOfType<const SelectionData *> C>
		bool areSelected( const C & p_items ) const
		{
			if ( p_items.size() == 0 )
				return false;

			for ( const SelectionData * const item : p_items )
				if ( !isSelected( item->getSelectionComponent() ) )
					return false;

			return true;
		}

		SelectionData &		  getSelectionData( const Component::Scene::Selectable & p_selectableComponent );
		const SelectionData & getSelectionData( const Component::Scene::Selectable & p_selectableComponent ) const;
		template<SelectionDataConcept T>
		T & getSelectionData( const Component::Scene::Selectable & p_selectableComponent )
		{
			return dynamic_cast<T &>( getSelectionData( p_selectableComponent ) );
		}
		template<SelectionDataConcept T>
		const T & getSelectionData( const Component::Scene::Selectable & p_selectableComponent ) const
		{
			return dynamic_cast<T &>( getSelectionData( p_selectableComponent ) );
		}

		template<Core::ECS::ConceptComponent C>
		SelectionData & getSelectionDataFromComponent( const C & p_component )
		{
			return getSelectionData( MAIN_REGISTRY().getComponent<Component::Scene::Selectable>( p_component ) );
		}
		template<Core::ECS::ConceptComponent C>
		const SelectionData & getSelectionDataFromComponent( const C & p_component ) const
		{
			return getSelectionData( MAIN_REGISTRY().getComponent<Component::Scene::Selectable>( p_component ) );
		}
		template<SelectionDataConcept T, Core::ECS::ConceptComponent C>
		T & getSelectionDataFromComponent( const C & p_component )
		{
			return getSelectionData<T>( MAIN_REGISTRY().getComponent<Component::Scene::Selectable>( p_component ) );
		}
		template<SelectionDataConcept T, Core::ECS::ConceptComponent C>
		const T & getSelectionDataFromComponent( const C & p_component ) const
		{
			return getSelectionData<T>( MAIN_REGISTRY().getComponent<Component::Scene::Selectable>( p_component ) );
		}

		inline size_t getCount() const { return _items.size(); }

		template<Core::ECS::ConceptComponent C>
		size_t getCount() const
		{
			Iterator<C> it	= begin<C>();
			size_t		res = 0;

			while ( it != end<C>() )
			{
				res++;
				it++;
			}

			return res;
		}

		inline bool isEmpty() const { return _items.size() == 0; }
		void		clear();

		inline const Component::Scene::Selectable & getCurrentObject() const
		{
			return _currentSelectionData->getSelectionComponent();
		}

		const SelectionData & getCurrentSelectionData() const { return *_currentSelectionData; }
		template<SelectionDataConcept T>
		const T & getCurrentSelectionData() const
		{
			return dynamic_cast<T &>( *_currentSelectionData );
		}

		void setCurrentObject( const Component::Scene::Selectable & p_item );
		void setCurrentObject( const SelectionData & p_selectionData );

		Util::Math::AABB getAABB() const;

		std::string toString() const;

		Util::Callback<const SelectionData &> onSelect;
		Util::Callback<const SelectionData &> onDeselect;
		Util::Callback<const SelectionData *> onCurrentObjectChange;

	  private:
		SelectionDataSet	  _items				= SelectionDataSet();
		const SelectionData * _currentSelectionData = nullptr;

		const std::unique_ptr<SelectionData> & _getSelectionDataPtr( const Component::Scene::Selectable & p_selectable
		) const;

		void _setCurrentObject( const SelectionData * const p_model );
		void _clearCurrentObject();
	};

} // namespace VTX::App::Application::Selection
#endif
