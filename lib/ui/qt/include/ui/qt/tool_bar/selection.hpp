#ifndef __VTX_UI_QT_TOOL_BAR_SELECTION__
#define __VTX_UI_QT_TOOL_BAR_SELECTION__

#include "ui/qt/services.hpp"
#include "ui/qt/settings.hpp"
#include "ui/qt/style/icons.hpp"
#include "ui/qt/style/style_manager.hpp"
#include "ui/qt/widget/base_widget.hpp"
#include <QActionGroup>
#include <QPointer>
#include <QToolBar>
#include <QToolButton>
#include <app/action/selection.hpp>
#include <app/services.hpp>
#include <util/event_hub.hpp>

namespace VTX::UI::QT::ToolBar
{

	class Selection : public Widget::BaseWidget<Selection, QToolBar>
	{
	  public:
		Selection( QWidget * p_parent ) : BaseWidget( p_parent )
		{
			setWindowTitle( "Selection" );

			// Lock selection.
			auto * lockAction = addAction<Action::Selection::Lock>();
			lockAction->setCheckable( true );
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

			// Granularity tool buttons.
			_granularityButton = new QToolButton( this );
			_granularityButton->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
			_granularityButton->setPopupMode( QToolButton::InstantPopup );
			_granularityButton->setText( "Selection" );

			QMenu * menu		= new QMenu( _granularityButton );
			auto *	actionGroup = new QActionGroup( menu );
			actionGroup->setExclusive( true );

			using Granularity = App::Action::Selection::E_GRANULARITY;
			using namespace Style;

			QAction * systemAction
				= new QAction( STYLE().iconFromCodepoint( Icons::SYSTEM ), "System", _granularityButton );
			systemAction->setToolTip( "System" );
			systemAction->setData( int( Granularity::SYSTEM ) );
			systemAction->setCheckable( true );
			actionGroup->addAction( systemAction );

			QAction * chainAction
				= new QAction( STYLE().iconFromCodepoint( Icons::CHAIN ), "Chain", _granularityButton );
			chainAction->setToolTip( "Chain" );
			chainAction->setData( int( Granularity::CHAIN ) );
			chainAction->setCheckable( true );
			actionGroup->addAction( chainAction );

			QAction * residueAction
				= new QAction( STYLE().iconFromCodepoint( Icons::RESIDUE ), "Residue", _granularityButton );
			residueAction->setToolTip( "Residue" );
			residueAction->setData( int( Granularity::RESIDUE ) );
			residueAction->setCheckable( true );
			actionGroup->addAction( residueAction );

			QAction * atomAction = new QAction( STYLE().iconFromCodepoint( Icons::ATOM ), "Atom", _granularityButton );
			atomAction->setToolTip( "Atom" );
			atomAction->setData( int( Granularity::ATOM ) );
			atomAction->setCheckable( true );
			actionGroup->addAction( atomAction );

			menu->addAction( systemAction );
			menu->addAction( chainAction );
			menu->addAction( residueAction );
			menu->addAction( atomAction );

			connect(
				menu,
				&QMenu::triggered,
				this,
				[ this ]( QAction * const p_action )
				{
					const int granularity = p_action->data().toInt();
					SETTINGS().setValue( SETTING_KEY_GRANULARITY, granularity );
					App::HUB().trigger<Events::SelectionGranularityChanged>( granularity );
					_syncGranularityButton();
				}
			);

			_granularityButton->setMenu( menu );
			addWidget( _granularityButton );
			_syncGranularityButton();
			App::HUB().connect<Events::SelectionGranularityChanged, &Selection::_onGranularityChanged>( this );
		}

	  private:
		QPointer<QToolButton> _granularityButton;

		void _onGranularityChanged( const Events::SelectionGranularityChanged & ) { _syncGranularityButton(); }

		void _syncGranularityButton()
		{
			const int granularity = SETTINGS().value( SETTING_KEY_GRANULARITY, 0 ).toInt();
			for ( QAction * const action : _granularityButton->menu()->actions() )
			{
				const bool isCurrent = action->data().toInt() == granularity;
				action->setChecked( isCurrent );
				if ( isCurrent )
				{
					_granularityButton->setIcon( action->icon() );
					_granularityButton->setToolTip( action->toolTip() );
				}
			}
		}
	};

} // namespace VTX::UI::QT::ToolBar

#endif
