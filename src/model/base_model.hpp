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
			using ViewSharedPtr				= std::shared_ptr<View::BaseView<BaseModel>>;
			using MapStringToViewSharedPtr	= std::map<const std::string, ViewSharedPtr>;
			using PairStringToViewSharedPtr = std::pair<const std::string, ViewSharedPtr>;

			BaseModel() = default;
			virtual ~BaseModel();

			virtual void				init();
			virtual const ViewSharedPtr getViewByName( const std::string & ) const final;
			bool						isSelected() const { return _isSelected; }
			virtual void				setSelected( const bool p_selected ) { _isSelected = p_selected; }

		  protected:
			void		 _addView( const ViewSharedPtr );
			virtual void _addViews() {};
			virtual void _notifyViews( const Event::EVENT_MODEL ) const final;
			void		 _removeView( const std::string & );
			virtual void _clearViews() final;

			bool					 _isSelected = false;
			MapStringToViewSharedPtr _views		 = MapStringToViewSharedPtr();
		};
	} // namespace Model
} // namespace VTX
#endif
