#include "base_model.hpp"

namespace VTX
{
	namespace Model
	{
		BaseModel::~BaseModel() { _clearViews(); }

		void BaseModel::init() { _addViews(); }

		const BaseModel::ViewSharedPtr BaseModel::getViewByName( const View::VIEW_NAME p_name ) const
		{
			try
			{
				return _views.at( p_name );
			}
			catch ( const std::exception )
			{
				VTX_WARNING( "View not found: " + ENUM_TO_STRING( p_name ) );
				return nullptr;
			}
		}

		void BaseModel::_addView( const ViewSharedPtr p_view )
		{
			p_view->setModel( this );
			try
			{
				_views.try_emplace( p_view->getViewName(), p_view );
			}
			catch ( const std::exception )
			{
				VTX_WARNING( "A view with this name already exists: " + ENUM_TO_STRING( p_view->getViewName() ) );
			}
		}

		void BaseModel::_removeView( const View::VIEW_NAME p_name )
		{
			ViewSharedPtr view = getViewByName( p_name );
			if ( view == nullptr ) return;

			view->setModel( nullptr );
			_views.erase( p_name );
		}

		void BaseModel::_notifyViews( const Event::EVENT_MODEL p_event ) const
		{
			for ( PairEnumToViewSharedPtr pair : _views )
			{
				pair.second->notify( p_event );
			}
		}

		void BaseModel::_clearViews()
		{
			for ( PairEnumToViewSharedPtr pair : _views )
			{
				pair.second->setModel( nullptr );
			}
			_views.clear();
		}

	} // namespace Model
} // namespace VTX
