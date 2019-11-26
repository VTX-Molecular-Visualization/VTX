#include "base_model.hpp"

namespace VTX
{
	namespace Model
	{
		BaseModel::~BaseModel()
		{
			for ( std::shared_ptr<View::BaseView<BaseModel>> view : _views )
			{
				view.reset();
			}
			_views.clear();
		}

		void BaseModel::init() { _addViews(); }

		void BaseModel::_addView( const std::shared_ptr<View::BaseView<BaseModel>> p_view )
		{
			p_view->setModel( this );
			_views.push_back( p_view );
		}

		void BaseModel::_notifyViews( const Event::EVENT_MODEL p_event ) const
		{
			for ( std::shared_ptr<View::BaseView<BaseModel>> view : _views )
			{
				view->notify( p_event );
			}
		}

	} // namespace Model
} // namespace VTX
