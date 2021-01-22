#ifndef __VTX_UI_WIDGET_INSPECTOR__ITEM_FIELD__
#define __VTX_UI_WIDGET_INSPECTOR__ITEM_FIELD__

#ifdef _MSC_VER
#pragma once
#endif

#include <QGridLayout>
#include <QLabel>
#include <string>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Inspector
			{
				class InspectorItemField
				{
				  public:
					inline InspectorItemField( const std::string & p_label )
					{
						_label = QString::fromStdString( p_label );

						_labelWidget = new QLabel();
						_labelWidget->setWordWrap( true );
						_valueWidget = new QLabel();
						_valueWidget->setWordWrap( true );
					};

					~InspectorItemField()
					{
						if ( _labelWidget != nullptr )
							delete _labelWidget;

						if ( _valueWidget != nullptr )
							delete _valueWidget;
					};

					inline void addInGrid( const int p_row, QGridLayout & p_grid )
					{
						p_grid.addWidget( _labelWidget, p_row, 0 );
						p_grid.addWidget( _valueWidget, p_row, 1 );
					};
					inline void localize() { _labelWidget->setText( _label ); };
					inline void setValue( const std::string & p_value ) { _valueWidget->setText( QString::fromStdString( p_value ) ); };

				  private:
					QLabel * _labelWidget;
					QLabel * _valueWidget;

					QString _label;
				};

			} // namespace Inspector
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX

#endif
