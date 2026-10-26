#ifndef __VTX_UI_QT_TOOL_SNAPSHOT__
#define __VTX_UI_QT_TOOL_SNAPSHOT__

#include "ui/qt/action_registry.hpp"
#include "ui/qt/actions.hpp"
#include "ui/qt/helper.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/style/style_manager.hpp"
#include "ui/qt/widget/base_widget.hpp"
#include "ui/qt/widget/editable_slider.hpp"
#include <QActionGroup>
#include <QButtonGroup>
#include <QToolBar>
#include <QWidgetAction>

namespace VTX::UI::QT::ToolBar
{

	class Snapshot : public Widget::BaseWidget<Snapshot, QToolBar>
	{
	  public:
		Snapshot( QWidget * const p_parent ) : BaseWidget( p_parent )
		{
			setWindowTitle( "Snapshot" );

			UI_ACTIONS().addTo( *this, Action::Snapshot::SNAPSHOT );
			UI_ACTIONS().addTo( *this, Action::Snapshot::EXPORT );

			return;
			auto * btn = Helper::makeQObject<QToolButton>( this );
			btn->setText( "Snapshot" );
			btn->setIcon( STYLE().iconFromCodepoint( Style::Icons::SNAPSHOT ) );
			btn->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
			btn->setPopupMode( QToolButton::MenuButtonPopup );

			connect( btn, &QToolButton::clicked, this, [] { qDebug() << "Action principale"; } );

			// Settings menu.
			auto * menu = Helper::makeQObject<QMenu>( btn );

			// Resolution selection.
			auto * containerRes = Helper::makeQObject<QWidget>( menu );
			auto * layoutRes	= Helper::makeQObject<QVBoxLayout>( containerRes );
			auto * titleRes		= Helper::makeQObject<QLabel>( "Resolution", containerRes );
			layoutRes->addWidget( titleRes );
			layoutRes->setContentsMargins( 8, 4, 8, 4 );

			auto * groupRes = Helper::makeQObject<QButtonGroup>( containerRes );
			auto * resA		= Helper::makeQObject<QRadioButton>( "Current", containerRes );
			auto * resB		= Helper::makeQObject<QRadioButton>( "800 x 600", containerRes );
			auto * resC		= Helper::makeQObject<QRadioButton>( "1920 x 1080", containerRes );

			resA->setChecked( true );

			groupRes->addButton( resA );
			groupRes->addButton( resB );
			groupRes->addButton( resC );

			layoutRes->addWidget( resA );
			layoutRes->addWidget( resB );
			layoutRes->addWidget( resC );

			auto * waRes = Helper::makeQObject<QWidgetAction>( menu );
			waRes->setDefaultWidget( containerRes );
			menu->addAction( waRes );

			//////////////////////
			menu->addSeparator();

			// Transparency slider.
			Widget::EditableSlider * slider
				= Helper::makeQObject<Widget::EditableSlider>( Qt::Orientation::Horizontal, menu );
			slider->setValue( 0 );
			slider->setSuffix( "%" );
			slider->setMinimum( 0 );
			slider->setMaximum( 100 );

			auto * waTrans = Helper::makeQObject<QWidgetAction>( menu );
			waTrans->setDefaultWidget( slider );
			menu->addAction( waTrans );

			// Display.
			btn->setMenu( menu );
			this->addWidget( btn );
		}

	  private:
	};

} // namespace VTX::UI::QT::ToolBar

#endif
