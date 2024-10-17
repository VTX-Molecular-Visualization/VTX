#include "ui/qt/layout/flow_layout.hpp"

namespace VTX::UI::QT::Layout
{

	FlowLayout::FlowLayout( QWidget * p_parent, int p_margin, int p_hSpacing, int p_vSpacing ) :
		QLayout( p_parent ), _hSpace( p_hSpacing ), _vSpace( p_vSpacing )
	{
		setContentsMargins( p_margin, p_margin, p_margin, p_margin );
	}

	FlowLayout::FlowLayout( int p_margin, int p_hSpacing, int p_vSpacing ) :
		_hSpace( p_hSpacing ), _vSpace( p_vSpacing )
	{
		setContentsMargins( p_margin, p_margin, p_margin, p_margin );
	}

	FlowLayout::~FlowLayout() { qDeleteAll( itemList ); }

	void FlowLayout::addItem( QLayoutItem * p_item ) { itemList.append( p_item ); }

	int FlowLayout::horizontalSpacing() const
	{
		if ( _hSpace >= 0 )
		{
			return _hSpace;
		}
		else
		{
			return smartSpacing( QStyle::PM_LayoutHorizontalSpacing );
		}
	}

	int FlowLayout::verticalSpacing() const
	{
		if ( _vSpace >= 0 )
		{
			return _vSpace;
		}
		else
		{
			return smartSpacing( QStyle::PM_LayoutVerticalSpacing );
		}
	}

	int FlowLayout::count() const { return itemList.size(); }

	QLayoutItem * FlowLayout::itemAt( int p_index ) const
	{
		if ( p_index >= 0 && p_index < itemList.size() )
		{
			return itemList.at( p_index );
		}
		else
		{
			return nullptr;
		}
	}

	QLayoutItem * FlowLayout::takeAt( int p_index )
	{
		if ( p_index >= 0 && p_index < itemList.size() )
		{
			return itemList.takeAt( p_index );
		}
		else
		{
			return nullptr;
		}
	}

	Qt::Orientations FlowLayout::expandingDirections() const { return {}; }

	bool FlowLayout::hasHeightForWidth() const { return true; }

	int FlowLayout::heightForWidth( int p_width ) const { return doLayout( QRect( 0, 0, p_width, 0 ), true ); }

	void FlowLayout::setGeometry( const QRect & p_rect )
	{
		QLayout::setGeometry( p_rect );
		doLayout( p_rect, false );
	}

	QSize FlowLayout::sizeHint() const { return minimumSize(); }

	QSize FlowLayout::minimumSize() const
	{
		QSize size;
		for ( const QLayoutItem * p_item : std::as_const( itemList ) )
		{
			size = size.expandedTo( p_item->minimumSize() );
		}

		const QMargins margins = contentsMargins();
		size += QSize( margins.left() + margins.right(), margins.top() + margins.bottom() );
		return size;
	}

	int FlowLayout::doLayout( const QRect & p_rect, bool p_testOnly ) const
	{
		int left, top, right, bottom;
		getContentsMargins( &left, &top, &right, &bottom );
		QRect effectiveRect = p_rect.adjusted( +left, +top, -right, -bottom );
		int	  x				= effectiveRect.x();
		int	  y				= effectiveRect.y();
		int	  lineHeight	= 0;
		for ( QLayoutItem * p_item : std::as_const( itemList ) )
		{
			const QWidget * p_wid  = p_item->widget();
			int				spaceX = horizontalSpacing();
			if ( spaceX == -1 )
			{
				spaceX
					= p_wid->style()->layoutSpacing( QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Horizontal );
			}
			int spaceY = verticalSpacing();
			if ( spaceY == -1 )
			{
				spaceY
					= p_wid->style()->layoutSpacing( QSizePolicy::PushButton, QSizePolicy::PushButton, Qt::Vertical );
			}
			int nextX = x + p_item->sizeHint().width() + spaceX;
			if ( nextX - spaceX > effectiveRect.right() && lineHeight > 0 )
			{
				x		   = effectiveRect.x();
				y		   = y + lineHeight + spaceY;
				nextX	   = x + p_item->sizeHint().width() + spaceX;
				lineHeight = 0;
			}

			if ( !p_testOnly )
			{
				p_item->setGeometry( QRect( QPoint( x, y ), p_item->sizeHint() ) );
			}

			x		   = nextX;
			lineHeight = qMax( lineHeight, p_item->sizeHint().height() );
		}
		return y + lineHeight - p_rect.y() + bottom;
	}

	int FlowLayout::smartSpacing( QStyle::PixelMetric p_pm ) const
	{
		QObject * p_parent = this->parent();
		if ( !p_parent )
		{
			return -1;
		}
		else if ( p_parent->isWidgetType() )
		{
			QWidget * p_pw = static_cast<QWidget *>( p_parent );
			return p_pw->style()->pixelMetric( p_pm, nullptr, p_pw );
		}
		else
		{
			return static_cast<QLayout *>( p_parent )->spacing();
		}
	}
} // namespace VTX::UI::QT::Layout
