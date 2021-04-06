#include "colormode_field_widget.hpp"
#include "ui/main_window.hpp"
#include "ui/widget/settings/setting_widget.hpp"
#include "ui/widget_factory.hpp"
#include "vtx_app.hpp"

namespace VTX::UI::Widget::CustomWidget
{
	void ColorModeFieldWidget::_setupUi( const QString & p_name )
	{
		setObjectName( p_name );
		setAcceptDrops( true );
		setFixedSize( 200, 50 );

		_layout = new QHBoxLayout( this );

		_colorModeComboBox = new QComboBoxMultiField( this );

		QStringList colorModeList = QStringList();
		for ( const std::string colorModeStrings : Generic::COLOR_MODE_STRING )
			colorModeList.append( QString::fromStdString( colorModeStrings ) );

		_colorModeComboBox->addItems( colorModeList );

		_colorSetButton = WidgetFactory::get().instantiateWidget<ColorFieldButton>( this, "colorSetButton" );
		_colorSetButton->hide();

		_openColorSettingsButton = new QPushButton( this );
		_openColorSettingsButton->setText( "Manage\nColors" );
		_openColorSettingsButton->hide();

		_layout->addWidget( _colorModeComboBox );
		_layout->addWidget( _colorSetButton );
		_layout->addWidget( _openColorSettingsButton );
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
		connect( _openColorSettingsButton, &QPushButton::clicked, this, &ColorModeFieldWidget::_openColorSettings );
	}

	void ColorModeFieldWidget::_refresh()
	{
		const Generic::COLOR_MODE currentColorMode = Generic::COLOR_MODE( _colorModeComboBox->currentIndex() );

		const bool displayColorSetButton = currentColorMode == Generic::COLOR_MODE::PROTEIN
										   || currentColorMode == Generic::COLOR_MODE::ATOM_PROTEIN
										   || currentColorMode == Generic::COLOR_MODE::CUSTOM
										   || currentColorMode == Generic::COLOR_MODE::ATOM_CUSTOM;

		const bool displayColorSettingButton
			= currentColorMode == Generic::COLOR_MODE::CHAIN || currentColorMode == Generic::COLOR_MODE::ATOM_CHAIN;

		if ( displayColorSetButton )
			_colorSetButton->show();
		else
			_colorSetButton->hide();

		if ( displayColorSettingButton )
			_openColorSettingsButton->show();
		else
			_openColorSettingsButton->hide();
	}

	void ColorModeFieldWidget::setColorMode( const Generic::COLOR_MODE p_colorMode )
	{
		_colorMode = p_colorMode;
		_colorModeComboBox->setCurrentIndex( (int)_colorMode );

		_refresh();

		emit colorModeChanged( _colorMode );
	}

	void ColorModeFieldWidget::setColor( const Color::Rgb & p_color )
	{
		_color = p_color;
		_colorSetButton->setColor( p_color );
	}

	void ColorModeFieldWidget::_openColorSettings()
	{
		VTXApp::get().getMainWindow().openSettingWindow( Widget::Settings::SETTING_MENU::COLORS );
	}

	void ColorModeFieldWidget::_colorModeChange( int p_index )
	{
		const Generic::COLOR_MODE colorMode = (Generic::COLOR_MODE)p_index;
		setColorMode( colorMode );
	}
	void ColorModeFieldWidget::_applyColor( const Color::Rgb & p_color )
	{
		_color = p_color;
		emit colorChanged( _color );
	}

	void ColorModeFieldWidget::localize() {};

	bool ColorModeFieldWidget::_hasToDisplayColorButton( const Generic::COLOR_MODE & p_colorMode ) const
	{
		return p_colorMode == Generic::COLOR_MODE::PROTEIN || p_colorMode == Generic::COLOR_MODE::ATOM_PROTEIN
			   || p_colorMode == Generic::COLOR_MODE::CUSTOM || p_colorMode == Generic::COLOR_MODE::ATOM_CUSTOM;
	}
	bool ColorModeFieldWidget::_hasToDisplaySettingButton( const Generic::COLOR_MODE & p_colorMode ) const
	{
		return p_colorMode == Generic::COLOR_MODE::CHAIN || p_colorMode == Generic::COLOR_MODE::ATOM_CHAIN;
	}

	void ColorModeFieldWidget::updateWithNewValue( const std::pair<Generic::COLOR_MODE, Color::Rgb> & p_value )
	{
		_colorModeComboBox->updateWithNewValue( int( p_value.first ) );
		_colorSetButton->updateWithNewValue( p_value.second );

		_refresh();

		if ( !_colorModeComboBox->hasIdenticalData() )
		{
			_colorSetButton->hide();
			_openColorSettingsButton->hide();
		}
	}
	void ColorModeFieldWidget::resetState()
	{
		_colorModeComboBox->resetState();
		_colorSetButton->resetState();
	}
	void ColorModeFieldWidget::_displayDifferentsDataFeedback() {}

} // namespace VTX::UI::Widget::CustomWidget
