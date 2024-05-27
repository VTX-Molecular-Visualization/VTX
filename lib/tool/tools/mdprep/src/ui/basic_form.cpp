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
#include "util/logger.hpp"
#include <ui/qt/util.hpp>
#include <ui/qt/validator.hpp>
//
#include "tools/mdprep/ui/basic_form.hpp"

namespace VTX::Tool::Mdprep::ui
{
	using namespace VTX::UI::QT::Util;
	namespace
	{
		std::array<std::string_view, 4> g_IonicChargeValues { "-2", "-1", "+1", "+2" };

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
		_loadValues( p_defaults );
	}
	void MdBasicParamForm::update( const EngineSpecificCommonFormData & p_data ) noexcept
	{
		_fieldForceFieldBio->clear();
		for ( auto & it_forcefieldStr : p_data.bioForcefieldCollection )
		{
			_fieldForceFieldBio->addItem( QString::fromStdString( it_forcefieldStr ) );
		}
	}
	namespace
	{

		void updateDurationLabel( QLabel * p_label, const QString & p_str, const double & p_dt ) noexcept
		{
			const double							 durationDoublePs = p_str.toDouble() * p_dt;
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

			const std::string formatedStr = std::format( "{:d}{} (dt = {:0.3f}ps)", duration, timeUnit, p_dt );
			p_label->setText( QString::fromStdString( formatedStr ) );
		}

		void addDurationRow(
			QFormLayout * p_dest,
			const char *  p_label,
			const char *  p_tooltip,
			QLineEdit **  p_out_durationNumber,
			QComboBox **  p_out_durationUnit
		) noexcept
		{
			*p_out_durationNumber	  = addUInt64Field( p_dest, p_label, p_tooltip );
			QWidget * qWidgetLeftCell = new QWidget;
			p_dest->replaceWidget( *p_out_durationNumber, qWidgetLeftCell );
			QHBoxLayout * qLayoutLeftCell = new QHBoxLayout( qWidgetLeftCell );
			qLayoutLeftCell->setContentsMargins( { 0, 0, 0, 0 } );
			qLayoutLeftCell->addWidget( *p_out_durationNumber );
			*p_out_durationUnit = new QComboBox;
			qLayoutLeftCell->addWidget( *p_out_durationUnit );
			for ( auto it_idxUnit = 0; it_idxUnit < static_cast<int>( E_MD_DURATION_UNIT::COUNT ); it_idxUnit++ )
				( *p_out_durationUnit )->addItem( string( static_cast<E_MD_DURATION_UNIT>( it_idxUnit ) ) );
		}
		void figureBestValueUnitRelation(
			const double &	 p_dt,
			const uint64_t & p_stepNumber,
			QLineEdit *		 p_fieldValue,
			QComboBox *		 p_fieldUnit
		)
		{
			double	 durationPicoSeconds = p_dt * p_stepNumber;
			uint8_t	 timeUnitIdx		 = 0;
			uint64_t duration			 = 0;
			if ( durationPicoSeconds < 1e0 )
			{
				timeUnitIdx = 0;
				duration	= static_cast<uint64_t>( durationPicoSeconds * 1e3 );
			}
			else if ( durationPicoSeconds < 1e3 )
			{
				timeUnitIdx = 1;
				duration	= static_cast<uint64_t>( durationPicoSeconds );
			}
			else if ( durationPicoSeconds < 1e6 )
			{
				timeUnitIdx = 2;
				duration	= static_cast<uint64_t>( durationPicoSeconds * 1e-3 );
			}
			else
			{
				timeUnitIdx = 3;
				duration	= static_cast<uint64_t>( durationPicoSeconds * 1e-6 );
			}
			p_fieldValue->setText( QString::number( duration ) );
			p_fieldUnit->setCurrentIndex( timeUnitIdx );
		}
	} // namespace
	void MdBasicParamForm::update( const MdAdvancedDataSample & p_data ) noexcept
	{
		VTX::VTX_DEBUG( "MdBasicParamForm::update" );
		_lastSeenAdvancedDataSample = p_data;
		figureBestValueUnitRelation(
			p_data.dtNvt, p_data.stepNumberNvt, _fieldDurationValueEquilibrationNvt, _fieldDurationUnitEquilibrationNvt
		);
		figureBestValueUnitRelation(
			p_data.dtNpt, p_data.stepNumberNpt, _fieldDurationValueEquilibrationNpt, _fieldDurationUnitEquilibrationNpt
		);
		figureBestValueUnitRelation(
			p_data.dtProd, p_data.stepNumberProd, _fieldDurationValueProduction, _fieldDurationUnitProduction
		);
	}
	void MdBasicParamForm::subscribe( UpdateCallback p_ ) noexcept { _updateCallback = p_; }
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
		addDurationRow(
			qFormSteps,
			"Label equi nvt",
			"tooltip equi nvt",
			&_fieldDurationValueEquilibrationNvt,
			&_fieldDurationUnitEquilibrationNvt
		);
		auto lambdaChangeSignal = [ & ]() { this->_sendChangeSignal(); };
		QLineEdit::connect( _fieldDurationValueEquilibrationNvt, &QLineEdit::textChanged, lambdaChangeSignal );
		QComboBox::connect( _fieldDurationUnitEquilibrationNvt, &QComboBox::currentIndexChanged, lambdaChangeSignal );
		addDurationRow(
			qFormSteps,
			"Label equi npt",
			"tooltip equi npt",
			&_fieldDurationValueEquilibrationNpt,
			&_fieldDurationUnitEquilibrationNpt
		);
		QLineEdit::connect( _fieldDurationValueEquilibrationNpt, &QLineEdit::textChanged, lambdaChangeSignal );
		QComboBox::connect( _fieldDurationUnitEquilibrationNpt, &QComboBox::currentIndexChanged, lambdaChangeSignal );
		addDurationRow(
			qFormSteps, "Label prod", "tooltip prod", &_fieldDurationValueProduction, &_fieldDurationUnitProduction
		);
		QLineEdit::connect( _fieldDurationValueProduction, &QLineEdit::textChanged, lambdaChangeSignal );
		QComboBox::connect( _fieldDurationUnitProduction, &QComboBox::currentIndexChanged, lambdaChangeSignal );

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
			if ( std::stol( it_chrgStr.data() ) > 0 )
				_fieldCationCharge->addItem( QString( it_chrgStr.data() ) );

