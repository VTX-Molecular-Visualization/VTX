#include <string>
#include <vector>
//
#include <ui/qt/util.hpp>
//
#include "tools/mdprep/gateway/backend_gromacs.hpp"
#include "tools/mdprep/gateway/form_data.hpp"
#include "tools/mdprep/gateway/shared.hpp"
#include "tools/mdprep/ui/md_engine_factory.hpp"
#include "tools/mdprep/ui/shared.hpp"
//
#include "tools/mdprep/ui/md_engine_specific_field_placer.hpp"
//
#include "tools/mdprep/ui/gromacs_field_placer.hpp"
#include "tools/mdprep/ui/md_engine_field_placer.hpp"
//
#include <qformlayout.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qtabwidget.h>
//
#include "tools/mdprep/ui/form_switch_button.hpp"

namespace VTX::Tool::Mdprep::ui
{
	using namespace VTX::Tool::Mdprep::Gateway;

	namespace
	{
		inline void setTextBasic( QPushButton * p_ ) noexcept { p_->setText( "Switch to Simple view" ); }
		inline void setTextAdvanced( QPushButton * p_ ) noexcept { p_->setText( "Switch to Advanced view" ); }
	} // namespace

	void FormSwitchButton::setupUi( QLayout * p_layout, const E_FORM_MODE & p_mode ) noexcept
	{
		_buttonViewSwitch		   = new QPushButton;
		QWidget * wButtonContainer = new QWidget;
		p_layout->addWidget( wButtonContainer );
		QHBoxLayout * qLayoutButton = new QHBoxLayout( wButtonContainer );
		qLayoutButton->setContentsMargins( { 0, 0, 0, 0 } );
		qLayoutButton->addStretch( 1 );
		qLayoutButton->addWidget( _buttonViewSwitch );
		QObject::connect( _buttonViewSwitch, &QPushButton::clicked, [ & ]() { this->switchFormMode(); } );
		_mode = p_mode;
		switch ( _mode )
		{
		case VTX::Tool::Mdprep::ui::FormSwitchButton::E_FORM_MODE::basic: setTextAdvanced( _buttonViewSwitch ); break;
		case VTX::Tool::Mdprep::ui::FormSwitchButton::E_FORM_MODE::advanced: setTextBasic( _buttonViewSwitch ); break;
		default: break;
		}
	}

	void FormSwitchButton::subscribeBasicSwitch( Callback p_ ) { _switchToBasic = std::move( p_ ); }

	void FormSwitchButton::subscribeAdvancedSwitch( Callback p_ ) { _switchToAdvanced = std::move( p_ ); }

	void FormSwitchButton::switchFormMode() noexcept
	{
		if ( _mode == E_FORM_MODE::basic )
		{
			_changeModeAdvanced();
			_mode = E_FORM_MODE::advanced;
		}
		else
		{
			_changeModeBasic();
			_mode = E_FORM_MODE::basic;
		}
	}

	void FormSwitchButton::_changeModeBasic() noexcept
	{
		setTextAdvanced( _buttonViewSwitch );
		_switchToBasic();
	}

	void FormSwitchButton::_changeModeAdvanced() noexcept
	{
		setTextBasic( _buttonViewSwitch );
		_switchToAdvanced();
	}

} // namespace VTX::Tool::Mdprep::ui
