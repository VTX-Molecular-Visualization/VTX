// #include <fmt/format.h>
#include <QFormLayout>
#include <QLabel>
#include <QLine>
#include <QLineEdit>
#include <QObject>
#include <QPushButton>
#include <QTimer>
#include <QValidator>
#include <fmt/format.h>
//
#include "tool/mdprep/gateway/form_data.hpp"
#include "tool/mdprep/ui/shared.hpp"
#include <util/sentry.hpp>
//
#include "tool/mdprep/ui/input_checker.hpp"
#include "tool/mdprep/ui/md_engine_specific_field_placer.hpp"
#include "tool/mdprep/ui/report.hpp"
//
#include "tool/mdprep/ui/form_basic/settings_dialog.hpp"
//
#include "app/vtx_app.hpp"
#include "tool/mdprep/ui/form_basic/event_manager.hpp"
//
#include "util/logger.hpp"
#include "util/sentry.hpp"
#include <ui/qt/util.hpp>
#include <ui/qt/validator.hpp>
//
#include "tool/mdprep/ui/form_basic/form_basic.hpp"

namespace VTX::Tool::Mdprep::ui::form_basic
{
	using namespace VTX::Tool::Mdprep::Gateway;
	using namespace VTX::UI::QT::Util;
	namespace
	{

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
	FormBasic::FormBasic(
		QWidget *					 p_container,
		SpecificFieldsPlacerCallback p_callback,
		const MdParameters &		 p_defaults,
		ReportManager &				 p_reportManager
	) : _data( std::make_unique<Data>( p_defaults, p_reportManager, std::move( p_callback ) ) ), _eventManager( *_data )
	{
		_createAndPlaceUiItems( p_container );
		_loadValues( p_defaults );
		_setupSlots();
	}
	void FormBasic::update( const EngineSpecificCommonInformation & p_data ) noexcept
	{
		Gateway::EngineSpecificCommonInformation * info = nullptr;
		_data->get( info );
		*info = p_data;
		_eventManager.performFirstInputCheck();
	}
	namespace
	{
		void createSettingButton( QPushButton ** p_out_buttonSettings )
		{
			*p_out_buttonSettings = new QPushButton;
			( *p_out_buttonSettings )->setFlat( true );
			( *p_out_buttonSettings )->setCursor( Qt::CursorShape::PointingHandCursor );
			( *p_out_buttonSettings )->setIcon( QIcon( ":/sprite/settings_icon.png" ) );
		}

		void addDurationRow(
			QFormLayout *  p_dest,
			const char *   p_label,
			const char *   p_tooltip,
			QLineEdit **   p_out_durationNumber,
			QComboBox **   p_out_durationUnit,
			QPushButton ** p_out_buttonSettings
		) noexcept
		{
			*p_out_durationNumber	   = addUInt64Field( p_dest, p_label, p_tooltip );
			QWidget * qWidgetRightCell = new QWidget;
			p_dest->replaceWidget( *p_out_durationNumber, qWidgetRightCell );
			QHBoxLayout * qLayoutRightCell = new QHBoxLayout( qWidgetRightCell );
			qLayoutRightCell->setContentsMargins( { 0, 0, 0, 0 } );
			qLayoutRightCell->addWidget( *p_out_durationNumber );
			*p_out_durationUnit = new QComboBox;
			qLayoutRightCell->addWidget( *p_out_durationUnit );
			for ( auto it_idxUnit = 0; it_idxUnit < static_cast<int>( E_MD_DURATION_UNIT::COUNT ); it_idxUnit++ )
				( *p_out_durationUnit )->addItem( string( static_cast<E_MD_DURATION_UNIT>( it_idxUnit ) ) );
			createSettingButton( p_out_buttonSettings );
			qLayoutRightCell->addWidget( *p_out_buttonSettings );
		}

		inline void figureUnitValueRelationWithoutLoss(
			uint64_t &	   p_duration,
			uint8_t &	   p_timeUnitIdx,
			const double & p_durationPs,
			const uint8_t  p_targetIdx
		)
		{
			double	multiplier	 = pow( 10, ( p_targetIdx - 1 ) * ( -3. ) );
			uint8_t resultingIdx = p_targetIdx;
			double	total		 = p_durationPs * multiplier;
			double	integralPart = 0.;
			double	decimalPart	 = std::modf( total, &integralPart );
			while ( decimalPart != 0 )
			{
				resultingIdx--;
				multiplier *= 1e3;
				total		 = p_durationPs * multiplier;
				integralPart = 0.;
				decimalPart	 = std::modf( total, &integralPart );
			}
			p_timeUnitIdx = resultingIdx;
			p_duration	  = static_cast<uint64_t>( integralPart );
		}

