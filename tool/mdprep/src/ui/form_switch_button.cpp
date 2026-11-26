#include <ui/qt/helper.hpp>
//
#include <QFormLayout>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QTabWidget>
//
#include "tool/mdprep/ui/form_switch_button.hpp"

namespace VTX::Tool::Mdprep::ui
{

	namespace
	{
		inline void setTextBasic( QPushButton * p_ ) { p_->setText( "Switch to Simple view" ); }

		inline void setTextAdvanced( QPushButton * p_ ) { p_->setText( "Switch to Advanced view" ); }
	} // namespace

	void FormSwitchButton::setupUi( QLayout * p_layout, const E_FORM_MODE & p_mode )
	{
		QWidget * wButtonContainer = VTX::UI::QT::Helper::makeQObject<QWidget>( p_layout->parentWidget() );
		_buttonToBasic			   = VTX::UI::QT::Helper::makeQObject<QPushButton>( "Basic", wButtonContainer );
		_buttonToAdvanced		   = VTX::UI::QT::Helper::makeQObject<QPushButton>( "Advanced", wButtonContainer );
		QLabel * qlabel			   = VTX::UI::QT::Helper::makeQObject<QLabel>( "Mode : ", wButtonContainer );

		_buttonToBasic->setCheckable( true );
		_buttonToAdvanced->setCheckable( true );

		p_layout->addWidget( wButtonContainer );
		QHBoxLayout * qLayoutButton = VTX::UI::QT::Helper::makeQObject<QHBoxLayout>( wButtonContainer );
		qLayoutButton->setContentsMargins( { 0, 0, 0, 0 } );
		qLayoutButton->addStretch( 1 );
		qLayoutButton->addWidget( qlabel );
		qLayoutButton->addWidget( _buttonToBasic );
		qLayoutButton->addWidget( _buttonToAdvanced );
		qLayoutButton->addStretch( 1 );
		QObject::connect( _buttonToBasic, &QPushButton::clicked, [ & ]() { this->_changeModeBasic(); } );
		QObject::connect( _buttonToAdvanced, &QPushButton::clicked, [ & ]() { this->_changeModeAdvanced(); } );

		switch ( p_mode )
		{
		case E_FORM_MODE::basic: _changeModeBasic(); break;
		case E_FORM_MODE::advanced: _changeModeAdvanced(); break;
		case E_FORM_MODE::COUNT: break;
		default: break;
		}
	}

	void FormSwitchButton::subscribeBasicSwitch( Callback p_ ) { _switchToBasic = std::move( p_ ); }

	void FormSwitchButton::subscribeAdvancedSwitch( Callback p_ ) { _switchToAdvanced = std::move( p_ ); }

	void FormSwitchButton::switchFormMode()
	{
		if ( _mode == E_FORM_MODE::basic )
		{
			_changeModeAdvanced();
		}
		else
		{
			_changeModeBasic();
		}
	}

	void FormSwitchButton::_changeModeBasic()
	{
		// We want the click on an already pushed button to be as consequenceless as possible.
		// However : by default, Qt wants to unpush a button on clic release. So we check it back.
		_uiUncheckAllButtons();
		_buttonToBasic->setChecked( true );

		if ( _mode == E_FORM_MODE::basic )
		{
			return;
		}

		_mode = E_FORM_MODE::basic;
		_switchToBasic();
	}

	void FormSwitchButton::_changeModeAdvanced()
	{
		_uiUncheckAllButtons();
		_buttonToAdvanced->setChecked( true );

		if ( _mode == E_FORM_MODE::advanced )
		{
			return;
		}

		_mode = E_FORM_MODE::advanced;
		_switchToAdvanced();
	}

	void FormSwitchButton::_uiUncheckAllButtons()
	{
		_buttonToBasic->setChecked( false );
		_buttonToAdvanced->setChecked( false );
	}

} // namespace VTX::Tool::Mdprep::ui
