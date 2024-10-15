#ifndef __VTX_UI_QT_DOCK_WIDGET_COLORS__
#define __VTX_UI_QT_DOCK_WIDGET_COLORS__

#include "ui/qt/core/base_dock_widget.hpp"
#include <QDockWidget>
#include <QPushButton>

namespace VTX::Core::Struct
{
	class ColorLayout;
}

namespace VTX::UI::QT::DockWidget
{

	class Colors : public Core::BaseDockWidget<Colors>
	{
	  public:
		Colors( QWidget * p_parent );

	  private:
		std::vector<QPointer<QPushButton>> _buttons;

		void _createGroupBox( const std::string_view, const size_t, const size_t );

		void _refreshColors( const VTX::Core::Struct::ColorLayout & );
		void _refreshColor( const VTX::Core::Struct::ColorLayout &, const size_t );

		void _changeColor( const size_t, const QColor & );
	};

} // namespace VTX::UI::QT::DockWidget

#endif
