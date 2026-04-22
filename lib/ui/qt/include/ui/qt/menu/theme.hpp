#ifndef __VTX_UI_QT_MENU_THEME__
#define __VTX_UI_QT_MENU_THEME__

#include "ui/qt/events.hpp"
#include "ui/qt/style/style_manager.hpp"
#include "ui/qt/widget/base_widget.hpp"
#include <QAction>
#include <QActionGroup>
#include <QMenu>
#include <QPointer>
#include <QString>
#include <array>
#include <cstddef>
#include <util/types.hpp>

namespace VTX::UI::QT::Menu
{

	class Theme : public Widget::BaseWidget<Theme, QMenu>
	{
	  public:
		Theme( QWidget * );

	  private:
		QPointer<QActionGroup> _actionGroup;
		std::array<QPointer<QAction>, static_cast<std::size_t>( toUnderlying( Style::E_THEME::COUNT ) )>
			_themeActions;

		void _addThemeAction( const QString &, const Style::E_THEME );
		void _syncTheme();
		void _onThemeChanged( const Events::ThemeChanged & );
	};

} // namespace VTX::UI::QT::Menu

#endif
