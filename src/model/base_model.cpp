#include "base_model.hpp"

namespace VTX
{
	namespace Model
	{
		BaseModel::BaseModel() {}

		void BaseModel::_notifyViews( const Event::EVENT_MODEL p_event )
		{
			for ( PairStringToItemPtr pair : _getItems() )
			{
				pair.second->notify( p_event );
			}
		}

	} // namespace Model
} // namespace VTX
