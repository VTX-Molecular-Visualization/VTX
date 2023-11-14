#include "app/application/selection/selection.hpp"
#include "app/application/ecs/registry_manager.hpp"
#include "app/component/scene/aabb_component.hpp"
#include "app/vtx_app.hpp"
#include <algorithm>
#include <sstream>
#include <util/exceptions.hpp>
#include <util/logger.hpp>

namespace VTX::App::Application::Selection
{
	Selection::Selection() = default;
	Selection::Selection( const Selection & p_source )
	{
		for ( const std::unique_ptr<SelectionData> & item : p_source._items )
		{
			_items.emplace( item->clone() );
		}
	}

	Selection::~Selection() = default;

	bool Selection::isEqualsTo( const Selection & p_other ) const
	{
		if ( _items.size() != p_other._items.size() )
			return false;

		for ( const std::unique_ptr<SelectionData> & lhsSelData : _items )
		{
			bool areEquals = false;
			for ( const std::unique_ptr<SelectionData> & rhsSelData : p_other._items )
			{
				if ( &lhsSelData->getSelectionComponent() == &rhsSelData->getSelectionComponent() )
				{
					areEquals = lhsSelData->isEqualsTo( *rhsSelData );
					break;
				}
			}

			if ( !areEquals )
				return false;
		}

		return true;
	}

	Selection Selection::add( const Selection & p_lhs, const Selection & p_rhs )
	{
		Selection res = Selection( p_lhs );

		for ( const std::unique_ptr<SelectionData> & item : p_rhs._items )
		{
			const Component::Scene::Selectable & rhsSelectableComponent = item->getSelectionComponent();

			if ( !res.isSelected( rhsSelectableComponent ) )
			{
				res.select( rhsSelectableComponent, AssignmentType::APPEND );
			}
			else
			{
				res.getSelectionData( rhsSelectableComponent ).add( *item );
			}
		}

		return res;
	}
	Selection Selection::remove( const Selection & p_lhs, const Selection & p_rhs )
	{
		Selection res = Selection( p_lhs );

		for ( const std::unique_ptr<SelectionData> & item : p_rhs._items )
		{
			const Component::Scene::Selectable & rhsSelectableComponent = item->getSelectionComponent();

			if ( res.isSelected( rhsSelectableComponent ) )
			{
				const SelectionData & rhsSelectionData = res.getSelectionData( rhsSelectableComponent ).remove( *item );

				if ( !rhsSelectionData.isValid() )
					res.unselect( rhsSelectableComponent );
			}
		}

		return res;
	}
	Selection Selection::intersection( const Selection & p_lhs, const Selection & p_rhs )
	{
		Selection res = Selection( p_lhs );

		for ( const std::unique_ptr<SelectionData> & item : p_rhs._items )
		{
			const Component::Scene::Selectable & rhsSelectableComponent = item->getSelectionComponent();

			if ( res.isSelected( rhsSelectableComponent ) )
			{
				const SelectionData & rhsSelectionData
					= res.getSelectionData( rhsSelectableComponent ).intersect( *item );

				if ( !rhsSelectionData.isValid() )
					res.unselect( rhsSelectableComponent );
			}
		}

		return res;
	}
	Selection Selection::exclusive( const Selection & p_lhs, const Selection & p_rhs )
	{
		Selection res = Selection( p_lhs );

		for ( const std::unique_ptr<SelectionData> & item : p_rhs._items )
		{
			const Component::Scene::Selectable & rhsSelectableComponent = item->getSelectionComponent();

			if ( res.isSelected( rhsSelectableComponent ) )
			{
				const SelectionData & rhsSelectionData
					= res.getSelectionData( rhsSelectableComponent ).exclude( *item );

				if ( !rhsSelectionData.isValid() )
					res.unselect( rhsSelectableComponent );
			}
			else
			{
				res.select( rhsSelectableComponent, *item, AssignmentType::APPEND );
			}
		}

		return res;
	}

	// TODO
	Selection Selection::inverse( const Selection & p_selection ) { return Selection(); }

