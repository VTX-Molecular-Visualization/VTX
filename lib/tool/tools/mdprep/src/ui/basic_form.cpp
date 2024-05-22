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
		std::array<std::string_view, 4> g_IonicChargeValues { "-2", "-1", "+1", "+2" };

		QLineEdit * addUInt64Field( QFormLayout * p_dest, const char * p_label, const char * p_tooltip ) noexcept
		{
			QLineEdit * out = new QLineEdit();
			out->setValidator( new VTX::UI::QT::QUInt64Validator( out ) );
			p_dest->addRow( UI::QT::Util::createLabelWithHelpTooltip( p_label, p_tooltip ), out );
			return out;
		}

		// Validator class that enforce Chemical symbol user input
		class QChemicalSymbolValidator : public QValidator
		{
		  public:
			QChemicalSymbolValidator( QObject * p_parent ) : QValidator( p_parent ) {}

			virtual QValidator::State validate( QString & input, int & pos ) const override
			{
				if ( input.size() > 2 )
					return QValidator::State::Invalid;
				for ( auto & it_char : input )
				{
					if ( it_char < 'A' || ( it_char > 'Z' && it_char < 'a' ) || it_char > 'z' )
						return QValidator::State::Invalid;
				}
				return State::Acceptable;
			}
		};

	} // namespace
	void MdBasicParamForm::setupUi( QWidget * p_container, const MdParametersBasic & p_defaults )
	{
		QVBoxLayout * qMainLayout = new QVBoxLayout( p_container );

		VTX::UI::QT::Util::addLabeledHLineSeparator( qMainLayout, "Force Fields" );

		QFormLayout * qFormForceField = new QFormLayout;
		qMainLayout->addLayout( qFormForceField );

		_fieldForceFieldBio = new QComboBox;
		qFormForceField->addRow( "Force Field for biological molecules", _fieldForceFieldBio );
		QComboBox * fieldForceFieldChem = new QComboBox;
		fieldForceFieldChem->addItem( "Not supported by VTX yet" );
		qFormForceField->addRow( "Force Field for Small Organic molecules", fieldForceFieldChem );

		qMainLayout->addSpacerItem( new QSpacerItem( 0, 10 ) );
		VTX::UI::QT::Util::addLabeledHLineSeparator( qMainLayout, "MD Duration" );

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

		qMainLayout->addSpacerItem( new QSpacerItem( 0, 10 ) );
		VTX::UI::QT::Util::addLabeledHLineSeparator( qMainLayout, "Other Parameters" );

		QGridLayout * qLayoutIons = new QGridLayout;
		qMainLayout->addLayout( qLayoutIons );

		qLayoutIons->addWidget(
			UI::QT::Util::createLabelWithHelpTooltip(
				"Anion Symbol",
				"Chemical symbol of the anion used to balance system charges and/or to buffer the system."
			),
			0,
			0
		);
		_fieldAnionName = new QLineEdit;
		_fieldAnionName->setValidator( new QChemicalSymbolValidator( _fieldAnionName ) );
		qLayoutIons->addWidget( _fieldAnionName, 0, 1 );

		qLayoutIons->addWidget(
			UI::QT::Util::createLabelWithHelpTooltip(
				"Cation Symbol",
				"Chemical symbol of the cation used to balance system charges and/or to buffer the system."
			),
			1,
			0
		);
		_fieldCationName = new QLineEdit;
		_fieldCationName->setValidator( new QChemicalSymbolValidator( _fieldCationName ) );
		qLayoutIons->addWidget( _fieldCationName, 1, 1 );

		qLayoutIons->addWidget(
			UI::QT::Util::createLabelWithHelpTooltip( "Anion Charge", "Formal Charge of the forementionned Anion." ),
			0,
			2
		);

		_fieldAnionCharge  = new QComboBox;
		_fieldCationCharge = new QComboBox;
		for ( auto & it_chrgStr : g_IonicChargeValues )
		{
			_fieldAnionCharge->addItem( QString( it_chrgStr.data() ) );
			_fieldCationCharge->addItem( QString( it_chrgStr.data() ) );
		}

		qLayoutIons->addWidget( _fieldAnionCharge, 0, 3 );

		qLayoutIons->addWidget(
			UI::QT::Util::createLabelWithHelpTooltip( "Cation Charge", "Formal Charge of the forementionned Cation." ),
			1,
			2
		);
		qLayoutIons->addWidget( _fieldCationCharge, 1, 3 );

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
