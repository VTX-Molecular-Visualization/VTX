#ifndef __VTX_APP_APPLICATION_SELECTION_SELECTION__
#define __VTX_APP_APPLICATION_SELECTION_SELECTION__

#include "app/application/selection/selection_data.hpp"
#include "app/component/scene/_fwd.hpp"
#include "app/component/scene/selectable.hpp"
#include "app/core/ecs/registry.hpp"
#include "app/vtx_app.hpp"
#include <map>
#include <set>
#include <util/math/aabb.hpp>

namespace VTX::App::Application::Selection
{
	class Selection
	{
	  public:
		using SelectionDataSet = std::set<std::unique_ptr<SelectionData>>;

	  public:
		template<typename C>
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
				return VTXApp::get().MAIN_REGISTRY().getComponent<C>( selectionComponent );
			}
			C * operator->()
			{
				const Component::Scene::Selectable & selectionComponent = ( *_it )->getSelectionComponent();
				return &VTXApp::get().MAIN_REGISTRY().getComponent<C>( selectionComponent );
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
				while ( _it != _endIt
						&& !VTXApp::get().MAIN_REGISTRY().hasComponent<C>( ( *_it )->getSelectionComponent() ) )
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
		template<typename C>
		inline Iterator<C> begin() const
		{
			return Iterator<C>( _items.cbegin(), _items.cend() );
		}

		SelectionDataSet::iterator end() { return _items.end(); }
		template<typename C>
		inline Iterator<C> end() const
		{
			return Iterator<C>( _items.cend(), _items.cend() );
		}

		template<typename C>
		bool hasItemOfType() const
		{
			return begin<C>() != end<C>();
		}

		SelectionData & select( const Component::Scene::Selectable & p_selectableComponent );

		template<typename C>
		SelectionData & select( const C & p_component )
		{
			const Component::Scene::Selectable & selectableComponent
				= VTXApp::get().MAIN_REGISTRY().getComponent<Component::Scene::Selectable>( p_component );

			return select( selectableComponent );
		}

		template<typename Container>
		void selectAll( const Container & p_items )
		{
			for ( Component::Scene::Selectable * const item : p_items )
				select( *item );
		}

		void unselect( const Component::Scene::Selectable & p_selectableComponent );

		template<typename Container>
		void unselectAll( const Container & p_items )
		{
			for ( Component::Scene::Selectable * const item : p_items )
				unselect( *item );
		}

		bool isSelected( const Component::Scene::Selectable & p_item ) const;

		template<typename Container>
		bool areSelected( const Container & p_items ) const
		{
			if ( p_items.size() == 0 )
				return false;

			for ( SelectionData * const item : p_items )
				if ( !isSelected( item->getSelectionComponent() ) )
					return false;

			return true;
		}

		SelectionData &		  getSelectionData( const Component::Scene::Selectable & p_selectableComponent );
		const SelectionData & getSelectionData( const Component::Scene::Selectable & p_selectableComponent ) const;

		template<typename C>
		SelectionData & getSelectionData( const C & p_component )
		{
			return getSelectionData(
				VTXApp::get().MAIN_REGISTRY().getComponent<Component::Scene::Selectable>( p_component )
			);
		}
		template<typename C>
		const SelectionData & getSelectionData( const C & p_component ) const
		{
			return getSelectionData(
				VTXApp::get().MAIN_REGISTRY().getComponent<Component::Scene::Selectable>( p_component )
			);
		}

		inline size_t getCount() const { return _items.size(); }

		template<typename C>
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

		inline const SelectionData * const getCurrentObject() const { return _currentObject; }
		void							   setCurrentObject( const SelectionData * const );

		Util::Math::AABB getAABB() const;

		std::string toString() const;

	  protected:
		void _notifyDataChanged();

	  private:
		SelectionDataSet _items = SelectionDataSet();

		std::map<const SelectionData *, Component::Scene::AABB *> _mapSelectionAABB
			= std::map<const SelectionData *, Component::Scene::AABB *>();

		const SelectionData * _currentObject = nullptr;

		const std::unique_ptr<SelectionData> & _getSelectionDataPtr( const Component::Scene::Selectable & p_selectable
		) const;

		void _clearWithoutNotify();

		// void _refreshMoleculeSelection( App::Old::Component::Chemistry::Molecule * const );

		void _setCurrentObject( const SelectionData * const p_model, const bool p_notify = true );
		void _clearCurrentObject( const bool p_notify = true );
	};

} // namespace VTX::App::Application::Selection
#endif
