#include "ui/qt/dock_widget/inspector.hpp"
#include "ui/qt/events.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/settings.hpp"
#include "ui/qt/style/icons.hpp"
#include "ui/qt/style/style_manager.hpp"
#include "ui/qt/widget/main_window.hpp"
#include "ui/qt/widget/transform.hpp"
#include <QFontDatabase>
#include <QLabel>
#include <QToolBar>
#include <QToolButton>
#include <app/action/selection.hpp>
#include <app/services.hpp>
#include <util/event_hub.hpp>

namespace VTX::UI::QT::DockWidget
{
	Inspector::Inspector( QWidget * p_parent ) : BaseDockWidget( p_parent, "Selection" )
	{
		setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
		setWindowIcon( STYLE().iconFromCodepoint( Style::Icons::SELECTION ) );

		///////////////
		_layout->addWidget( new Widget::Transform( this ) );
		///////////////

		// Selection toolbar.
		auto * toolbar = new QToolBar( this );
		toolbar->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
		// toolbar->setIconSize( QSize( 18, 18 ) );
		auto * lockAction = Application::getAction<Action::Selection::Lock>();
		lockAction->setCheckable( true );
		toolbar->addAction( lockAction );
		toolbar->addAction( Application::getAction<Action::Selection::Save>() );
		toolbar->addAction( Application::getAction<Action::Selection::Clear>() );
		_layout->addWidget( toolbar );

		// Picking granularity combobox.
		auto * cbPickingGranularity = new QComboBox( this );
		cbPickingGranularity->addItem(
			"Atom", QVariant( toUnderlying( App::Action::Selection::E_GRANULARITY::ATOM ) )
		);
		cbPickingGranularity->addItem(
			"Residue", QVariant( toUnderlying( App::Action::Selection::E_GRANULARITY::RESIDUE ) )
		);
		cbPickingGranularity->addItem(
			"Chain", QVariant( toUnderlying( App::Action::Selection::E_GRANULARITY::CHAIN ) )
		);
		cbPickingGranularity->addItem(
			"System", QVariant( toUnderlying( App::Action::Selection::E_GRANULARITY::SYSTEM ) )
		);
		_layout->addWidget( cbPickingGranularity );

		// Selection list widget.
		_selectionListWidget = new Widget::Selection( this );
		_layout->addWidget( _selectionListWidget );

		//_layout->addSpacerItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding ) );

		lockAction->setChecked( SETTINGS().value( SETTING_KEY_LOCK_SELECTION, false ).toBool() );

		connect(
			lockAction,
			&QAction::toggled,
			this,
			[]( const bool p_locked )
			{
				SETTINGS().setValue( SETTING_KEY_LOCK_SELECTION, p_locked );
				App::HUB().trigger<Events::SelectionLocked>( p_locked );
			}
		);

		const int index = cbPickingGranularity->findData( SETTINGS().value( SETTING_KEY_GRANULARITY, 0 ).toInt() );
		if ( index != -1 )
		{
			cbPickingGranularity->setCurrentIndex( index );
		}

		connect(
			cbPickingGranularity,
			&QComboBox::currentIndexChanged,
			this,
			[ cbPickingGranularity ]( const int )
			{ SETTINGS().setValue( SETTING_KEY_GRANULARITY, cbPickingGranularity->currentData() ); }
		);
	}

} // namespace VTX::UI::QT::DockWidget