	SelectionData & Selection::select(
		const Component::Scene::Selectable & p_selectableComponent,
		const AssignmentType				 p_assignment
	)
	{
		if ( p_assignment == AssignmentType::SET )
			clear();

		if ( isSelected( p_selectableComponent ) )
		{
			SelectionData & res = getSelectionData( p_selectableComponent );
			res.selectAll();

			return res;
		}
		else
		{
			std::unique_ptr<Application::Selection::SelectionData> selectionItem
				= p_selectableComponent.instantiateSelectionData();

			auto it = _items.emplace( std::move( selectionItem ) );

			if ( MAIN_REGISTRY().hasComponent<Component::Scene::AABB>( p_selectableComponent ) )
			{
				Component::Scene::AABB & aabbComponent
					= MAIN_REGISTRY().getComponent<Component::Scene::AABB>( p_selectableComponent );

				_mapSelectionAABB[ ( *( it.first ) ).get() ] = &aabbComponent;
			}

			return **( it.first );
		}
	}
	SelectionData & Selection::select(
		const Component::Scene::Selectable & p_selectableComponent,
		const SelectionData &				 p_selectionData,
		const AssignmentType				 p_assignment
	)
	{
		if ( p_assignment == AssignmentType::SET )
			clear();

		if ( isSelected( p_selectableComponent ) )
		{
			SelectionData & res = getSelectionData( p_selectableComponent );
			res.add( p_selectionData );

			return res;
		}
		else
		{
			std::unique_ptr<Application::Selection::SelectionData> selectionItem
				= p_selectableComponent.instantiateSelectionData();
			selectionItem->set( p_selectionData );

			auto it = _items.emplace( std::move( selectionItem ) );

			if ( MAIN_REGISTRY().hasComponent<Component::Scene::AABB>( p_selectableComponent ) )
			{
				Component::Scene::AABB & aabbComponent
					= MAIN_REGISTRY().getComponent<Component::Scene::AABB>( p_selectableComponent );

				_mapSelectionAABB[ ( *( it.first ) ).get() ] = &aabbComponent;
			}

			return **( it.first );
		}
	}

	void Selection::selectAll(
		const std::initializer_list<const Component::Scene::Selectable *> & p_items,
		const AssignmentType												p_assignment
	)
	{
		if ( p_assignment == AssignmentType::SET )
			clear();

		for ( const Component::Scene::Selectable * const item : p_items )
			select( *item, AssignmentType::APPEND );
	}

	void Selection::unselect( const Component::Scene::Selectable & p_selectableComponent )
	{
		const std::unique_ptr<SelectionData> & selItem = _getSelectionDataPtr( p_selectableComponent );

		if ( MAIN_REGISTRY().hasComponent<Component::Scene::AABB>( p_selectableComponent ) )
		{
			_mapSelectionAABB.erase( selItem.get() );
		}

		_items.erase( selItem );
	}

	bool Selection::isSelected( const Component::Scene::Selectable & p_selectableComponent ) const
	{
		auto it = std::find_if(
			_items.begin(),
			_items.end(),
			[ &p_selectableComponent ]( const std::unique_ptr<SelectionData> & p_item )
			{ return &p_item->getSelectionComponent() == &p_selectableComponent; }
		);

		return it != _items.end();
	}

	bool Selection::areSelected( const std::initializer_list<const Component::Scene::Selectable *> & p_items ) const
	{
		if ( p_items.size() == 0 )
			return false;

		for ( const Component::Scene::Selectable * item : p_items )
			if ( !isSelected( *item ) )
				return false;

		return true;
	}

	void Selection::clear()
	{
		_clearWithoutNotify();
		_notifyDataChanged();
	}

	Util::Math::AABB Selection::getAABB() const
	{
		Util::Math::AABB res = Util::Math::AABB();

		if ( _mapSelectionAABB.empty() )
		{
			res.extend( VEC3F_ZERO );
		}
		else
		{
			for ( const auto & pairItemAABB : _mapSelectionAABB )
			{
				res.extend( pairItemAABB.second->getWorldAABB() );
			}
		}

		return res;
	}
	void Selection::setCurrentObject( const SelectionData * const p_object ) { _setCurrentObject( p_object ); }

	void Selection::_notifyDataChanged() {}

	void Selection::_clearWithoutNotify()
	{
		_items.clear();
		_mapSelectionAABB.clear();
	}

	SelectionData & Selection::getSelectionData( const Component::Scene::Selectable & p_selectable )
	{
		return *_getSelectionDataPtr( p_selectable );
	}

	const SelectionData & Selection::getSelectionData( const Component::Scene::Selectable & p_selectable ) const
	{
		return *_getSelectionDataPtr( p_selectable );
	}

	const std::unique_ptr<SelectionData> & Selection::_getSelectionDataPtr(
		const Component::Scene::Selectable & p_selectable
	) const
	{
		for ( const std::unique_ptr<SelectionData> & selectionItem : _items )
		{
			if ( &selectionItem->getSelectionComponent() == &p_selectable )
			{
				return selectionItem;
			}
		}

		throw VTXException( "Selectable object not found in selection." );
	}

	void Selection::_setCurrentObject( const SelectionData * const p_object, const bool p_notify )
	{
		if ( _currentObject != p_object )
		{
			_currentObject = p_object;

			if ( p_notify )
				_notifyDataChanged();
		}
	}
	void Selection::_clearCurrentObject( const bool p_notify )
	{
		if ( _currentObject != nullptr )
		{
			_currentObject = nullptr;

			if ( p_notify )
				_notifyDataChanged();
		}
	}

	std::string Selection::toString() const
	{
		std::stringstream sstr = std::stringstream();

		sstr << "Obj count : " << _items.size() << std::endl;

		for ( const std::unique_ptr<SelectionData> & data : _items )
		{
			sstr << data->toString() << std::endl;
		}

		return sstr.str();
	}

} // namespace VTX::App::Application::Selection
