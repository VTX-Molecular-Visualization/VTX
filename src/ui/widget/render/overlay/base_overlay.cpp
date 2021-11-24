#include "base_overlay.hpp"
#include <QToolButton>

namespace VTX::UI::Widget::Render::Overlay
{
	BaseOverlay::BaseOverlay( QWidget * p_parent ) : BaseManualWidget( p_parent ) {};

	void BaseOverlay::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		setContentsMargins( 0, 0, 0, 0 );
		// setContentsMargins( 8, 1, 8, 1 );
		setSizePolicy( QSizePolicy::Policy::MinimumExpanding, QSizePolicy::Policy::MinimumExpanding );

		setFloatable( true );
		setMovable( false );
	}

	void BaseOverlay::_setupSlots() {}

	void BaseOverlay::localize() {};

	bool BaseOverlay::eventFilter( QObject * p_obj, QEvent * p_event )
	{
		BaseManualWidget::eventFilter( p_obj, p_event );

		if ( p_event->type() == QEvent::Show )
		{
			QMenu * const menu = dynamic_cast<QMenu *>( p_obj );

			if ( menu != nullptr )
			{
				QAction * const		  action	   = _mapMenuActions[ menu ];
				const QWidget * const actionWidget = widgetForAction( action );

				QPoint localPos;

				switch ( _anchorPosition )
				{
				case OVERLAY_ANCHOR::BOTTOM_CENTER:
					localPos = actionWidget->rect().topLeft() - QPoint( 0, menu->height() );
					break;
				case OVERLAY_ANCHOR::NONE:
				default: localPos = QPoint( 0, 0 ); break;
				}

				const QPoint pos = actionWidget->mapToGlobal( localPos );
				menu->move( pos );

				return true;
			}
		}
		return false;
	}

	QAction * BaseOverlay::addMenu( QMenu * const p_menu )
	{
		QToolButton * openMenuButton = new QToolButton( this );
		openMenuButton->setPopupMode( QToolButton::InstantPopup );

		openMenuButton->setMenu( p_menu );
		QAction * const openMenuAction = addWidget( openMenuButton );

		p_menu->installEventFilter( this );

		_mapMenuActions[ p_menu ] = openMenuAction;

		return openMenuAction;
	}

	void BaseOverlay::setAnchorPosition( const OVERLAY_ANCHOR & p_anchor )
	{
		_anchorPosition = p_anchor;

		switch ( _anchorPosition )
		{
		case OVERLAY_ANCHOR::BOTTOM_CENTER: setOrientation( Qt::Orientation::Horizontal ); break;
		case OVERLAY_ANCHOR::NONE:
		default: setOrientation( Qt::Orientation::Horizontal ); break;
		}
	}

	void BaseOverlay::updatePosition( const QSize & p_renderSize )
	{
		QPoint position = _getPosition( p_renderSize );
		move( position );
	}

	QPoint BaseOverlay::_getPosition( const QSize & p_rect )
	{
		QPoint	  res;
		const int OVERLAY_TOP_OFFEST	= 5;
		const int OVERLAY_BOTTOM_OFFEST = 5;
		const int OVERLAY_LEFT_OFFEST	= 5;
		const int OVERLAY_RIGHT_OFFEST	= 5;

		switch ( _anchorPosition )
		{
		case OVERLAY_ANCHOR::BOTTOM_CENTER:
			res.setX( ( p_rect.width() - width() ) / 2 );
			res.setY( p_rect.height() - height() - OVERLAY_BOTTOM_OFFEST );
			break;
		case OVERLAY_ANCHOR::NONE:
		default: res = QPoint( 0, 0 ); break;
		}

		return res;
	}

	void BaseOverlay::_refreshSize()
	{
		QSize sizeHint = BaseManualWidget::sizeHint();

		// I don't understand why, but we need to add 10 to the size of the toolbar to not clamp it
		// ( maybe linked to the border-radius of 10 in the stylesheet ? )
		if ( orientation() == Qt::Orientation::Horizontal )
		{
			sizeHint.rwidth() += 10;
		}
		else if ( orientation() == Qt::Orientation::Vertical )
		{
			sizeHint.rheight() += 10;
		}

		setFixedSize( sizeHint );
	}

} // namespace VTX::UI::Widget::Render::Overlay
