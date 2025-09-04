#include "ui/qt/dock_widget/color_layouts.hpp"
#include "ui/qt/helper.hpp"
#include "ui/qt/layout/flow_layout.hpp"
#include <QColor>
#include <QColorDialog>
#include <QGroupBox>
#include <QLineEdit>
#include <QVBoxLayout>
#include <app/action/color_layout.hpp>
#include <app/application/scene.hpp>
#include <app/component/representation/color_layout.hpp>
#include <core/chemdb/chain.hpp>
#include <core/chemdb/color_layout.hpp>
#include <core/struct/color_layout.hpp>
#include <util/chrono.hpp>
#include <util/color/rgba.hpp>
#include <util/string.hpp>

namespace VTX::UI::QT::DockWidget
{

	ColorLayouts::ColorLayouts( QWidget * p_parent ) : Core::BaseDockWidget<ColorLayouts>( "Colors", p_parent )
	{
		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

		// Checkbox to hide non usual items.
		_checkBoxHide = new QCheckBox( "Hide non usual", _root );
		_layout->addWidget( _checkBoxHide );
		/*
		connect(
			_checkBoxHide,
			&QCheckBox::checkStateChanged,
			[ this ]( const int p_state )
			{
				using namespace VTX::Core::ChemDB;
				const bool hide = p_state == Qt::Checked;
				_refreshButtonVisibility(
					hide, ColorLayout::LAYOUT_OFFSET_ATOMS, ColorLayout::LAYOUT_COUNT_ATOMS, Atom::SYMBOL_IS_COMMON
				);
				_refreshButtonVisibility(
					hide,
					ColorLayout::LAYOUT_OFFSET_RESIDUES,
					ColorLayout::LAYOUT_COUNT_RESIDUES,
					Residue::SYMBOL_IS_COMMON
				);
			}
		);
		*/
	}

	void ColorLayouts::save() { SETTINGS.setValue( _SETTING_KEY_HIDE, _checkBoxHide->isChecked() ); }

	void ColorLayouts::restore() { _checkBoxHide->setChecked( SETTINGS.value( _SETTING_KEY_HIDE, true ).toBool() ); }
} // namespace VTX::UI::QT::DockWidget
