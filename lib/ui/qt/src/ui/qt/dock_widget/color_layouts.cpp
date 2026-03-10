#include "ui/qt/dock_widget/color_layouts.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/settings.hpp"
#include "ui/qt/style/icons.hpp"
#include "ui/qt/style/style_manager.hpp"
#include "ui/qt/widget/library/color_layout.hpp"
#include <util/factories.hpp>
namespace
{
	constexpr std::string_view SETTING_KEY_HIDE = "colors/hide_non_common";
}

namespace VTX::UI::QT::DockWidget
{

	ColorLayouts::ColorLayouts( QWidget * p_parent ) : BaseDockWidget( p_parent, "Colors" )
	{
		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
		setWindowIcon( STYLE().iconFromCodepoint( Style::Icons::COLOR_LAYOUT ) );

		// Checkbox to hide non usual items.
		_checkBoxHide = new QCheckBox( "Hide non usual", _root );
		_layout->addWidget( _checkBoxHide );

		auto * const colorLayoutWidget = new Widget::Library::ColorLayout( this );
		_layout->addWidget( colorLayoutWidget );

		_layout->addSpacerItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding ) );

		connect(
			_checkBoxHide,
			&QCheckBox::checkStateChanged,
			[ colorLayoutWidget ]( const int p_state )
			{
				const bool hide = p_state == Qt::Checked;
				colorLayoutWidget->refreshVisibility( hide );
			}
		);

		_checkBoxHide->setChecked( SETTINGS().value( SETTING_KEY_HIDE, true ).toBool() );
	}

	ColorLayouts::~ColorLayouts() { SETTINGS().setValue( SETTING_KEY_HIDE, _checkBoxHide->isChecked() ); }
} // namespace VTX::UI::QT::DockWidget
