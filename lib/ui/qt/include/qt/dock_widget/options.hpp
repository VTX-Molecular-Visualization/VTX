#ifndef __VTX_UI_QT_DOCK_WIDGET_OPTIONS__
#define __VTX_UI_QT_DOCK_WIDGET_OPTIONS__

#include "qt/base_widget.hpp"
#include <QCheckBox>
#include <QDockWidget>
#include <QGroupBox>
#include <QVBoxLayout>

namespace VTX::UI::QT::DockWidget
{

	class Options : public BaseWidget<Options, QDockWidget>
	{
	  public:
		Options( QWidget * p_parent ) : BaseWidget<Options, QDockWidget>( "Options", p_parent )
		{
			setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );

			// Create grid layout.
			auto * widget = new QWidget( this );
			auto * layout = new QVBoxLayout( widget );
			widget->setLayout( layout );
			setWidget( widget );

			auto * gbInterface	   = new QGroupBox( "Interface", widget );
			auto * layoutInterface = new QVBoxLayout( widget );

			// Use action?
			auto * aToolbarText = ACTION<Action::ShowToolbarText>();
			aToolbarText->setCheckable( true );
			aToolbarText->setChecked( true );
			QObject::connect(
				aToolbarText,
				&QAction::triggered,
				[ aToolbarText ]() { VTX_INFO( "Show toolbar text: {}", aToolbarText->isChecked() ); }
			);
			widget->addAction( aToolbarText );

			// layoutInterface->addWidget( aToolbarText );
			gbInterface->setLayout( layoutInterface );
			layout->addWidget( gbInterface );

			// Set layout size adapted to content.
			layout->setSizeConstraint( QLayout::SetMinAndMaxSize );
			layoutInterface->setSizeConstraint( QLayout::SetMinAndMaxSize );
		}

		virtual ~Options() {}
	};

} // namespace VTX::UI::QT::DockWidget

#endif
