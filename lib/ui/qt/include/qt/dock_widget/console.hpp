#ifndef __VTX_UI_QT_DOCK_WIDGET_CONSOLE__
#define __VTX_UI_QT_DOCK_WIDGET_CONSOLE__

#include "qt/base_widget.hpp"
#include <QDockWidget>
#include <QListWidget>
#include <QMenu>
#include <QVBoxLayout>
#include <app/vtx_app.hpp>
#include <mutex>
#include <util/enum.hpp>
#include <util/logger.hpp>

namespace VTX::UI::QT::DockWidget
{

	class Console : public BaseWidget<Console, QDockWidget>
	{
	  public:
		Console( QWidget * p_parent ) : BaseWidget<Console, QDockWidget>( "Console", p_parent )
		{
			setAllowedAreas( Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea );

			QWidget * const mainWidget = new QWidget( this );
			_listWidget				   = new QListWidget( this );
			_listWidget->setContextMenuPolicy( Qt::ContextMenuPolicy::CustomContextMenu );

			// Set widget.
			QVBoxLayout * const mainLayout = new QVBoxLayout( mainWidget );
			mainLayout->setContentsMargins( 0, 0, 0, 0 );
			mainLayout->addWidget( _listWidget );
			setWidget( mainWidget );

			connect(
				_listWidget,
				&QListWidget::customContextMenuRequested,
				this,
				[ & ]( const QPoint & p_pos )
				{
					QMenu			menu( this );
					QAction * const clearAction = new QAction( "Clear", &menu );
					connect( clearAction, &QAction::triggered, this, &Console::clear );
					menu.addAction( clearAction );
					menu.exec( _listWidget->mapToGlobal( p_pos ) );
				}
			);

			LOGGER().onPrintLog += [ this ]( const Util::LogInfo & p_logInfo ) { _appendLog( p_logInfo ); };
		}

		virtual ~Console() {}

		void clear()
		{
			_listWidgetMutex.lock();
			_listWidget->clear();
			_listWidgetMutex.unlock();
		}

	  private:
		const int _LOG_COUNT = 50;

		QListWidget * _listWidget	   = nullptr;
		std::mutex	  _listWidgetMutex = std::mutex();

		void _appendLog( const Util::LogInfo & p_logInfo )
		{
			const std::string message = fmt::format( "[{}] {}", p_logInfo.date, p_logInfo.message );

			QListWidgetItem * const newItem = new QListWidgetItem( QString::fromStdString( message ) );

			// TODO: use palellete color.
			// newItem->setData( Qt::ForegroundRole, _getMessageColor( p_logInfo.level ) );
			newItem->setFlags( Qt::ItemFlag::ItemNeverHasChildren );

			_listWidgetMutex.lock();
			_listWidget->addItem( newItem );
			_listWidgetMutex.unlock();

			if ( _listWidget->count() > _LOG_COUNT )
				_flush();

			// _appendLog can be called from a different thread
			// Qt events are not thread safe and need to be called from the main thread
			// We delayed the scrollToBottom on main thread at the end of frame.
			APP().onEndOfFrameOneShot += [ this ]()
			{
				_listWidgetMutex.lock();
				_listWidget->scrollToBottom();
				_listWidgetMutex.unlock();
			};
		}

		void _flush()
		{
			QListWidgetItem * const itemToRemove = _listWidget->takeItem( 0 );
			_listWidgetMutex.lock();
			_listWidget->removeItemWidget( itemToRemove );
			_listWidgetMutex.unlock();
			delete itemToRemove;
		}

		QColor _getMessageColor( const Util::LOG_LEVEL p_level )
		{
			QColor res;
			// TODO: use palellete color.
			/*
			switch ( p_level )
			{
			 case Util::LOG_LEVEL::LOG_DEBUG: res = CONSOLE_DEBUG_COLOR; break;
			 case Util::LOG_LEVEL::LOG_INFO: res = CONSOLE_INFO_COLOR; break;
			 case Util::LOG_LEVEL::LOG_WARNING: res = CONSOLE_WARNING_COLOR; break;
			 case Util::LOG_LEVEL::LOG_ERROR: res = CONSOLE_ERROR_COLOR; break;
			 case Util::LOG_LEVEL::LOG_CRITICAL: res = CONSOLE_CRITICAL_COLOR; break;
			default: res = QColor(); break;
			}
			*/

			return res;
		}
	};

} // namespace VTX::UI::QT::DockWidget

#endif
