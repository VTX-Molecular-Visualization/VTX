#ifndef __VTX_UI_WIDGET_RENDER_BASE_INTEGRATED_WIDGET__
#define __VTX_UI_WIDGET_RENDER_BASE_INTEGRATED_WIDGET__

#include "ui/widget/base_manual_widget.hpp"
#include <QWidget>
#include <type_traits>

namespace VTX::UI::Widget::Render
{
	class BaseIntegratedWidget
	{
	  public:
		virtual void updatePosition() = 0;
	};

	template<typename W, typename = std::enable_if<std::is_base_of<QWidget, W>::value>>
	class TemplatedIntegratedWidget : public VTX::UI::Widget::BaseManualWidget<W>, public BaseIntegratedWidget
	{
	  protected:
		TemplatedIntegratedWidget( QWidget * p_parent = nullptr ) : VTX::UI::Widget::BaseManualWidget<W>( p_parent ) {}
		void _setupUi( const QString & p_name ) override
		{
			VTX::UI::Widget::BaseManualWidget<W>::_setupUi( p_name );
			static_cast<QWidget *>( this )->setMouseTracking( true );
		}
	};

} // namespace VTX::UI::Widget::Render
#endif
