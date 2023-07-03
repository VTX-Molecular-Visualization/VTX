#include "ui/old_ui/ui/widget/custom_widget/ribbon_color_mode_field_widget.hpp"
#include "ui/old_ui/ui/main_window.hpp"
#include "ui/old_ui/ui/widget/settings/setting_widget.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include "ui/old_ui/util/ui.hpp"
#include "ui/old_ui/vtx_app.hpp"
#include <core/chemdb/color.hpp>

namespace VTX::UI::Widget::CustomWidget
{
	void RibbonColorModeFieldWidget::_setupUi( const QString & p_name )
	{
		setObjectName( p_name );
		setAcceptDrops( true );
		setFixedSize( 200, 50 );

		_layout = new QHBoxLayout( this );

		_colorModeComboBox = new CustomWidget::QComboBoxMultiField( this );
		Util::UI::filterEventOnWidget( _colorModeComboBox, QEvent::Type::Wheel );

		QStringList colorModeList = QStringList();
		for ( const std::string colorModeStrings : VTX::Core::ChemDB::Color::SECONDARY_STRUCTURE_COLOR_MODE_STRING )
			colorModeList.append( QString::fromStdString( colorModeStrings ) );

		_colorModeComboBox->addItems( colorModeList );

		_colorSetButton = WidgetFactory::get().instantiateWidget<ColorFieldButton>( this, "colorSetButton" );
		_colorSetButton->hide();

		// !V0.1
		//_openColorSettingsButton = new QPushButton( this );
		//_openColorSettingsButton->setText( "Manage\nColors" );
		//_openColorSettingsButton->hide();

		_layout->addWidget( _colorModeComboBox );
		_layout->addWidget( _colorSetButton );
		// !V0.1
		//_layout->addWidget( _openColorSettingsButton );
	}

	void RibbonColorModeFieldWidget::_setupSlots()
	{
		connect( _colorModeComboBox,
				 QOverload<int>::of( &QComboBox::currentIndexChanged ),
				 this,
				 &RibbonColorModeFieldWidget::_colorModeChange );
		connect( _colorSetButton,
				 QOverload<const Util::Color::Rgba &>::of( &ColorFieldButton::onValueChange ),
				 this,
				 &RibbonColorModeFieldWidget::_applyColor );
		// !V0.1
		// connect( _openColorSettingsButton,
		//		 &QPushButton::clicked,
		//		 this,
		//		 &SecondaryStructureColorModeFieldWidget::_openColorSettings );
	}

	void RibbonColorModeFieldWidget::_refresh()
	{
		const VTX::Core::ChemDB::Color::SECONDARY_STRUCTURE_COLOR_MODE currentColorMode
			= VTX::Core::ChemDB::Color::SECONDARY_STRUCTURE_COLOR_MODE( _colorModeComboBox->currentIndex() );

		const bool displayColorSetButton
			= currentColorMode == VTX::Core::ChemDB::Color::SECONDARY_STRUCTURE_COLOR_MODE::PROTEIN
			  || currentColorMode == VTX::Core::ChemDB::Color::SECONDARY_STRUCTURE_COLOR_MODE::CUSTOM;
		const bool displayColorSettingButton
			= currentColorMode == VTX::Core::ChemDB::Color::SECONDARY_STRUCTURE_COLOR_MODE::CHAIN;

		if ( displayColorSetButton )
			_colorSetButton->show();
		else
			_colorSetButton->hide();

		// !V0.1
		// if ( displayColorSettingButton )
		//	_openColorSettingsButton->show();
		// else
		//	_openColorSettingsButton->hide();
	}

	void RibbonColorModeFieldWidget::setColorMode(
		const VTX::Core::ChemDB::Color::SECONDARY_STRUCTURE_COLOR_MODE & p_colorMode )
	{
		_colorMode = p_colorMode;
		_colorModeComboBox->setCurrentIndex( int( _colorMode ) );

		_refresh();

		emit colorModeChanged( _colorMode );
	}

	void RibbonColorModeFieldWidget::setColor( const Util::Color::Rgba & p_color )
	{
		_color = p_color;
		_colorSetButton->setColor( p_color );
	}

	// !V0.1
	// void SecondaryStructureColorModeFieldWidget::_openColorSettings()
	//{
	//	UI::VTXApp::get().getMainWindow().openSettingWindow( Widget::Settings::SETTING_MENU::COLORS );
	//}

	void RibbonColorModeFieldWidget::_colorModeChange( int p_index )
	{
		const VTX::Core::ChemDB::Color::SECONDARY_STRUCTURE_COLOR_MODE colorMode
			= VTX::Core::ChemDB::Color::SECONDARY_STRUCTURE_COLOR_MODE( p_index );
		setColorMode( colorMode );
	}
	void RibbonColorModeFieldWidget::_applyColor( const Util::Color::Rgba & p_color )
	{
		_color = p_color;
		emit colorChanged( _color );
	}

	void RibbonColorModeFieldWidget::localize() {};

	void RibbonColorModeFieldWidget::updateWithNewValue(
		const std::pair<VTX::Core::ChemDB::Color::SECONDARY_STRUCTURE_COLOR_MODE, Util::Color::Rgba> & p_value )
	{
		_colorModeComboBox->updateWithNewValue( int( p_value.first ) );
		_colorSetButton->updateWithNewValue( p_value.second );

		_refresh();
	}
	void RibbonColorModeFieldWidget::resetState()
	{
		_colorModeComboBox->resetState();
		_colorSetButton->resetState();
	}
	void RibbonColorModeFieldWidget::_displayDifferentsDataFeedback() {}

} // namespace VTX::UI::Widget::CustomWidget
