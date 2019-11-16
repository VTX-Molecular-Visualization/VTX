#include "base_model.hpp"

namespace VTX
{
	namespace Model
	{
		void BaseModel::init() { _addViews(); }

		void BaseModel::_addView( View::BaseView<BaseModel> * const p_view )
		{
			p_view->setModel( *this );
			_views.push_back( p_view );
		}

		void BaseModel::_notifyViews() const
		{
			for ( View::BaseView<BaseModel> * view : _views )
			{
				view->notify();
			}
		}

	} // namespace Model
} // namespace VTX
