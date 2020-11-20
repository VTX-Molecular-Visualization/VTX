#ifndef __VTX_UI_WIDGET_VIEW_ITEM_WIDGET__
#define __VTX_UI_WIDGET_VIEW_ITEM_WIDGET__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_manual_widget.hpp"
#include "model/base_model.hpp"
#include <QWidget>
#include <type_traits>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			template<typename M, typename = std::enable_if<std::is_base_of<Model::BaseModel, M>::value>>
			class ViewItemWidget : public BaseManualWidget<QWidget>
			{
				VTX_MANUAL_WIDGET_DECLARATION

			  public:
				ViewItemWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) {};

				inline M *	getModel() { return _model; };
				inline void setModel( M * const p_model )
				{
					_model = p_model;
					refresh();
				};

				virtual void refresh() {};

			  protected:
				M * _model = nullptr;
			};

		} // namespace Widget
	}	  // namespace UI
} // namespace VTX

#endif
