#ifndef __VTX_UI_QT_TOOL_SNAPSHOT__
#define __VTX_UI_QT_TOOL_SNAPSHOT__

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
		Snapshot( QWidget * p_parent ) : BaseWidget( p_parent )
		{
			setWindowTitle( "Snapshot" );

			addAction( Action::Snapshot::SNAPSHOT );
			// addAction( Action::Snapshot::EXPORT );

			return;
			auto * btn = new QToolButton;
			btn->setText( "Snapshot" );
			btn->setIcon( STYLE().iconFromCodepoint( Style::Icons::SNAPSHOT ) );
			btn->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
			btn->setPopupMode( QToolButton::MenuButtonPopup );

			connect( btn, &QToolButton::clicked, this, [] { qDebug() << "Action principale"; } );

			// Settings menu.
			auto * menu = new QMenu( btn );

			// Resolution selection.
			auto * containerRes = new QWidget;
			auto * layoutRes	= new QVBoxLayout( containerRes );
			auto * titleRes		= new QLabel( "Resolution" );
			layoutRes->addWidget( titleRes );
			layoutRes->setContentsMargins( 8, 4, 8, 4 );

			auto * groupRes = new QButtonGroup( containerRes );
			auto * resA		= new QRadioButton( "Current" );
			auto * resB		= new QRadioButton( "800 x 600" );
			auto * resC		= new QRadioButton( "1920 x 1080" );

			resA->setChecked( true );

			groupRes->addButton( resA );
			groupRes->addButton( resB );
			groupRes->addButton( resC );

			layoutRes->addWidget( resA );
			layoutRes->addWidget( resB );
			layoutRes->addWidget( resC );

			auto * waRes = new QWidgetAction( menu );
			waRes->setDefaultWidget( containerRes );
			menu->addAction( waRes );

			//////////////////////
			menu->addSeparator();

			// Transparency slider.
			Widget::EditableSlider * slider = new Widget::EditableSlider( Qt::Orientation::Horizontal, menu );
			slider->setValue( 0 );
			slider->setSuffix( "%" );
			slider->setMinimum( 0 );
			slider->setMaximum( 100 );

			auto * waTrans = new QWidgetAction( menu );
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
