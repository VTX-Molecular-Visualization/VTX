#include "ui/qt/action_registry.hpp"
#include "ui/qt/application.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/style/style_manager.hpp"
#include <QActionGroup>
#include <QApplication>
#include <QIcon>
#include <QKeySequence>
#include <QVariant>
#include <QWidget>
#include <util/logger.hpp>
#include <utility>
#include <variant>

namespace VTX::UI::QT
{

	QAction * ActionRegistry::registerAction( const App::UI::DescAction & p_action, const ActionHandler p_handler )
	{
		const std::string actionId = key( p_action );
		if ( actionId.empty() )
		{
			VTX_ERROR( "Unable to register UI action without key" );
			return nullptr;
		}

		const bool hasHandler = bool( p_handler );
		if ( hasHandler )
		{
			_handlers[ actionId ] = std::move( p_handler );
		}

		if ( QAction * const action = getAction( actionId ) )
		{
			if ( hasHandler )
			{
				_bindTrigger( *action, actionId );
			}
			return action;
		}

		QAction * const action = _createAction( p_action, actionId );
		if ( hasHandler )
		{
			_bindTrigger( *action, actionId );
		}
		return action;
	}

	QAction * ActionRegistry::getAction( const std::string_view p_key ) const
	{ return Q_APP()->findChild<QAction *>( QString::fromUtf8( p_key.data(), int( p_key.size() ) ) ); }

	void ActionRegistry::trigger( const std::string_view p_key, ActionParams p_params ) const
	{
		const std::string actionId { p_key };
		const auto		  it = _handlers.find( actionId );
		if ( it == _handlers.end() )
		{
			VTX_WARNING( "No UI action handler registered for: {}", p_key );
			return;
		}

		it->second( ActionContext { p_key, std::move( p_params ) } );
	}

	bool ActionRegistry::isChecked( const std::string_view p_key ) const
	{
		QAction * const action = getAction( p_key );
		if ( action == nullptr )
		{
			VTX_ERROR( "Unable to read checked state of unregistered UI action: {}", p_key );
			return false;
		}

		return action->isChecked();
	}

	void ActionRegistry::setChecked( const std::string_view p_key, const bool p_checked ) const
	{
		QAction * const action = getAction( p_key );
		if ( action == nullptr )
		{
			VTX_ERROR( "Unable to set checked state of unregistered UI action: {}", p_key );
			return;
		}

		action->setChecked( p_checked );
	}

	void ActionRegistry::installShortcuts( QWidget & p_host ) const
	{
		for ( QAction * const action : Q_APP()->findChildren<QAction *>() )
		{
			if ( action->shortcut().isEmpty() || p_host.actions().contains( action ) )
			{
				continue;
			}

			p_host.addAction( action );
		}
	}

	QAction * ActionRegistry::addMenuAction( QMenu & p_menu, const std::string_view p_key )
	{
		QAction * const action = getAction( p_key );
		if ( action == nullptr )
		{
			VTX_ERROR( "Unable to add unregistered UI action to menu: {}", p_key );
			return nullptr;
		}

		p_menu.addAction( action );
		return action;
	}

	QAction * ActionRegistry::addToolBarAction( QToolBar & p_toolbar, const std::string_view p_key )
	{
		QAction * const action = getAction( p_key );
		if ( action == nullptr )
		{
			VTX_ERROR( "Unable to add unregistered UI action to toolbar: {}", p_key );
			return nullptr;
		}

		p_toolbar.addAction( action );
		return action;
	}

	std::string ActionRegistry::key( const App::UI::DescAction & p_action ) { return p_action.key; }

	QAction * ActionRegistry::_createAction( const App::UI::DescAction & p_action, const std::string_view p_key )
	{
		auto * const qAction = new QAction( Q_APP() );
		qAction->setObjectName( QString::fromUtf8( p_key.data(), int( p_key.size() ) ) );

		VTX_TRACE( "UI action created: {}", p_key );

		qAction->setText( QString::fromStdString( p_action.name ) );
		qAction->setCheckable( p_action.checkable );

		if ( p_action.group )
		{
			auto * qActionGroup = Q_APP()->findChild<QActionGroup *>( QString::fromStdString( *p_action.group ) );
			if ( not qActionGroup )
			{
				qActionGroup = new QActionGroup( Q_APP() );
				qActionGroup->setObjectName( QString::fromStdString( *p_action.group ) );
			}

			qAction->setCheckable( true );
			qActionGroup->addAction( qAction );
		}

		if ( p_action.tip )
		{
			QString tip = QString::fromStdString( *p_action.tip );
			if ( p_action.shortcut )
			{
				tip.append( " (" + QString::fromStdString( *p_action.shortcut ) + ")" );
			}

			qAction->setStatusTip( tip );
			qAction->setToolTip( tip );
			qAction->setWhatsThis( tip );
		}

		if ( p_action.icon )
		{
			if ( std::holds_alternative<int>( *p_action.icon ) )
			{
				qAction->setIcon( STYLE().iconFromCodepoint( std::get<int>( *p_action.icon ) ) );
			}
			else if ( std::holds_alternative<std::string>( *p_action.icon ) )
			{
				qAction->setIcon( QIcon( QString::fromStdString( ":/" + std::get<std::string>( *p_action.icon ) ) ) );
			}
			else
			{
				VTX_ERROR( "Invalid icon type for action: {}", p_key );
			}
		}

		if ( p_action.shortcut )
		{
			qAction->setShortcut( QKeySequence( QString::fromStdString( *p_action.shortcut ) ) );
			qAction->setShortcutContext( Qt::ApplicationShortcut );
		}

		return qAction;
	}

	void ActionRegistry::_bindTrigger( QAction & p_action, const std::string_view p_key )
	{
		static constexpr const char * BOUND_PROPERTY = "vtxActionRegistryBound";
		if ( p_action.property( BOUND_PROPERTY ).toBool() )
		{
			return;
		}

		p_action.setProperty( BOUND_PROPERTY, true );

		const std::string actionId( p_key );
		QObject::connect( &p_action, &QAction::triggered, &p_action, [ this, actionId ]() { trigger( actionId ); } );
	}
} // namespace VTX::UI::QT
