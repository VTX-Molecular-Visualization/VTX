#ifndef __VTX_UI_WIDGET_VIEW_ITEM_WIDGET__
#define __VTX_UI_WIDGET_VIEW_ITEM_WIDGET__

#include "base_manual_widget.hpp"
#include <QWidget>
#include <type_traits>

namespace VTX
{
	namespace Model
	{
		class BaseModel;
	}

	namespace UI::Widget
	{
		template<typename M, typename = std::enable_if<std::is_base_of<Model::BaseModel, M>::value>>
		class ViewItemWidget : public BaseManualWidget<QWidget>
		{
			VTX_WIDGET

		  public:
			ViewItemWidget( QWidget * p_parent ) : BaseManualWidget( p_parent ) {}

			inline M *	getModel() { return _model; }
			inline void setModel( M * const p_model )
			{
				_model = p_model;
				refresh();
			}

			virtual void notify( const Event::VTXEvent * const p_event ) {};

			virtual void refresh() {}

		  protected:
			M * _model = nullptr;
		};

	} // namespace UI::Widget
} // namespace VTX

#endif
