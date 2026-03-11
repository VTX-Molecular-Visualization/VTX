#ifndef __VTX_UI_QT_WIDGET_EXPANDABLE_TOOL_BAR__
#define __VTX_UI_QT_WIDGET_EXPANDABLE_TOOL_BAR__

#include <QHBoxLayout>
#include <QWidget>

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

		template<App::UI::ConceptAction A>
		QAction * addAction()
		{
			auto * btn	  = new QToolButton( this );
			auto * action = Application::getAction<A>();
			btn->setDefaultAction( action );
			btn->setToolButtonStyle( _style );
			btn->setIconSize( _iconSize );
			btn->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred );
			btn->setAutoRaise( true );
			_layout->addWidget( btn );

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
	};
} // namespace VTX::UI::QT::Widget
#endif
