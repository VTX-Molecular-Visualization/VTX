#ifndef __VTX_UI_QT_DOCK_WIDGET_COLORS__
#define __VTX_UI_QT_DOCK_WIDGET_COLORS__

#include "ui/qt/core/base_dock_widget.hpp"
#include "ui/qt/settings.hpp"
#include <QCheckbox>
#include <QDockWidget>
#include <QPushButton>

namespace VTX::Core::Struct
{
	class ColorLayout;
}

namespace VTX::UI::QT::DockWidget
{

	class Colors : public Core::BaseDockWidget<Colors>, public Savable
	{
	  public:
		Colors( QWidget * p_parent );

		void save() override;
		void restore() override;

	  private:
		inline static const QString _SETTING_KEY_HIDE = "colors/hide_non_common";

		std::vector<QPointer<QPushButton>> _buttons;
		QPointer<QCheckBox>				   _checkBoxHide;

		void _createGroupBox(
			const std::string_view,
			const VTX::Core::Struct::ColorLayout &,
			const size_t,
			const size_t
		);

		void _refreshColors( const VTX::Core::Struct::ColorLayout & );
		void _refreshColor( const VTX::Core::Struct::ColorLayout &, const size_t );
		void _refreshButtonVisibility( const int );

		void _changeColor( const size_t, const QColor & );
	};

} // namespace VTX::UI::QT::DockWidget

#endif
