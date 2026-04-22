#ifndef __VTX_UI_QT_TOOL_BAR_SELECTION__
#define __VTX_UI_QT_TOOL_BAR_SELECTION__

#include "ui/qt/action_registry.hpp"
#include "ui/qt/events.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/settings.hpp"
#include "ui/qt/widget/base_widget.hpp"
#include <QPointer>
#include <QMenu>
#include <QToolBar>
#include <QToolButton>
#include <app/action/selection.hpp>
#include <app/services.hpp>
#include <string_view>
#include <util/types.hpp>

namespace VTX::UI::QT::ToolBar
{

	class Selection : public Widget::BaseWidget<Selection, QToolBar>
	{
	  public:
		Selection( QWidget * p_parent ) : BaseWidget( p_parent )
		{
			setWindowTitle( "Selection" );

			// Granularity tool buttons.
			_granularityButton = new QToolButton( this );
			_granularityButton->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
			_granularityButton->setPopupMode( QToolButton::InstantPopup );
			_granularityButton->setText( "Selection" );

			QMenu * const menu = new QMenu( _granularityButton );
			const auto	  addRegisteredAction
				= [ menu ]( const std::string_view p_actionId )
			{
				if ( QAction * const action = UI_ACTIONS().getAction( p_actionId ) )
				{
					menu->addAction( action );
				}
			};

			addRegisteredAction( Action::Selection::SET_GRANULARITY_SYSTEM );
			addRegisteredAction( Action::Selection::SET_GRANULARITY_CHAIN );
			addRegisteredAction( Action::Selection::SET_GRANULARITY_RESIDUE );
			addRegisteredAction( Action::Selection::SET_GRANULARITY_ATOM );

			_granularityButton->setMenu( menu );
			addWidget( _granularityButton );
			_syncGranularityButton();
			App::HUB().connect<Events::SelectionGranularityChanged, &Selection::_onGranularityChanged>( this );

			// Lock selection.
			addAction( Action::Selection::LOCK );
		}

	  private:
		QPointer<QToolButton> _granularityButton;

		void _onGranularityChanged( const Events::SelectionGranularityChanged & ) { _syncGranularityButton(); }

		void _syncGranularityButton()
		{
			using Granularity = App::Action::Selection::E_GRANULARITY;

			const int granularity = SETTINGS().value( SETTING_KEY_GRANULARITY, 0 ).toInt();
			const auto syncAction
				= [ this, granularity ]( const std::string_view p_actionId, const Granularity p_granularity )
			{
				QAction * const action = UI_ACTIONS().getAction( p_actionId );
				if ( action == nullptr )
				{
					return;
				}

				const bool isCurrent = toUnderlying( p_granularity ) == granularity;
				action->setChecked( isCurrent );
				if ( isCurrent )
				{
					_granularityButton->setIcon( action->icon() );
					_granularityButton->setToolTip( action->toolTip() );
				}
			};

			syncAction( Action::Selection::SET_GRANULARITY_SYSTEM, Granularity::SYSTEM );
			syncAction( Action::Selection::SET_GRANULARITY_CHAIN, Granularity::CHAIN );
			syncAction( Action::Selection::SET_GRANULARITY_RESIDUE, Granularity::RESIDUE );
			syncAction( Action::Selection::SET_GRANULARITY_ATOM, Granularity::ATOM );
		}
	};

} // namespace VTX::UI::QT::ToolBar

#endif
