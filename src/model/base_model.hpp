#ifndef __VTX_BASE_MODEL__
#define __VTX_BASE_MODEL__

#ifdef _MSC_VER
#pragma once
#endif

#include "../math/transform.hpp"
#include "../view/base_view.hpp"
#include <map>

namespace VTX
{
	namespace Model
	{
		class BaseModel
		{
		  public:
			using ViewSharedPtr			  = std::shared_ptr<View::BaseView<BaseModel>>;
			using MapEnumToViewSharedPtr  = std::map<View::VIEW_NAME, ViewSharedPtr>;
			using PairEnumToViewSharedPtr = std::pair<View::VIEW_NAME, ViewSharedPtr>;

			BaseModel();
			virtual ~BaseModel();

			uint						getId() const { return _id; }
			void						setId( const uint p_id ) { _id = p_id; }
			virtual void				init();
			virtual const ViewSharedPtr getViewByName( const View::VIEW_NAME ) const final;
			bool						isSelected() const { return _isSelected; }
			virtual void				setSelected( const bool p_selected ) { _isSelected = p_selected; }

		  protected:
			void		 _addView( const ViewSharedPtr );
			virtual void _addViews() {};
			virtual void _notifyViews( const Event::EVENT_MODEL ) const final;
			void		 _removeView( const View::VIEW_NAME );
			virtual void _clearViews() final;

			uint				   _id		   = INVALID_ID;
			bool				   _isSelected = false;
			MapEnumToViewSharedPtr _views	   = MapEnumToViewSharedPtr();

		  private:
			static uint _COUNTER;
		};
	} // namespace Model
} // namespace VTX
#endif
