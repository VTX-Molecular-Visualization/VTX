#include <qformlayout.h>
#include <qlabel.h>
#include <qline.h>
#include <qlineedit.h>
#include <qvalidator.h>
//
#include "tools/mdprep/ui/form_data.hpp"
#include <ui/qt/util.hpp>
#include <ui/qt/validator.hpp>
//
#include "tools/mdprep/ui/basic_form.hpp"

namespace VTX::Tool::Mdprep::ui
{
	namespace
	{
		QLineEdit * addUInt64Field( QFormLayout * p_dest, const char * p_label, const char * p_tooltip ) noexcept
		{
			QLineEdit * out = new QLineEdit();
			out->setValidator( new VTX::UI::QT::QUInt64Validator( out ) );
			p_dest->addRow( UI::QT::Util::createLabelWithHelpTooltip( p_label, p_tooltip ), out );
			return out;
		}
	} // namespace
	void MdBasicParamForm::setupUi( QWidget * p_container, const MdParametersBasic & p_defaults )
	{
		QVBoxLayout * qMainLayout = new QVBoxLayout( p_container );

		QFormLayout * qFormForceField = new QFormLayout;
		qMainLayout->addLayout( qFormForceField );

		_fieldForceFieldBio = new QComboBox;
		qFormForceField->addRow( "Force Field for biological molecules", _fieldForceFieldBio );
		QComboBox * fieldForceFieldChem = new QComboBox;
		fieldForceFieldChem->addItem( "Not supported by VTX yet" );
		qFormForceField->addRow( "Force Field for Small Organic molecules", fieldForceFieldChem );

		auto qLine1 = new QFrame;
		qLine1->setFrameShape( QFrame::HLine );
		qLine1->setFrameShadow( QFrame::Sunken );
		qMainLayout->addWidget( qLine1 );

		QFormLayout * qFormSteps = new QFormLayout;
		qMainLayout->addLayout( qFormSteps );
		_fieldStepNumberMinimization = addUInt64Field(
			qFormSteps,
			"Number of Minimization Steps",
			"Dictates how long the minimization can be. Bigger systems need higher number of "
			"steps.\nMinimization "
			"shall still end earlier if the energy plateau is reached."
		);
		_fieldStepNumberEquilibrationNvt = addUInt64Field(
			qFormSteps,
			"Number of Nvt Equilibration Steps",
			"Number of Equilibration steps with constant volume and temperature.\nThis is the first equilibration "
			"step. Bigger systems need higher number of steps.\n"
		);
		_fieldStepNumberEquilibrationNpt = addUInt64Field(
			qFormSteps,
			"Number of Npt Equilibration Steps",
			"Number of Equilibration steps with constant pressure and temperature.\nThis is the second and last "
			"equilibration step. Bigger systems need higher number of steps.\n"
		);
		_fieldStepNumberProduction = addUInt64Field(
			qFormSteps, "Number of Production Steps", "Actual trajectory that you'll be able to load and analyse.\n"
		);

		auto qLine2 = new QFrame;
		qLine2->setFrameShape( QFrame::HLine );
		qLine2->setFrameShadow( QFrame::Sunken );
		qMainLayout->addWidget( qLine2 );

		_layoutFields = new QFormLayout;
		qMainLayout->addLayout( _layoutFields );
		qMainLayout->addStretch( 1 );
	}
	void MdBasicParamForm::update( const EngineSpecificCommonFormData & p_data ) noexcept
	{
		_fieldForceFieldBio->clear();
		for ( auto & it_forcefieldStr : p_data.bioForcefieldCollection )
		{
			_fieldForceFieldBio->addItem( QString::fromStdString( it_forcefieldStr ) );
		}
	}
	void		  MdBasicParamForm::get( MdParametersBasic & ) const noexcept {}
	QFormLayout * MdBasicParamForm::layoutFieldsMdEngine() noexcept { return _layoutFields; }
} // namespace VTX::Tool::Mdprep::ui
