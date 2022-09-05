#include "attribute_list_layout.hpp"
#include "style.hpp"
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
		addWidget( p_label, _currentRow, 0 );
		addWidget( p_item, _currentRow, 1 );

		_currentRow++;
	}
	void AttributeListLayout::addAttribute( QWidget * const p_item, const Qt::Alignment & p_alignment )
	{
		addWidget( p_item, _currentRow, 0, 1, 2, p_alignment );
		_currentRow++;
	}

	void AttributeListLayout::startAttributeSection( const QString & p_title )
	{
		QLabel * const titleWidget = new QLabel( widget() );
		Util::UI::setDynamicProperty( titleWidget, Style::WidgetProperty::OVERIDDEN_PARAMETER );

		titleWidget->setText( p_title );
		addWidget( titleWidget, _currentRow, 0, 1, 2, Qt::AlignmentFlag::AlignLeft );

		_currentRow++;
	}
	void AttributeListLayout::finishAttributeSection( const bool p_addSpacer )
	{
		if ( p_addSpacer )
		{
			QSpacerItem * const spacer = new QSpacerItem( 0, Style::ATTRIBUTE_LIST_LAYOUT_SPACER_HEIGHT );
			addItem( spacer, _currentRow, 0, 1, 2 );
		}
		_currentRow++;
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
