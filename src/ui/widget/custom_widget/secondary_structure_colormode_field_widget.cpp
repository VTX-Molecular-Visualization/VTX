#include "secondary_structure_colormode_field_widget.hpp"
#include "ui/main_window.hpp"
#include "ui/widget/settings/setting_widget.hpp"
#include "ui/widget_factory.hpp"
#include "vtx_app.hpp"

namespace VTX::UI::Widget::CustomWidget
{
	void SecondaryStructureColorModeFieldWidget::_setupUi( const QString & p_name )
	{
		setObjectName( p_name );
		setAcceptDrops( true );
		setFixedSize( 200, 50 );

		_layout = new QHBoxLayout( this );

		_colorModeComboBox = new CustomWidget::QComboBoxMultiField( this );

		QStringList colorModeList = QStringList();
		for ( const std::string colorModeStrings : Generic::SECONDARY_STRUCTURE_COLOR_MODE_STRING )
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

	void SecondaryStructureColorModeFieldWidget::_setupSlots()
	{
		connect( _colorModeComboBox,
				 QOverload<int>::of( &QComboBox::currentIndexChanged ),
				 this,
				 &SecondaryStructureColorModeFieldWidget::_colorModeChange );
		connect( _colorSetButton,
				 QOverload<const Color::Rgb &>::of( &ColorFieldButton::onValueChange ),
				 this,
				 &SecondaryStructureColorModeFieldWidget::_applyColor );
		connect( _openColorSettingsButton,
				 &QPushButton::clicked,
				 this,
				 &SecondaryStructureColorModeFieldWidget::_openColorSettings );
	}

	void SecondaryStructureColorModeFieldWidget::_refresh()
	{
		const Generic::SECONDARY_STRUCTURE_COLOR_MODE currentColorMode
			= Generic::SECONDARY_STRUCTURE_COLOR_MODE( _colorModeComboBox->currentIndex() );

		const bool displayColorSetButton = currentColorMode == Generic::SECONDARY_STRUCTURE_COLOR_MODE::PROTEIN
										   || currentColorMode == Generic::SECONDARY_STRUCTURE_COLOR_MODE::CUSTOM;
		const bool displayColorSettingButton = currentColorMode == Generic::SECONDARY_STRUCTURE_COLOR_MODE::CHAIN;

		if ( displayColorSetButton )
			_colorSetButton->show();
		else
			_colorSetButton->hide();

		if ( displayColorSettingButton )
			_openColorSettingsButton->show();
		else
			_openColorSettingsButton->hide();
	}

	void SecondaryStructureColorModeFieldWidget::setColorMode(
		const Generic::SECONDARY_STRUCTURE_COLOR_MODE & p_colorMode )
	{
		_colorMode = p_colorMode;
		_colorModeComboBox->setCurrentIndex( int( _colorMode ) );

		_refresh();

		emit colorModeChanged( _colorMode );
	}

	void SecondaryStructureColorModeFieldWidget::setColor( const Color::Rgb & p_color )
	{
		_color = p_color;
		_colorSetButton->setColor( p_color );
	}

	void SecondaryStructureColorModeFieldWidget::_openColorSettings()
	{
		VTXApp::get().getMainWindow().openSettingWindow( Widget::Settings::SETTING_MENU::COLORS );
	}

	void SecondaryStructureColorModeFieldWidget::_colorModeChange( int p_index )
	{
		const Generic::SECONDARY_STRUCTURE_COLOR_MODE colorMode = Generic::SECONDARY_STRUCTURE_COLOR_MODE( p_index );
		setColorMode( colorMode );
	}
	void SecondaryStructureColorModeFieldWidget::_applyColor( const Color::Rgb & p_color )
	{
		_color = p_color;
		emit colorChanged( _color );
	}

	void SecondaryStructureColorModeFieldWidget::localize() {};

	void SecondaryStructureColorModeFieldWidget::updateWithNewValue(
		const std::pair<Generic::SECONDARY_STRUCTURE_COLOR_MODE, Color::Rgb> & p_value )
	{
		_colorModeComboBox->updateWithNewValue( int( p_value.first ) );
		_colorSetButton->updateWithNewValue( p_value.second );

		_refresh();
	}
	void SecondaryStructureColorModeFieldWidget::resetState()
	{
		_colorModeComboBox->resetState();
		_colorSetButton->resetState();
	}
	void SecondaryStructureColorModeFieldWidget::_displayDifferentsDataFeedback() {}

} // namespace VTX::UI::Widget::CustomWidget
