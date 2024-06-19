#include <string>
#include <vector>
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
#include "tools/mdprep/ui/md_field_organizer.hpp"

namespace VTX::Tool::Mdprep::ui
{
	using namespace VTX::Tool::Mdprep::Gateway;

	void MdFieldsOrganizer::setupUi( QLayout * p_layout, const E_FORM_MODE & p_mode ) noexcept
	{
		_buttonViewSwitch		   = new QPushButton;
		QWidget * wButtonContainer = new QWidget;
		p_layout->addWidget( wButtonContainer );
		QHBoxLayout * qLayoutButton = new QHBoxLayout( wButtonContainer );
		qLayoutButton->setContentsMargins( { 0, 0, 0, 0 } );
		qLayoutButton->addStretch( 1 );
		qLayoutButton->addWidget( _buttonViewSwitch );
		QObject::connect( _buttonViewSwitch, &QPushButton::clicked, [ & ]() { this->switchFormMode(); } );
		containerParamBasic	   = new QWidget();
		containerParamAdvanced = new QWidget();
		p_layout->addWidget( containerParamBasic );
		p_layout->addWidget( containerParamAdvanced );

		_mode = p_mode;
		if ( _mode == E_FORM_MODE::basic )
			_changeModeBasic();
		else
			_changeModeAdvanced();
	}

	void MdFieldsOrganizer::switchFormMode() noexcept
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

	void MdFieldsOrganizer::_changeModeBasic() noexcept
	{
		_buttonViewSwitch->setText( "Switch to Advanced view" );
		containerParamAdvanced->hide();
		containerParamBasic->show();
	}

	void MdFieldsOrganizer::_changeModeAdvanced() noexcept
	{
		_buttonViewSwitch->setText( "Switch to Simple view" );
		containerParamBasic->hide();
		containerParamAdvanced->show();
	}

} // namespace VTX::Tool::Mdprep::ui
