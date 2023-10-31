#include "app/application/selection/selection.hpp"
#include "app/application/ecs/registry_manager.hpp"
#include "app/component/scene/aabb_component.hpp"
#include "app/vtx_app.hpp"
#include <algorithm>
#include <sstream>
#include <util/exceptions.hpp>

namespace VTX::App::Application::Selection
{
	Selection::Selection() = default;
	Selection::Selection( const Selection & p_source )
	{
		for ( const std::unique_ptr<SelectionData> & item : _items )
		{
			_items.emplace( std::make_unique<SelectionData>( *item ) );
		}
	}

	Selection::~Selection() = default;

	Selection Selection::add( const Selection & p_lhs, const Selection & p_rhs )
	{
		Selection res = Selection( p_lhs );

		for ( const std::unique_ptr<SelectionData> & item : p_rhs._items )
		{
			res.select( item->getSelectionComponent() );
		}

		return res;
	}

	// TODO
	Selection Selection::remove( const Selection & p_lhs, const Selection & p_rhs ) { return Selection(); }
	Selection Selection::intersection( const Selection & p_lhs, const Selection & p_rhs ) { return Selection(); }
	Selection Selection::exclusive( const Selection & p_lhs, const Selection & p_rhs ) { return Selection(); }
	Selection Selection::inverse( const Selection & p_selection ) { return Selection(); }

	SelectionData & Selection::select( const Component::Scene::Selectable & p_selectableComponent )
	{
		if ( isSelected( p_selectableComponent ) )
		{
			return getSelectionData( p_selectableComponent );
		}
		else
		{
			std::unique_ptr<Application::Selection::SelectionData> selectionItem
				= p_selectableComponent.instantiateSelectionData();

			auto it = _items.emplace( std::move( selectionItem ) );

			if ( VTXApp::get().MAIN_REGISTRY().hasComponent<Component::Scene::AABB>( p_selectableComponent ) )
			{
				Component::Scene::AABB & aabbComponent
					= VTXApp::get().MAIN_REGISTRY().getComponent<Component::Scene::AABB>( p_selectableComponent );

				_mapSelectionAABB[ ( *( it.first ) ).get() ] = &aabbComponent;
			}

			return **( it.first );
		}
	}
	void Selection::unselect( const Component::Scene::Selectable & p_selectableComponent )
	{
		const std::unique_ptr<SelectionData> & selItem = _getSelectionDataPtr( p_selectableComponent );

		if ( VTXApp::get().MAIN_REGISTRY().hasComponent<Component::Scene::AABB>( p_selectableComponent ) )
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
