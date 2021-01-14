#include "vector3_widget.hpp"
#include <QLabel>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace CustomWidget
			{
				void Vector3Widget::_setupUi( const QString & p_name )
				{
					BaseManualWidget::_setupUi( p_name );

					QHBoxLayout * const mainLayout = new QHBoxLayout( this );
					mainLayout->setSpacing( 2 );

					QLabel * const xLabel = new QLabel();
					xLabel->setText( "x" );
					_widgetX = new QDoubleSpinBox();

					QLabel * const yLabel = new QLabel();
					yLabel->setText( "y" );
					_widgetY = new QDoubleSpinBox();

					QLabel * const zLabel = new QLabel();
					zLabel->setText( "z" );
					_widgetZ = new QDoubleSpinBox();

					mainLayout->addWidget( xLabel );
					mainLayout->addWidget( _widgetX );
					mainLayout->addWidget( yLabel );
					mainLayout->addWidget( _widgetY );
					mainLayout->addWidget( zLabel );
					mainLayout->addWidget( _widgetZ );
				}

				void Vector3Widget::_setupSlots()
				{
					connect( _widgetX, QOverload<const double>::of( &QDoubleSpinBox::valueChanged ), this, &Vector3Widget::_onInternalValueXChanged );
					connect( _widgetY, QOverload<const double>::of( &QDoubleSpinBox::valueChanged ), this, &Vector3Widget::_onInternalValueYChanged );
					connect( _widgetZ, QOverload<const double>::of( &QDoubleSpinBox::valueChanged ), this, &Vector3Widget::_onInternalValueZChanged );
				}

				void Vector3Widget::_onInternalValueXChanged( const double p_newValue )
				{
					_data.x = p_newValue;
					emit onValueChange( _data );
				}
				void Vector3Widget::_onInternalValueYChanged( const double p_newValue )
				{
					_data.y = p_newValue;
					emit onValueChange( _data );
				}
				void Vector3Widget::_onInternalValueZChanged( const double p_newValue )
				{
					_data.z = p_newValue;
					emit onValueChange( _data );
				}

				void Vector3Widget::_refresh()
				{
					_widgetX->setValue( _data.x );
					_widgetY->setValue( _data.y );
					_widgetZ->setValue( _data.z );
				}

				void Vector3Widget::localize() {};
			} // namespace CustomWidget
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
