#include "attribute_list_layout.hpp"
#include "util/ui.hpp"

namespace VTX::UI::Layout
{
	AttributeListLayout::AttributeListLayout( QWidget * const p_parent ) : QGridLayout( p_parent )
	{
		setSizeConstraint( QLayout::SizeConstraint::SetMinAndMaxSize );
		setContentsMargins( 10, 10, 10, 10 );

		setVerticalSpacing( Style::DATA_GRID_VERTICAL_SPACE );
		setHorizontalSpacing( Style::DATA_GRID_HORIZONTAL_SPACE );
		setColumnStretch( 0, 1 );
		setColumnStretch( 1, 10 );
	}

	void AttributeListLayout::addAttribute( QWidget * const p_item, const QString & p_label )
	{
		QLabel * const labelWidget = new QLabel( widget() );
		labelWidget->setAlignment( Qt::AlignTop | Qt::AlignLeft );
		labelWidget->setText( p_label );

		addAttribute( p_item, labelWidget );
	}
	void AttributeListLayout::addAttribute( QWidget * const p_item, QLabel * const p_label )
	{
		QMargins margin = p_label->contentsMargins();
		margin.setLeft( margin.left() + ( _currentIndent * _indentSize ) );
		p_label->setContentsMargins( margin );

		addWidget( p_label, _currentRow, 0 );
		addWidget( p_item, _currentRow, 1 );

		_currentRow++;
	}
	void AttributeListLayout::addAttribute( QWidget * const p_item, const Qt::Alignment & p_alignment )
	{
		QMargins margin = p_item->contentsMargins();
		margin.setLeft( margin.left() + ( _currentIndent * _indentSize ) );
		p_item->setContentsMargins( margin );

		addWidget( p_item, _currentRow, 0, 1, 2, p_alignment );
		_currentRow++;
	}

	void AttributeListLayout::addSpace( const int p_space )
	{
		addItem( new QSpacerItem( 0, p_space ), _currentRow, 0, 2, 1 );
		_currentRow++;
	}

	void AttributeListLayout::startAttributeSection( const QString & p_title )
	{
		QLabel * const titleWidget = new QLabel( widget() );
		titleWidget->setText( p_title );
		startAttributeSection( titleWidget );
	}
	void AttributeListLayout::startAttributeSection( QWidget * const p_titleWidget )
	{
		Util::UI::setDynamicProperty( p_titleWidget, Style::WidgetProperty::ATTRIBUTE_LAYOUT_SECTION_TITLE );
		addWidget( p_titleWidget, _currentRow, 0, 1, 2, Qt::AlignmentFlag::AlignLeft );

		_currentRow++;
		_currentIndent++;
	}
	void AttributeListLayout::startAttributeSection( const QString & p_title, QWidget * const p_widget )
	{
		QLabel * const titleWidget = new QLabel( widget() );
		titleWidget->setText( p_title );
		Util::UI::setDynamicProperty( titleWidget, Style::WidgetProperty::ATTRIBUTE_LAYOUT_SECTION_TITLE );

		addWidget( titleWidget, _currentRow, 0, 1, 1, Qt::AlignmentFlag::AlignLeft );
		addWidget( p_widget, _currentRow, 1, 1, 1, Qt::AlignmentFlag::AlignLeft );

		_currentRow++;
		_currentIndent++;
	}

	void AttributeListLayout::finishAttributeSection( const bool p_addSpacer )
	{
		if ( p_addSpacer )
		{
			QSpacerItem * const spacer = new QSpacerItem( 0, _endSectionSpacerSize );
			addItem( spacer, _currentRow, 0, 1, 2 );

			_currentRow++;
		}

		_currentIndent--;

		assert( _currentIndent >= 0 );
	}

	void AttributeListLayout::setAttributeVisibility( QWidget * const p_item, const bool p_visible ) const
	{
		const int itemRow = _findWidgetRow( p_item );
		if ( itemRow != -1 )
		{
			itemAtPosition( itemRow, 0 )->widget()->setVisible( p_visible );
			itemAtPosition( itemRow, 1 )->widget()->setVisible( p_visible );
		}
	}
	void AttributeListLayout::enableAttribute( QWidget * const p_item, const bool p_enable ) const
	{
		const int itemRow = _findWidgetRow( p_item );
		if ( itemRow != -1 )
		{
			itemAtPosition( itemRow, 0 )->widget()->setEnabled( p_enable );
			itemAtPosition( itemRow, 1 )->widget()->setEnabled( p_enable );
		}
	}

	int AttributeListLayout::_findWidgetRow( QWidget * const p_item ) const
	{
		for ( int i = 0; i < _currentRow; i++ )
		{
			if ( itemAtPosition( i, 1 )->widget() == p_item )
			{
				return i;
			}
		}

		return -1;
	}

} // namespace VTX::UI::Layout
