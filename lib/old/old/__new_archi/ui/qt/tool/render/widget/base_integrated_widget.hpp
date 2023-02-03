#ifndef __VTX_UI_QT_WIDGET_RENDER_BASE_INTEGRATED_WIDGET__
#define __VTX_UI_QT_WIDGET_RENDER_BASE_INTEGRATED_WIDGET__

#include "__new_archi/ui/qt/widget/base_manual_widget.hpp"
#include <QWidget>
#include <type_traits>

namespace VTX::UI::QT::Tool::Render::Widget
{
	class BaseIntegratedWidget
	{
	  public:
		virtual void updatePosition() = 0;
	};

	template<typename W, typename = std::enable_if<std::is_base_of<QWidget, W>::value>>
	class TemplatedIntegratedWidget : public VTX::UI::QT::Widget::BaseManualWidget<W>, public BaseIntegratedWidget
	{
	  protected:
		TemplatedIntegratedWidget( QWidget * p_parent = nullptr ) : VTX::UI::QT::Widget::BaseManualWidget<W>( p_parent )
		{
		}
		void _setupUi( const QString & p_name ) override
		{
			VTX::UI::QT::Widget::BaseManualWidget<W>::_setupUi( p_name );
			static_cast<QWidget *>( this )->setMouseTracking( true );
		}
	};

} // namespace VTX::UI::QT::Tool::Render::Widget
#endif
