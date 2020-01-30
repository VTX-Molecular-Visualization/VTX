#include "base_model.hpp"

namespace VTX
{
	namespace Model
	{
		// uint BaseModel::_COUNTER = 0;

		BaseModel::BaseModel()
		{ /* _id = _COUNTER++;*/
		}

		/*
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
		*/

		void BaseModel::_notifyViews( const Event::EVENT_MODEL p_event )
		{
			for ( PairStringToItemPtr pair : _getItems() )
			{
				pair.second->notify( p_event );
			}
		}

	} // namespace Model
} // namespace VTX
