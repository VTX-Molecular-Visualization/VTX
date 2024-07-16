#ifndef __VTX_UI_QT_DOCK_WIDGET_OPTIONS__
#define __VTX_UI_QT_DOCK_WIDGET_OPTIONS__

#include "qt/base_widget.hpp"
#include <QCheckBox>
#include <QDockWidget>
#include <QGroupBox>
#include <QToolButton>
#include <QVBoxLayout>

namespace VTX::UI::QT::DockWidget
{

	class Options : public BaseWidget<Options, QDockWidget>, public Savable
	{
	  public:
		Options( QWidget * p_parent ) : BaseWidget<Options, QDockWidget>( "Options", p_parent )
		{
			setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

			// Create layout.
			auto * widget = new QWidget( this );
			auto * layout = new QVBoxLayout( widget );
			layout->setContentsMargins( 0, 0, 0, 0 );
			layout->setSizeConstraint( QLayout::SetNoConstraint );
			setWidget( widget );

			auto * gbInterface	   = new QGroupBox( "Interface", widget );
			auto * layoutInterface = new QVBoxLayout( widget );

			// Use action?

			auto * aToolbarText = ACTION<Action::ShowToolBarText>();
			aToolbarText->setCheckable( true );
			aToolbarText->setChecked( true );
			QObject::connect(
				aToolbarText,
				&QAction::toggled,
				[ aToolbarText ]()
				{
					VTX_DEBUG( "Show toolbar text: {}", aToolbarText->isChecked() );
					WIDGET<Widget::MainWindow>()->setToolButtonStyle(
						aToolbarText->isChecked() ? Qt::ToolButtonTextUnderIcon : Qt::ToolButtonIconOnly
					);
				}
			);

			// auto * cbToolbarText = new QCheckBox( "Show toolbar text", gbInterface );

			auto * tbToolbarText = new QToolButton( gbInterface );
			tbToolbarText->setDefaultAction( aToolbarText );
			tbToolbarText->setToolButtonStyle( Qt::ToolButtonTextBesideIcon );
			tbToolbarText->setCheckable( true );

			layoutInterface->addWidget( tbToolbarText );

			gbInterface->setLayout( layoutInterface );
			layout->addWidget( gbInterface );

			// Set layout size adapted to content.
			layout->setSizeConstraint( QLayout::SetFixedSize );
			// layoutInterface->setSizeConstraint( QLayout::SetFixedSize );
		}

		virtual ~Options() {}

		void save() override { SETTINGS.setValue( "showToolBarText", ACTION<Action::ShowToolBarText>()->isChecked() ); }

		void restore() override
		{
			ACTION<Action::ShowToolBarText>()->setChecked( SETTINGS.value( "showToolBarText", true ).toBool() );
		}

	  private:
	};

} // namespace VTX::UI::QT::DockWidget

#endif
