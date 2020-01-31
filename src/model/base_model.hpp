#ifndef __VTX_BASE_MODEL__
#define __VTX_BASE_MODEL__

#ifdef _MSC_VER
#pragma once
#endif

#include "../generic/has_collection.hpp"
#include "../math/transform.hpp"
#include "../view/base_view.hpp"
#include <map>

namespace VTX
{
	namespace Model
	{
		class BaseModel : public Generic::HasCollection<View::BaseView<BaseModel>>
		{
		  public:
			BaseModel();
			virtual ~BaseModel() = default;

			uint		 getId() const { return _id; }
			void		 setId( const uint p_id ) { _id = p_id; }
			bool		 isSelected() const { return _isSelected; }
			virtual void setSelected( const bool p_selected ) { _isSelected = p_selected; }

		  protected:
			virtual void _notifyViews( const Event::EVENT_MODEL ) final;
			virtual void _deleteView( const std::string & p_viewName )
			{
				View::BaseView<BaseModel> * view = (View::BaseView<BaseModel> *)_getItem( p_viewName );
				removeItem( p_viewName );
				delete view;
			}

			uint _id		 = INVALID_ID;
			bool _isSelected = false;

		  private:
			static uint _COUNTER;
		};
	} // namespace Model
} // namespace VTX
#endif
