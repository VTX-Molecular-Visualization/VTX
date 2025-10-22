#include "ui/qt/dock_widget/color_layouts.hpp"
#include "ui/qt/widget/library/color_layout.hpp"
#include <util/factories.hpp>

namespace VTX::UI::QT::DockWidget
{

	ColorLayouts::ColorLayouts( QWidget * p_parent ) : BaseDockWidget( "Colors", p_parent )
	{
		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

		// Checkbox to hide non usual items.
		_checkBoxHide = new QCheckBox( "Hide non usual", _root );
		_layout->addWidget( _checkBoxHide );

		auto * const colorLayoutWidget = Util::Factories::newInit<Widget::Library::ColorLayout>( this );
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
	}

	void ColorLayouts::save( Settings & p_settings )
	{
		p_settings.setValue( _SETTING_KEY_HIDE, _checkBoxHide->isChecked() );
	}

	void ColorLayouts::restore( const Settings & p_settings )
	{
		_checkBoxHide->setChecked( p_settings.value( _SETTING_KEY_HIDE, true ).toBool() );
	}
} // namespace VTX::UI::QT::DockWidget
