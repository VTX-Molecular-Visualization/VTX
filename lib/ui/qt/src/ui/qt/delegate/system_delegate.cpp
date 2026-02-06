#include "ui/qt/delegate/system_delegate.hpp"
#include "ui/qt/services.hpp"
#include "ui/qt/style/icons.hpp"
#include "ui/qt/style/style_manager.hpp"
#include <QApplication>
#include <QMouseEvent>
#include <QPainter>

namespace VTX::UI::QT::Delegate
{

	SystemDelegate::SystemDelegate( const App::ECS::Entity p_system, QObject * p_parent ) :
		QStyledItemDelegate( p_parent ), _system( p_system )
	{
		_icons[ 0 ] = STYLE().iconFromCodepoint( Style::Icons::VISIBILITY_OFF );
		_icons[ 1 ] = STYLE().iconFromCodepoint( Style::Icons::COLORS );
		_icons[ 2 ] = STYLE().iconFromCodepoint( Style::Icons::TUNE );
	}

	void SystemDelegate::paint(
		QPainter *					 p_painter,
		const QStyleOptionViewItem & p_option,
		const QModelIndex &			 p_index
	) const
	{
		QStyledItemDelegate::paint( p_painter, p_option, p_index );

		if ( not( p_option.state & QStyle::State_MouseOver ) )
		{
			return;
		}

		p_painter->save();

		for ( int i = 0; i < _icons.size(); ++i )
		{
			const QRect r = _buttonRect( p_option, i );
			_icons[ i ].paint( p_painter, r, Qt::AlignCenter, QIcon::Normal );
		}

		p_painter->restore();
	}

	QSize SystemDelegate::sizeHint( const QStyleOptionViewItem & p_option, const QModelIndex & p_index ) const
	{
		return QStyledItemDelegate::sizeHint( p_option, p_index );
	}

	bool SystemDelegate::editorEvent(
		QEvent *					 p_event,
		QAbstractItemModel *		 p_model,
		const QStyleOptionViewItem & p_option,
		const QModelIndex &			 p_index
	)
	{
		if ( p_event->type() == QEvent::MouseButtonPress )
		{
			auto * e = static_cast<QMouseEvent *>( p_event );

			const int hit = _hitTestButton( p_option, e->pos() );
			if ( hit < 0 )
			{
				return QStyledItemDelegate::editorEvent( p_event, p_model, p_option, p_index );
			}

			switch ( static_cast<ACTION>( hit ) )
			{
			case ACTION::VISIBILITY: emit visibilityClicked( p_index, e->globalPosition() ); break;
			case ACTION::COLOR_SCHEME: emit colorSchemeClicked( p_index, e->globalPosition() ); break;
			case ACTION::REPRESENTATION: emit representationClicked( p_index, e->globalPosition() ); break;

			default: break;
			}

			p_event->accept();
			return true;
		}

		return QStyledItemDelegate::editorEvent( p_event, p_model, p_option, p_index );
	}

	QWidget * SystemDelegate::createEditor(
		QWidget *					 p_parent,
		const QStyleOptionViewItem & p_option,
		const QModelIndex &			 p_index
	) const
	{
		return QStyledItemDelegate::createEditor( p_parent, p_option, p_index );
	}

	void SystemDelegate::setEditorData( QWidget * p_editor, const QModelIndex & p_index ) const
	{
		QStyledItemDelegate::setEditorData( p_editor, p_index );
	}

	void SystemDelegate::setModelData(
		QWidget *			 p_editor,
		QAbstractItemModel * p_model,
		const QModelIndex &	 p_index
	) const
	{
		QStyledItemDelegate::setModelData( p_editor, p_model, p_index );
	}

	QRect SystemDelegate::_buttonsRect( const QStyleOptionViewItem & p_option ) const
	{
		const int	count  = static_cast<int>( _icons.size() );
		const int	totalW = count * ICON_SIZE + ( count - 1 ) * SPACING;
		const QRect r	   = p_option.rect;

		return QRect( r.right() - totalW - MARGIN_R, r.center().y() - ICON_SIZE / 2, totalW, ICON_SIZE );
	}

	QRect SystemDelegate::_buttonRect( const QStyleOptionViewItem & p_option, const int p_i ) const
	{
		const QRect br = _buttonsRect( p_option );

		return QRect( br.left() + p_i * ( ICON_SIZE + SPACING ), br.top(), ICON_SIZE, ICON_SIZE );
	}

	int SystemDelegate::_hitTestButton( const QStyleOptionViewItem & p_option, const QPoint & p_pos ) const
	{
		for ( int i = 0; i < _icons.size(); ++i )
		{
			if ( _buttonRect( p_option, i ).contains( p_pos ) )
			{
				return i;
			}
		}

		return -1;
	}

} // namespace VTX::UI::QT::Delegate
