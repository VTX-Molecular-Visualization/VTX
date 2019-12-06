#ifndef __VTX_BASE_MODEL__
#define __VTX_BASE_MODEL__

#ifdef _MSC_VER
#pragma once
#endif

#include "../view/base_view.hpp"
#include <vector>

namespace VTX
{
	namespace Model
	{
		class BaseModel
		{
		  public:
			BaseModel() = default;
			virtual ~BaseModel();

			virtual void init();
			bool		 isSelected() const { return _isSelected; }
			virtual void setSelected( const bool p_selected ) { _isSelected = p_selected; }

		  protected:
			void		 _addView( const std::shared_ptr<View::BaseView<BaseModel>> );
			virtual void _addViews() {};
			virtual void _notifyViews( const Event::EVENT_MODEL ) const final;
			virtual void _resetViews() final;

			bool													_isSelected = false;
			std::vector<std::shared_ptr<View::BaseView<BaseModel>>> _views
				= std::vector<std::shared_ptr<View::BaseView<BaseModel>>>();
		};
	} // namespace Model
} // namespace VTX
#endif