			if ( std::stol( it_chrgStr.data() ) < 0 )
				_fieldAnionCharge->addItem( QString( it_chrgStr.data() ) );
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
	void MdBasicParamForm::_loadValues( const MdParametersBasic & p_defaults ) noexcept
	{
		for ( int it_idx = 0; _fieldForceFieldBio->count(); it_idx++ )
			if ( _fieldForceFieldBio->itemText( it_idx ).toStdString() == p_defaults.forcefieldBio )
			{
				_fieldForceFieldBio->setCurrentIndex( it_idx );
				break;
			}
		_fieldStepNumberMinimization->setText( QString::number( p_defaults.stepNumberMinimization ) );
		_fieldDurationValueEquilibrationNvt->setText( QString::number( p_defaults.DurationValueEquilibrationNvt ) );
		_fieldDurationUnitEquilibrationNvt->setCurrentIndex( static_cast<int>( p_defaults.DurationUnitNvt ) );
		_fieldDurationValueEquilibrationNpt->setText( QString::number( p_defaults.DurationValueEquilibrationNpt ) );
		_fieldDurationUnitEquilibrationNpt->setCurrentIndex( static_cast<int>( p_defaults.DurationUnitNpt ) );
		_fieldDurationValueProduction->setText( QString::number( p_defaults.DurationValueProduction ) );
		_fieldDurationUnitProduction->setCurrentIndex( static_cast<int>( p_defaults.DurationUnitProd ) );
		_fieldAnionName->setText( QString::fromStdString( p_defaults.anionName ) );
		_fieldCationName->setText( QString::fromStdString( p_defaults.cationName ) );

		for ( int it_idx = 0; it_idx < g_IonicChargeValues.size(); it_idx++ )
		{
			auto & it_chrgStr = g_IonicChargeValues[ it_idx ];
			if ( std::stol( it_chrgStr.data() ) == p_defaults.anionCharge )
				//_fieldAnionCharge->setCurrentIndex( it_idx );
				_fieldAnionCharge->setCurrentText( g_IonicChargeValues[ it_idx ].data() );
			if ( std::stol( it_chrgStr.data() ) == p_defaults.cationCharge )
				_fieldCationCharge->setCurrentText( g_IonicChargeValues[ it_idx ].data() );
		}
	}
	void MdBasicParamForm::_sendChangeSignal() noexcept
	{
		MdBasicDataSample dataSample;
		dataSample.durationEquilibrationNvt = _fieldDurationValueEquilibrationNvt->text().toInt();
		dataSample.unitEquilibrationNvt
			= static_cast<E_MD_DURATION_UNIT>( _fieldDurationUnitEquilibrationNvt->currentIndex() );
		dataSample.durationEquilibrationNpt = _fieldDurationValueEquilibrationNpt->text().toInt();
		dataSample.unitEquilibrationNpt
			= static_cast<E_MD_DURATION_UNIT>( _fieldDurationUnitEquilibrationNpt->currentIndex() );
		dataSample.durationProduction = _fieldDurationValueProduction->text().toInt();
		dataSample.unitProduction	  = static_cast<E_MD_DURATION_UNIT>( _fieldDurationUnitProduction->currentIndex() );
		_updateCallback( dataSample );
	}
	void		  MdBasicParamForm::get( MdParametersBasic & ) const noexcept {}
	QFormLayout * MdBasicParamForm::layoutFieldsMdEngine() noexcept { return _layoutFieldEngineSpecific; }
} // namespace VTX::Tool::Mdprep::ui
