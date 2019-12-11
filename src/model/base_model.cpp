#include "base_model.hpp"
#include <magic_enum.hpp>

namespace VTX
{
	namespace Model
	{
		BaseModel::~BaseModel() { _clearViews(); }

		void BaseModel::init() { _addViews(); }

		const BaseModel::ViewSharedPtr BaseModel::getViewByName( const std::string & p_name ) const
		{
			try
			{
				return _views.at( p_name );
			}
			catch ( const std::exception )
			{
				VTX_WARNING( "View not found: " + p_name );
				return nullptr;
			}
		}

		void BaseModel::_addView( const ViewSharedPtr p_view )
		{
			p_view->setModel( this );
			try
			{
				_views.try_emplace( p_view->getNameStr(), p_view );
			}
			catch ( const std::exception )
			{
				VTX_WARNING( "A view with this name already exists: " + p_view->getNameStr() );
			}
		}

		void BaseModel::_notifyViews( const Event::EVENT_MODEL p_event ) const
		{
			for ( PairStringToViewSharedPtr pair : _views )
			{
				pair.second->notify( p_event );
			}
		}

		void BaseModel::_clearViews()
		{
			for ( PairStringToViewSharedPtr pair : _views )
			{
				pair.second->setModel( nullptr );
			}
			_views.clear();
		}

	} // namespace Model
} // namespace VTX
