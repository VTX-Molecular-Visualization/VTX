#include "base_model.hpp"

namespace VTX
{
	namespace Model
	{
		BaseModel::~BaseModel()
		{
			for ( View::BaseView<BaseModel> * const view : _views )
			{
				delete view;
			}
			_views.clear();
		}

		void BaseModel::init() { _addViews(); }

		template<typename T, typename>
		void BaseModel::_addView( T * const p_view )
		{
			p_view->setModel( *this );
			_views.push_back( p_view );
		}

		void BaseModel::_notifyViews( Event::EVENT_MODEL p_event ) const
		{
			for ( View::BaseView<BaseModel> * view : _views )
			{
				view->notify( p_event );
			}
		}

	} // namespace Model
} // namespace VTX
