#ifndef VTX_UI_QT_TOOL_BAR_SELECTION_HPP
#define VTX_UI_QT_TOOL_BAR_SELECTION_HPP

#include "ui/qt/action_registry.hpp"
#include "ui/qt/actions.hpp"
#include "ui/qt/events.hpp"
#include "ui/qt/helper.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/settings.hpp"
#include "ui/qt/widget/base_widget.hpp"
#include <QMenu>
#include <QPointer>
#include <QToolBar>
#include <QToolButton>
#include <app/action/selection.hpp>
#include <app/services.hpp>
#include <string_view>
#include <util/event_hub.hpp>
#include <util/types.hpp>
#include <utility>

namespace VTX::UI::QT::ToolBar
{

	class Selection : public Widget::BaseWidget<Selection, QToolBar>
	{
	  public:
		Selection( QWidget * const p_parent ) : BaseWidget( p_parent )
		{
			setWindowTitle( "Selection" );

			// Granularity tool buttons.
			_granularityButton = Helper::makeQObject<QToolButton>( this );
			_granularityButton->setToolButtonStyle( Qt::ToolButtonTextUnderIcon );
			_granularityButton->setPopupMode( QToolButton::InstantPopup );
			_granularityButton->setText( "Selection" );

			QMenu * const menu				  = Helper::makeQObject<QMenu>( _granularityButton );
			const auto	  addRegisteredAction = [ menu ]( const std::string_view p_actionId )
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
			_granularityChangedConnection
				= App::HUB().connect<Events::SelectionGranularityChanged, &Selection::_onGranularityChanged>( this );

			// Lock selection.
			UI_ACTIONS().addTo( *this, Action::Selection::LOCK );
		}

	  private:
		QPointer<QToolButton> _granularityButton;

		void _onGranularityChanged( const Events::SelectionGranularityChanged & ) { _syncGranularityButton(); }

		void _syncGranularityButton()
		{
			using Granularity = App::Action::Selection::E_GRANULARITY;

			const int  granularity = SETTINGS().value( SETTING_KEY_GRANULARITY, 0 ).toInt();
			const auto syncAction
				= [ this, granularity ]( const std::string_view p_actionId, const Granularity p_granularity )
			{
				QAction * const action = UI_ACTIONS().getAction( p_actionId );
				if ( action == nullptr )
				{
					return;
				}

				const bool isCurrent = std::cmp_equal( toUnderlying( p_granularity ), granularity );
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

		VTX::Util::EventHub::ScopedConnection _granularityChangedConnection;
	};

} // namespace VTX::UI::QT::ToolBar

#endif
