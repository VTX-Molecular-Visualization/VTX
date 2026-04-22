#ifndef __VTX_UI_QT_WIDGET_EXPANDABLE_TOOL_BAR__
#define __VTX_UI_QT_WIDGET_EXPANDABLE_TOOL_BAR__

#include "ui/qt/action_registry.hpp"
#include "ui/qt/services.hpp"
#include <QHBoxLayout>
#include <QPointer>
#include <QToolButton>
#include <QWidget>
#include <string_view>
#include <util/logger.hpp>

namespace VTX::UI::QT::Widget
{
	/**
	 * @brief Custom toolbar that expand to parent width.
	 */
	class ExpandableToolBar : public QWidget
	{
	  public:
		explicit ExpandableToolBar( QWidget * p_parent ) : QWidget( p_parent )
		{
			_layout = new QHBoxLayout( this );
			_layout->setContentsMargins( 0, 0, 0, 0 );
		}

		QAction * addAction( const std::string_view p_actionId )
		{
			auto * action = UI_ACTIONS().getAction( p_actionId );
			if ( action == nullptr )
			{
				VTX_ERROR( "Unable to add unregistered UI action to expandable toolbar: {}", p_actionId );
				return nullptr;
			}

			_addButton( *action );
			return action;
		}

		void setToolButtonStyle( Qt::ToolButtonStyle p_style )
		{
			_style = p_style;
			for ( int i = 0; i < _layout->count(); ++i )
			{
				if ( auto * btn = qobject_cast<QToolButton *>( _layout->itemAt( i )->widget() ) )
				{
					btn->setToolButtonStyle( _style );
				}
			}
		}

		void setIconSize( const QSize & p_size )
		{
			_iconSize = p_size;
			for ( int i = 0; i < _layout->count(); ++i )
			{
				if ( auto * btn = qobject_cast<QToolButton *>( _layout->itemAt( i )->widget() ) )
				{
					btn->setIconSize( _iconSize );
				}
			}
		}

	  private:
		QPointer<QHBoxLayout> _layout;
		Qt::ToolButtonStyle	  _style	= Qt::ToolButtonTextUnderIcon;
		QSize				  _iconSize = QSize( 18, 18 );

		void _addButton( QAction & p_action )
		{
			auto * btn = new QToolButton( this );
			btn->setDefaultAction( &p_action );
			btn->setToolButtonStyle( _style );
			btn->setIconSize( _iconSize );
			btn->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
			btn->setAutoRaise( true );
			_layout->addWidget( btn );
		}
	};
} // namespace VTX::UI::QT::Widget
#endif