		// Given dt an stepnumber, will set the value and duration field to appropriate values
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
				figureUnitValueRelationWithoutLoss( duration, timeUnitIdx, durationPicoSeconds, 2 );
			}
			else
			{
				figureUnitValueRelationWithoutLoss( duration, timeUnitIdx, durationPicoSeconds, 3 );
			}
			p_fieldValue->setText( QString::number( duration ) );
			p_fieldUnit->setCurrentIndex( timeUnitIdx );
		}
	} // namespace

	void FormBasic::_createAndPlaceUiItems( QWidget * p_container ) noexcept
	{
		_uiObjects._parent				  = p_container;
		QVBoxLayout * _IntermediateLayout = new QVBoxLayout( p_container );
		_toDeleteObjects[ 0 ]			  = _IntermediateLayout;
		_uiObjects._container			  = new QWidget;
		_toDeleteObjects[ 1 ]			  = _uiObjects._container;
		QWidget * label					  = new QWidget;

		_IntermediateLayout->setContentsMargins( { 0, 0, 0, 0 } );
		_IntermediateLayout->addWidget( _uiObjects._container );
		QVBoxLayout * qMainLayout = new QVBoxLayout( _uiObjects._container );

		VTX::UI::QT::Util::addLabeledHLineSeparator( qMainLayout, "System Parameters" );

		_uiObjects._buttonInputCheck = new QPushButton( "Check System" );
		qMainLayout->addWidget( _uiObjects._buttonInputCheck );
		_eventManager.setInputCheck( _uiObjects._buttonInputCheck );

		QHBoxLayout * qLayoutForceField = new QHBoxLayout;
		qMainLayout->addLayout( qLayoutForceField );
		createSettingButton( &_uiObjects._buttonSystemSettings );
		_uiObjects._labelSystemCheckMsgContainer = new QWidget;
		_uiObjects._layoutSystemCheckMsg		 = new QVBoxLayout( _uiObjects._labelSystemCheckMsgContainer );

		_eventManager.setSystemMsg( _uiObjects._layoutSystemCheckMsg );
		qLayoutForceField->addWidget( _uiObjects._labelSystemCheckMsgContainer, 1 );
		qLayoutForceField->addWidget( _uiObjects._buttonSystemSettings );

		qMainLayout->addSpacerItem( new QSpacerItem( 0, 10 ) );
		VTX::UI::QT::Util::addLabeledHLineSeparator( qMainLayout, "MD Duration" );

		QFormLayout * qFormSteps = new QFormLayout;
		qMainLayout->addLayout( qFormSteps );

		VTX::Tool::Mdprep::ui::get(
			E_COMMON_FIELDS_UINT64::stepNumberMinimization, label, _uiObjects._fieldStepNumberMinimization
		);
		qFormSteps->addRow( label, _uiObjects._fieldStepNumberMinimization );

		QWidget * qMinizationWidget = new QWidget;
		qMinizationWidget->setContentsMargins( { 0, 0, 0, 0 } );
		qFormSteps->replaceWidget( _uiObjects._fieldStepNumberMinimization, qMinizationWidget );
		QHBoxLayout * qMinimizationLayout = new QHBoxLayout( qMinizationWidget );
		qMinimizationLayout->setContentsMargins( { 0, 0, 0, 0 } );

		qMinimizationLayout->addWidget( _uiObjects._fieldStepNumberMinimization, 1 );
		createSettingButton( &_uiObjects._buttonMinimizationSettings );
		qMinimizationLayout->addWidget( _uiObjects._buttonMinimizationSettings, 0 );

		addDurationRow(
			qFormSteps,
			"Equilibration NVT duration",
			"Equilibration of the restrained system at constant volume and temperature.<br>When a MD is performed, the "
			"system must be stable before running unrestrained production run.<br> The larger the system, the longer "
			"shall be the equilibration.",
			&_uiObjects._fieldDurationValueEquilibrationNvt,
			&_uiObjects._fieldDurationUnitEquilibrationNvt,
			&_uiObjects._buttonEquilibrationNvtSettings
		);
		addDurationRow(
			qFormSteps,
			"Equilibration NPT duration",
			"Equilibration of the restrained system at constant pressure and temperature.<br>When a MD is performed, "
			"the system must be stable before running unrestrained production run.<br> The larger the system, the "
			"longer shall be the equilibration.",
			&_uiObjects._fieldDurationValueEquilibrationNpt,
			&_uiObjects._fieldDurationUnitEquilibrationNpt,
			&_uiObjects._buttonEquilibrationNptSettings
		);
		addDurationRow(
			qFormSteps,
			"Production duration",
			"Production run refers to the actual unrestrained molecular dynamic simulation.",
			&_uiObjects._fieldDurationValueProduction,
			&_uiObjects._fieldDurationUnitProduction,
			&_uiObjects._buttonProductionSettings
		);

		qMainLayout->addSpacerItem( new QSpacerItem( 0, 10 ) );
		VTX::UI::QT::Util::addLabeledHLineSeparator( qMainLayout, "Ions" );

		Gateway::MdParameters * params = nullptr;
		_data->get( params );
		_fieldsIons = IonicFields( qMainLayout, params->system );

		_uiObjects._layoutIons = new QFormLayout;
		qMainLayout->addLayout( _uiObjects._layoutIons );

		qMainLayout->addStretch( 1 );
	}
	void FormBasic::_loadValues( const MdParameters & p_defaults ) noexcept
	{
		_uiObjects._fieldStepNumberMinimization->setText( QString::number( p_defaults.minimization.stepNumber ) );
		figureBestValueUnitRelation(
			p_defaults.nvt.dt,
			p_defaults.nvt.nsteps,
			_uiObjects._fieldDurationValueEquilibrationNvt,
			_uiObjects._fieldDurationUnitEquilibrationNvt
		);
		figureBestValueUnitRelation(
			p_defaults.npt.dt,
			p_defaults.npt.nsteps,
			_uiObjects._fieldDurationValueEquilibrationNpt,
			_uiObjects._fieldDurationUnitEquilibrationNpt
		);
		figureBestValueUnitRelation(
			p_defaults.prod.dt,
			p_defaults.prod.nsteps,
			_uiObjects._fieldDurationValueProduction,
			_uiObjects._fieldDurationUnitProduction
		);
	}
	void FormBasic::_setupSlots() noexcept
	{
		_eventManager.setMinimizationSettings( _uiObjects._buttonMinimizationSettings );
		_eventManager.setNvtSettings( _uiObjects._buttonEquilibrationNvtSettings );
		_eventManager.setNptSettings( _uiObjects._buttonEquilibrationNptSettings );
		_eventManager.setProductionSettings( _uiObjects._buttonProductionSettings );
		_eventManager.setSystemSettings( _uiObjects._buttonSystemSettings );
	}
	namespace
	{
		inline double multiplierPs( const std::string_view p_unit )
		{
			if ( p_unit == "fs" )
				return 1e-3;
			if ( p_unit == "ps" )
				return 1e0;
			if ( p_unit == "ns" )
				return 1e3;
			if ( p_unit == "\xc2\xb5s" )
				return 1e6;
			return 0.;
		}
		inline void figureNumStep(
			Gateway::MdParametersStepData & p_data,
			QComboBox *						p_unitField,
			QLineEdit *						p_valueField
		)
		{
			p_data.nsteps = static_cast<uint64_t>(
				multiplierPs( p_unitField->currentText().toStdString() ) * p_valueField->text().toDouble() / p_data.dt
			);
		}

	} // namespace
	void FormBasic::get( MdParameters & p_in ) const noexcept
	{
		Gateway::MdParameters * params = nullptr;
		_data->get( params );
		p_in						 = *params;
		p_in.minimization.stepNumber = _uiObjects._fieldStepNumberMinimization->text().toUInt();

		_fieldsIons.get( p_in.system );

		figureNumStep(
			p_in.nvt, _uiObjects._fieldDurationUnitEquilibrationNvt, _uiObjects._fieldDurationValueEquilibrationNvt
		);
		figureNumStep(
			p_in.npt, _uiObjects._fieldDurationUnitEquilibrationNpt, _uiObjects._fieldDurationValueEquilibrationNpt
		);
		figureNumStep( p_in.prod, _uiObjects._fieldDurationUnitProduction, _uiObjects._fieldDurationValueProduction );
	}
	void FormBasic::get( FormLayouts & p_out ) const noexcept { p_out.baseParametersIon = _uiObjects._layoutIons; }

} // namespace VTX::Tool::Mdprep::ui::form_basic
