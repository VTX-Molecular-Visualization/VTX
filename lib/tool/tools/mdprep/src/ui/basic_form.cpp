// #include <fmt/format.h>
#include <format>
#include <qformlayout.h>
#include <qlabel.h>
#include <qline.h>
#include <qlineedit.h>
#include <qobject.h>
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
		void addUInt64FieldWithRightLabel(
			QFormLayout * p_dest,
			const char *  p_label,
			const char *  p_tooltip,
			QLineEdit **  p_out_field,
			QLabel **	  p_out_label
		) noexcept
		{
			*p_out_field				  = new QLineEdit();
			*p_out_label				  = new QLabel;
			QWidget *	  qRightCol		  = new QWidget;
			QHBoxLayout * qRightColLayout = new QHBoxLayout( qRightCol );
			qRightColLayout->setContentsMargins( 0, 0, 0, 0 );
			qRightColLayout->addWidget( *p_out_field, 1 );
			qRightColLayout->addWidget( *p_out_label, 1 );

			( *p_out_field )->setValidator( new VTX::UI::QT::QUInt64Validator( *p_out_field ) );

			p_dest->addRow( UI::QT::Util::createLabelWithHelpTooltip( p_label, p_tooltip ), qRightCol );
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
		_createAndPlaceUiItems( p_container );
		_loadDefaultValues( p_defaults );
	}
	void MdBasicParamForm::update( const EngineSpecificCommonFormData & p_data ) noexcept
	{
		_fieldForceFieldBio->clear();
		for ( auto & it_forcefieldStr : p_data.bioForcefieldCollection )
		{
			_fieldForceFieldBio->addItem( QString::fromStdString( it_forcefieldStr ) );
		}
	}
	void MdBasicParamForm::_updateDurationLabel( QLabel * p_label, const QString & p_str ) noexcept
	{
		const double							 durationDoublePs = p_str.toDouble() * _lastSeenAdvancedDataSample.dt;
		static const std::array<const char *, 4> timeUnits { "fs", "ps", "ns", "\xc2\xb5s" };
		const char *							 timeUnit;
		uint64_t								 duration = 0;

		if ( durationDoublePs < 1e0 )
		{
			timeUnit = timeUnits[ 0 ];
			duration = static_cast<uint64_t>( durationDoublePs * 1e3 );
		}
		else if ( durationDoublePs < 1e3 )
		{
			timeUnit = timeUnits[ 1 ];
			duration = static_cast<uint64_t>( durationDoublePs );
		}
		else if ( durationDoublePs < 1e6 )
		{
			timeUnit = timeUnits[ 2 ];
			duration = static_cast<uint64_t>( durationDoublePs * 1e-3 );
		}
		else
		{
			timeUnit = timeUnits[ 3 ];
			duration = static_cast<uint64_t>( durationDoublePs * 1e-6 );
		}

		const std::string formatedStr
			= std::format( "{:d}{} (dt = {:0.3f}ps)", duration, timeUnit, _lastSeenAdvancedDataSample.dt );
		p_label->setText( QString::fromStdString( formatedStr ) );
	}
	void MdBasicParamForm::update( const MdAdvancedDataSample & p_data ) noexcept
	{
		_lastSeenAdvancedDataSample = p_data;
	}
	void MdBasicParamForm::_createAndPlaceUiItems( QWidget * p_container ) noexcept
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
		addUInt64FieldWithRightLabel(
			qFormSteps,
			"Number of Nvt Equilibration Steps",
			"Number of Equilibration steps with constant volume and temperature.\nThis is the first equilibration "
			"step. Bigger systems need higher number of steps.\n",
			&_fieldStepNumberEquilibrationNvt,
			&_labelDurationEquilibrationNvt
		);
		_fieldStepNumberEquilibrationNvt->connect(
			_fieldStepNumberEquilibrationNvt,
			&QLineEdit::textChanged,
			[ & ]( const QString & p_ ) { this->_updateDurationLabel( this->_labelDurationEquilibrationNvt, p_ ); }
		);
		addUInt64FieldWithRightLabel(
			qFormSteps,
			"Number of Npt Equilibration Steps",
			"Number of Equilibration steps with constant pressure and temperature.\nThis is the second and last "
			"equilibration step. Bigger systems need higher number of steps.\n",
			&_fieldStepNumberEquilibrationNpt,
			&_labelDurationEquilibrationNpt
		);
		_fieldStepNumberEquilibrationNpt->connect(
			_fieldStepNumberEquilibrationNpt,
			&QLineEdit::textChanged,
			[ & ]( const QString & p_ ) { this->_updateDurationLabel( this->_labelDurationEquilibrationNpt, p_ ); }
		);
		addUInt64FieldWithRightLabel(
			qFormSteps,
			"Number of Production Steps",
			"Actual trajectory that you'll be able to load and analyse.\n",
			&_fieldStepNumberProduction,
			&_labelDurationProduction
		);
		_fieldStepNumberProduction->connect(
			_fieldStepNumberProduction,
			&QLineEdit::textChanged,
			[ & ]( const QString & p_ ) { this->_updateDurationLabel( this->_labelDurationProduction, p_ ); }
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

		_layoutFieldEngineSpecific = new QFormLayout;
		qMainLayout->addLayout( _layoutFieldEngineSpecific );

		qMainLayout->addStretch( 1 );
	}
	void MdBasicParamForm::_loadDefaultValues( const MdParametersBasic & p_defaults ) noexcept
	{
		for ( int it_idx = 0; _fieldForceFieldBio->count(); it_idx++ )
			if ( _fieldForceFieldBio->itemText( it_idx ).toStdString() == p_defaults.forcefieldBio )
			{
				_fieldForceFieldBio->setCurrentIndex( it_idx );
				break;
			}
		_fieldStepNumberMinimization->setText( QString::number( p_defaults.stepNumberMinimization ) );
		_fieldStepNumberEquilibrationNvt->setText( QString::number( p_defaults.stepNumberEquilibrationNvt ) );
		_fieldStepNumberEquilibrationNpt->setText( QString::number( p_defaults.stepNumberEquilibrationNpt ) );
		_fieldStepNumberProduction->setText( QString::number( p_defaults.stepNumberProduction ) );
		_fieldAnionName->setText( QString::fromStdString( p_defaults.anionName ) );
		_fieldCationName->setText( QString::fromStdString( p_defaults.cationName ) );

		for ( int it_idx = 0; it_idx < g_IonicChargeValues.size(); it_idx++ )
		{
			auto & it_chrgStr = g_IonicChargeValues[ it_idx ];
			if ( std::stol( it_chrgStr.data() ) == p_defaults.anionCharge )
				_fieldAnionCharge->setCurrentIndex( it_idx );
			if ( std::stol( it_chrgStr.data() ) == p_defaults.cationCharge )
				_fieldCationCharge->setCurrentIndex( it_idx );
		}
	}
	void		  MdBasicParamForm::get( MdParametersBasic & ) const noexcept {}
	QFormLayout * MdBasicParamForm::layoutFieldsMdEngine() noexcept { return _layoutFieldEngineSpecific; }
} // namespace VTX::Tool::Mdprep::ui
