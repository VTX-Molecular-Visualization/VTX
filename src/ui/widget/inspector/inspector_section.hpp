#ifndef __VTX_UI_WIDGET_INSPECTOR_SECTION__
#define __VTX_UI_WIDGET_INSPECTOR_SECTION__

#ifdef _MSC_VER
#pragma once
#endif

#include "style.hpp"
#include "ui/widget/custom_widget/collapsing_header_widget.hpp"
#include "ui/widget_factory.hpp"
#include <QGridLayout>
#include <QString>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Inspector
			{
				class InspectorSection : public CustomWidget::CollapsingHeaderWidget
				{
					VTX_MANUAL_WIDGET_DECLARATION

				  public:
					InspectorSection( QWidget * p_parent ) : CustomWidget::CollapsingHeaderWidget( p_parent ) {};
					~InspectorSection() {};

					inline void appendField( const std::string & p_label, QWidget * const p_widget )
					{
						p_widget->setParent( this );

						QLabel * const label = new QLabel( this );
						label->setText( QString::fromStdString( p_label ) );

						const int row = _gridLayout->rowCount();
						_gridLayout->addWidget( label, row, 0 );
						_gridLayout->addWidget( p_widget, row, 1 );
					};
					inline void localize() {

					};

				  protected:
					void _setupUi( const QString & p_name ) override
					{
						CustomWidget::CollapsingHeaderWidget::_setupUi( p_name );

						setHeaderHeight( Style::INSPECTOR_HEADER_HEIGHT );
						displayIconInHeader( false );

						QWidget * const content = new QWidget( this );
						content->setContentsMargins( 4, 4, 4, 0 );

						_gridLayout = new QGridLayout( content );
						_gridLayout->setColumnStretch( 1, 100 );
						_gridLayout->setContentsMargins( 0, 0, 0, 0 );

						setBody( content );
					}

				  private:
					QGridLayout * _gridLayout;
				};

			} // namespace Inspector
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX

#endif
