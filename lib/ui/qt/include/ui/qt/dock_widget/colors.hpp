#ifndef __VTX_UI_QT_DOCK_WIDGET_COLORS__
#define __VTX_UI_QT_DOCK_WIDGET_COLORS__

#include "ui/qt/core/base_dock_widget.hpp"
#include <QDockWidget>
#include <QPushButton>

namespace VTX::App::Component::Representation
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
		// const VTX::Core::Struct::ColorLayout * _currentLayout;

		std::vector<QPointer<QPushButton>> _buttons;

		App::Component::Representation::ColorLayout * _component;

		void _refreshColors();
		void _refreshColor( const size_t );

		void _changeColor( const size_t, const QColor & );
	};

} // namespace VTX::UI::QT::DockWidget

#endif
