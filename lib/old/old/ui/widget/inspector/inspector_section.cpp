#include "inspector_section.hpp"
#include <QGridLayout>
#include <QString>

namespace VTX::UI::Widget::Inspector
{
	void InspectorSection::_setupUi( const QString & p_name )
	{
		CustomWidget::CollapsingHeaderWidget::_setupUi( p_name );
		setHeaderHeight( Style::INSPECTOR_HEADER_HEIGHT );
		displayIconInHeader( false );
	};

	void InspectorSectionVLayout::_setupUi( const QString & p_name )
	{
		InspectorSection::_setupUi( p_name );

		QWidget * const content = new QWidget( this );
		content->setContentsMargins( 4, 4, 4, 0 );

		_gridLayout = new QGridLayout( content );
		_gridLayout->setColumnStretch( 1, 100 );
		_gridLayout->setContentsMargins( 0, 0, 0, 0 );

		setBody( content );
	};
	void InspectorSectionVLayout::appendField( QWidget * const p_widget )
	{
		p_widget->setParent( this );

		const int row = _gridLayout->rowCount();
		_gridLayout->addWidget( p_widget, row, 0, 1, 2 );
	};
	void InspectorSectionVLayout::appendField( const std::string & p_label, QWidget * const p_widget )
	{
		QLabel * const label = new QLabel( this );
		label->setText( QString::fromStdString( p_label ) );

		appendField( label, p_widget );
	};
	void InspectorSectionVLayout::appendField( QLabel * const p_labelWidget, QWidget * const p_widget )
	{
		p_widget->setParent( this );

		const int row = _gridLayout->rowCount();
		_gridLayout->addWidget( p_labelWidget, row, 0 );
		_gridLayout->addWidget( p_widget, row, 1 );
	};
} // namespace VTX::UI::Widget::Inspector
