#include "app/selection/selection.hpp"
#include "app/vtx_app.hpp"
#include <algorithm>
#include <sstream>
#include <string_view>
#include <util/exceptions.hpp>
#include <util/logger.hpp>

namespace VTX::App::Selection
{
	Selection::Selection() = default;
	Selection::Selection( const Selection & p_source )
	{
		for ( const std::unique_ptr<SelectionData> & item : p_source._items )
		{
			_items.emplace( item->clone() );
		}

		_currentSelectionData = _currentSelectionData;
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

			onSelect( res );

			_setCurrentObject( &res );

			return res;
		}
		else
		{
			std::unique_ptr<App::Selection::SelectionData> selectionItem
				= p_selectableComponent.instantiateSelectionData();

			auto								   it				= _items.emplace( std::move( selectionItem ) );
			const std::unique_ptr<SelectionData> & selectionDataPtr = *( it.first );

			onSelect( *selectionDataPtr );

			_setCurrentObject( selectionDataPtr.get() );

			return *selectionDataPtr;
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

			onSelect( res );

			_setCurrentObject( &res );

			return res;
		}
		else
		{
			std::unique_ptr<App::Selection::SelectionData> selectionItem
				= p_selectableComponent.instantiateSelectionData();
			selectionItem->set( p_selectionData );

			auto								   it				= _items.emplace( std::move( selectionItem ) );
			const std::unique_ptr<SelectionData> & selectionDataPtr = *( it.first );

			onSelect( *selectionDataPtr );

			_setCurrentObject( selectionDataPtr.get() );

			return *selectionDataPtr;
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

		onDeselect( *selItem );

		if ( _currentSelectionData == selItem.get() )
			_clearCurrentObject();

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
		_clearCurrentObject();

		while ( _items.size() > 0 )
		{
			const std::unique_ptr<SelectionData> & selectionData = *_items.begin();
			onDeselect( *selectionData );

			_items.erase( _items.begin() );
		}
	}

	Util::Math::AABB Selection::getAABB() const
	{
		Util::Math::AABB res = Util::Math::AABB();

		for ( const std::unique_ptr<SelectionData> & selectionData : _items )
		{
			res.extend( selectionData->getAABB() );
		}

		return res;
	}
	void Selection::setCurrentObject( const Component::Scene::Selectable & p_item )
	{
		_setCurrentObject( _getSelectionDataPtr( p_item ).get() );
	}
	void Selection::setCurrentObject( const SelectionData & p_selectionData ) { _setCurrentObject( &p_selectionData ); }

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

	void Selection::_setCurrentObject( const SelectionData * const p_selectionData )
	{
		if ( _currentSelectionData != p_selectionData )
		{
			_currentSelectionData = p_selectionData;
			onCurrentObjectChange( p_selectionData );
		}
	}
	void Selection::_clearCurrentObject()
	{
		if ( _currentSelectionData != nullptr )
		{
			_currentSelectionData = nullptr;
			onCurrentObjectChange( nullptr );
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

} // namespace VTX::App::Selection
