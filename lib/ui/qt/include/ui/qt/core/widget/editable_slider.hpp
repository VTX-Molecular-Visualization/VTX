#ifndef __VTX_UI_QT_CORE_WIDGET_EDITABLE_SLIDER__
#define __VTX_UI_QT_CORE_WIDGET_EDITABLE_SLIDER__

#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QPointer>
#include <QSlider>
#include <util/logger.hpp>
#include <util/math.hpp>

namespace VTX::UI::QT::Core::Widget
{
	/**
	 * @brief QSlider with editable area.
	 */
	class EditableSlider : public QWidget
	{
		Q_OBJECT

	  public:
		explicit EditableSlider( Qt::Orientation p_orientation, QWidget * p_parent ) : QWidget( p_parent )
		{
			_slider	 = new QSlider( p_orientation, this );
			_spinbox = new QDoubleSpinBox( this );

			_slider->setSingleStep( 1 );
			_spinbox->setSingleStep( 0.01 );
			_spinbox->setDecimals( 2 );

			auto * layout = new QHBoxLayout( this );
			layout->setContentsMargins( 0, 0, 0, 0 );
			// layout->setSpacing( 0 );

			layout->addWidget( _slider );
			layout->addWidget( _spinbox );

			connect(
				_slider,
				&QSlider::valueChanged,
				this,
				[ this ]( const int p_value )
				{
					const float value = float( p_value ) / 100.f;
					_spinbox->setValue( value );
				}
			);

			connect(
				_spinbox,
				&QDoubleSpinBox::valueChanged,
				this,
				[ this ]( const double p_value )
				{
					QSignalBlocker blocker( _slider );
					_slider->setValue( std::lround( p_value * 100.0f ) );
					emit valueChanged( p_value );
				}
			);
		}

		inline void setValue( const float p_value ) { _spinbox->setValue( p_value ); }

		inline void setMinimum( const float p_min )
		{
			_slider->setMinimum( std::lround( p_min * 100.0f ) );
			_spinbox->setMinimum( p_min );
		}

		inline void setMaximum( const float p_max )
		{
			_slider->setMaximum( std::lround( p_max * 100.0f ) );
			_spinbox->setMaximum( p_max );
		}

		inline void setSuffix( const QString & p_suffix ) { _spinbox->setSuffix( " " + p_suffix ); }

	  signals:
		void valueChanged( const float value );

	  private:
		QPointer<QSlider>		 _slider;
		QPointer<QDoubleSpinBox> _spinbox;
	};
} // namespace VTX::UI::QT::Core::Widget
#endif
