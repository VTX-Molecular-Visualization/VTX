#include "colormode_field_widget.hpp"
#include "ui/widget_factory.hpp"

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace CustomWidget
			{
				void ColorModeFieldWidget::_setupUi( const QString & p_name )
				{
					setObjectName( p_name );
					setAcceptDrops( true );
					setFixedSize( 200, 50 );

					_layout = new QHBoxLayout( this );

					_colorModeComboBox = new QComboBox( this );

					QStringList colorModeList = QStringList();
					for ( const std::string colorModeStrings : Generic::COLOR_MODE_STRING )
						colorModeList.append( QString::fromStdString( colorModeStrings ) );

					_colorModeComboBox->addItems( colorModeList );

					_colorSetButton
						= WidgetFactory::get().instantiateWidget<ColorFieldButton>( this, "colorSetButton" );
					_colorSetButton->hide();

					_layout->addWidget( _colorModeComboBox );
					_layout->addWidget( _colorSetButton );
				}

				void ColorModeFieldWidget::_setupSlots()
				{
					connect( _colorModeComboBox,
							 QOverload<int>::of( &QComboBox::currentIndexChanged ),
							 this,
							 &ColorModeFieldWidget::_colorModeChange );
					connect( _colorSetButton,
							 QOverload<const Color::Rgb &>::of( &ColorFieldButton::onValueChange ),
							 this,
							 &ColorModeFieldWidget::_applyColor );
				}

				void ColorModeFieldWidget::_colorModeChange( int p_index )
				{
					const Generic::COLOR_MODE colorMode = (Generic::COLOR_MODE)p_index;
					setColorMode( colorMode );
				}
				void ColorModeFieldWidget::_applyColor( const Color::Rgb & p_color ) {}

				void ColorModeFieldWidget::localize() {};

			} // namespace CustomWidget
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
