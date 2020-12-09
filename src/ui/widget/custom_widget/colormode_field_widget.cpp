#include "colormode_field_widget.hpp"

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

					_layout->addWidget( _colorModeComboBox );

					_colorSetButton = new QPushButton( this );
					_colorSetButton->hide();
				
					_colorDialog = new QColorDialog( this );
				}

				void ColorModeFieldWidget::_setupSlots() { connect( _colorSetButton, &QPushButton::clicked, this, &ColorModeFieldWidget::_openColorDialog ); }

				void ColorModeFieldWidget::_openColorDialog() { _colorDialog->open(); }

				void ColorModeFieldWidget::localize() {};

			} // namespace CustomWidget
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
