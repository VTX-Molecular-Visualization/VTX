#include "transition_color_mode_field_widget.hpp"
#include "ui/main_window.hpp"
#include "ui/widget/settings/setting_widget.hpp"
#include "ui/widget_factory.hpp"
#include "util/ui.hpp"
#include "vtx_app.hpp"

namespace VTX::UI::Widget::CustomWidget
{
	void TransitionColorModeFieldWidget::_setupUi( const QString & p_name )
	{
		setObjectName( p_name );
		setAcceptDrops( true );
		setFixedSize( 200, 50 );

		_layout = new QHBoxLayout( this );

		_colorModeComboBox = new CustomWidget::QComboBoxMultiField( this );
		Util::UI::filterEventOnWidget( _colorModeComboBox, QEvent::Type::Wheel );

		QStringList colorModeList = QStringList();
		for ( const std::string colorModeStrings : Generic::TRANSITION_COLOR_MODE_STRING )
			colorModeList.append( QString::fromStdString( colorModeStrings ) );

		_colorModeComboBox->addItems( colorModeList );

		// !V0.1
		//_openColorSettingsButton = new QPushButton( this );
		//_openColorSettingsButton->setText( "Manage\nColors" );
		//_openColorSettingsButton->hide();

		_layout->addWidget( _colorModeComboBox );
		// !V0.1
		//_layout->addWidget( _openColorSettingsButton );
	}

	void TransitionColorModeFieldWidget::_setupSlots()
	{
		connect( _colorModeComboBox,
				 QOverload<int>::of( &QComboBox::currentIndexChanged ),
				 this,
				 &TransitionColorModeFieldWidget::_colorModeChange );

		// !V0.1
		// connect( _openColorSettingsButton,
		//		 &QPushButton::clicked,
		//		 this,
		//		 &SecondaryStructureColorModeFieldWidget::_openColorSettings );
	}

	void TransitionColorModeFieldWidget::_refresh() {}

	void TransitionColorModeFieldWidget::setColorMode( const Generic::COLOR_TRANSITION_MODE & p_colorMode )
	{
		_colorMode = p_colorMode;
		_colorModeComboBox->setCurrentIndex( int( _colorMode ) );

		_refresh();

		emit colorModeChanged( _colorMode );
	}

	// !V0.1
	// void SecondaryStructureColorModeFieldWidget::_openColorSettings()
	//{
	//	VTXApp::get().getMainWindow().openSettingWindow( Widget::Settings::SETTING_MENU::COLORS );
	//}

	void TransitionColorModeFieldWidget::_colorModeChange( int p_index )
	{
		const Generic::COLOR_TRANSITION_MODE colorMode = Generic::COLOR_TRANSITION_MODE( p_index );
		setColorMode( colorMode );
	}

	void TransitionColorModeFieldWidget::localize() {};

	void TransitionColorModeFieldWidget::updateWithNewValue( const Generic::COLOR_TRANSITION_MODE & p_value )
	{
		_colorModeComboBox->updateWithNewValue( int( p_value ) );

		_refresh();
	}
	void TransitionColorModeFieldWidget::resetState() { _colorModeComboBox->resetState(); }
	void TransitionColorModeFieldWidget::_displayDifferentsDataFeedback() {}

} // namespace VTX::UI::Widget::CustomWidget
