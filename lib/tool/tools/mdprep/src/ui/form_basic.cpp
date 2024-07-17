// #include <fmt/format.h>
#include <format>
#include <qformlayout.h>
#include <qlabel.h>
#include <qline.h>
#include <qlineedit.h>
#include <qobject.h>
#include <qpushbutton.h>
#include <qvalidator.h>
//
#include "tools/mdprep/gateway/form_data.hpp"
#include "tools/mdprep/ui/input_checker.hpp"
#include "tools/mdprep/ui/md_engine_specific_field_placer.hpp"
#include "tools/mdprep/ui/shared.hpp"
//
#include "tools/mdprep/ui/form_basic_settings_dialog.hpp"
#include "util/logger.hpp"
#include <ui/qt/util.hpp>
#include <ui/qt/validator.hpp>
#include <ui/qt/widget_factory.hpp>
//
#include "tools/mdprep/ui/form_basic.hpp"

namespace VTX::Tool::Mdprep::ui
{
	using namespace VTX::Tool::Mdprep::Gateway;
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
	FormBasic::FormBasic(
		QWidget *					 p_container,
		SpecificFieldsPlacerCallback p_callback,
		const MdParameters &		 p_defaults,
		InputChecker				 p_inputChecker
	) : _settingsButtons( p_defaults, std::move( p_callback ), std::move( p_inputChecker ) )
	{
		_createAndPlaceUiItems( p_container );
		_loadValues( p_defaults );
		_setupSlots();
	}
	void FormBasic::update( const EngineSpecificCommonInformation & p_data ) noexcept
	{
		_settingsButtons.lastFormData = p_data;
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

		QHBoxLayout * qLayoutForceField = new QHBoxLayout;
		qMainLayout->addLayout( qLayoutForceField );
		createSettingButton( &_uiObjects._buttonSystemSettings );
		_uiObjects._labelSystemCheckMsg = new QLabel;
		_uiObjects._labelSystemCheckMsg->setText( "Checking system compatibility with default parameters ..." );
		_uiObjects._labelSystemCheckMsg->setWordWrap( true );
		qLayoutForceField->addWidget( _uiObjects._labelSystemCheckMsg, 1 );
		qLayoutForceField->addWidget( _uiObjects._buttonSystemSettings );

		qMainLayout->addSpacerItem( new QSpacerItem( 0, 10 ) );
		VTX::UI::QT::Util::addLabeledHLineSeparator( qMainLayout, "MD Duration" );

		QFormLayout * qFormSteps = new QFormLayout;
		qMainLayout->addLayout( qFormSteps );

		VTX::Tool::Mdprep::ui::get(
			E_COMMON_FIELDS_UINT64::stepNumber, label, _uiObjects._fieldStepNumberMinimization
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

		_fieldsIons = IonicFields( qMainLayout, _settingsButtons.parameters.system );

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
		_settingsButtons.setMinimizationSettings( _uiObjects._buttonMinimizationSettings );
		_settingsButtons.setNvtSettings( _uiObjects._buttonEquilibrationNvtSettings );
		_settingsButtons.setNptSettings( _uiObjects._buttonEquilibrationNptSettings );
		_settingsButtons.setProductionSettings( _uiObjects._buttonProductionSettings );
		_settingsButtons.setSystemSettings( _uiObjects._buttonSystemSettings );
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
		p_in						 = _settingsButtons.parameters;
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

	FormBasic::SettingsButtons::SettingsButtons(
		MdParameters				 p_data,
		SpecificFieldsPlacerCallback p_callback,
		InputChecker				 p_inputChecker
	) noexcept :
		parameters( std::move( p_data ) ), inputChecker( std::move( p_inputChecker ) ),
		_mdEngineFieldPlacerCallback( std::move( p_callback ) )
	{
	}

	FormBasic::SettingsButtons::~SettingsButtons() { _disconnectAll( _uiObjects ); }

	FormBasic::SettingsButtons::SettingsButtons( SettingsButtons && p_ ) noexcept :
		parameters( std::move( p_.parameters ) ), inputChecker( std::move( p_.inputChecker ) ),
		lastFormData( std::move( p_.lastFormData ) ),
		_mdEngineFieldPlacerCallback( std::move( p_._mdEngineFieldPlacerCallback ) ),
		_settingsDialog( std::move( p_._settingsDialog ) )
	{
		_disconnectAll( p_._uiObjects );
		std::swap( p_._uiObjects, _uiObjects );
		_connectSettingsMinimization();
		_connectSettingsNvt();
		_connectSettingsNpt();
		_connectSettingsProd();
		_connectSettingsSystem();
	}

	FormBasic::SettingsButtons & FormBasic::SettingsButtons::operator=( SettingsButtons && p_ ) noexcept
	{
		if ( &p_ == this )
			return *this;

		parameters					 = std::move( p_.parameters );
		lastFormData				 = std::move( p_.lastFormData );
		_mdEngineFieldPlacerCallback = std::move( p_._mdEngineFieldPlacerCallback );
		_settingsDialog				 = std::move( p_._settingsDialog );

		_disconnectAll( _uiObjects );
		_disconnectAll( p_._uiObjects );
		_uiObjects	  = std::move( p_._uiObjects );
		p_._uiObjects = UiObjects();
		_connectSettingsMinimization();
		_connectSettingsNvt();
		_connectSettingsNpt();
		_connectSettingsProd();
		_connectSettingsSystem();

		return *this;
	}

	void FormBasic::SettingsButtons::setMinimizationSettings( QPushButton * p_ ) noexcept
	{
		_uiObjects._buttonMinimizationSettings = p_;
		_connectSettingsMinimization();
	}

	void FormBasic::SettingsButtons::setNvtSettings( QPushButton * p_ ) noexcept
	{
		_uiObjects._buttonEquilibrationNvtSettings = p_;
		_connectSettingsNvt();
	}

	void FormBasic::SettingsButtons::setNptSettings( QPushButton * p_ ) noexcept
	{
		_uiObjects._buttonEquilibrationNptSettings = p_;
		_connectSettingsNpt();
	}

	void FormBasic::SettingsButtons::setProductionSettings( QPushButton * p_ ) noexcept
	{
		_uiObjects._buttonProductionSettings = p_;
		_connectSettingsProd();
	}

	void FormBasic::SettingsButtons::setSystemSettings( QPushButton * p_ ) noexcept
	{
		_uiObjects._buttonSystemSettings = p_;
		_connectSettingsSystem();
	}

	void FormBasic::SettingsButtons::_disconnectAll( UiObjects & p_uiObjects ) noexcept
	{
		if ( p_uiObjects._buttonMinimizationSettings )
			p_uiObjects._buttonMinimizationSettings->disconnect( p_uiObjects._buttonMinimizationSettings );
		if ( p_uiObjects._buttonEquilibrationNvtSettings )
			p_uiObjects._buttonEquilibrationNvtSettings->disconnect( p_uiObjects._buttonEquilibrationNvtSettings );
		if ( p_uiObjects._buttonEquilibrationNptSettings )
			p_uiObjects._buttonEquilibrationNptSettings->disconnect( p_uiObjects._buttonEquilibrationNptSettings );
		if ( p_uiObjects._buttonProductionSettings )
			p_uiObjects._buttonProductionSettings->disconnect( p_uiObjects._buttonProductionSettings );
		if ( p_uiObjects._buttonSystemSettings )
			p_uiObjects._buttonSystemSettings->disconnect( p_uiObjects._buttonSystemSettings );
	}

	void FormBasic::SettingsButtons::_connectSettingsMinimization() noexcept
	{
		QPushButton::connect(
			_uiObjects._buttonMinimizationSettings,
			&QPushButton::clicked,
			[ & ]() { this->_openSettingsMinimization(); }
		);
	}

	void FormBasic::SettingsButtons::_connectSettingsNvt() noexcept
	{
		QPushButton::connect(
			_uiObjects._buttonEquilibrationNvtSettings, &QPushButton::clicked, [ & ]() { this->_openSettingsNvt(); }
		);
	}

	void FormBasic::SettingsButtons::_connectSettingsNpt() noexcept
	{
		QPushButton::connect(
			_uiObjects._buttonEquilibrationNptSettings, &QPushButton::clicked, [ & ]() { this->_openSettingsNpt(); }
		);
	}

	void FormBasic::SettingsButtons::_connectSettingsProd() noexcept
	{
		QPushButton::connect(
			_uiObjects._buttonProductionSettings, &QPushButton::clicked, [ & ]() { this->_openSettingsProd(); }
		);
	}

	void FormBasic::SettingsButtons::_connectSettingsSystem() noexcept
	{
		QPushButton::connect(
			_uiObjects._buttonSystemSettings, &QPushButton::clicked, [ & ]() { this->_openSettingsSystem(); }
		);
	}

	void FormBasic::SettingsButtons::_openSettingsMinimization() noexcept
	{
		_settingsDialog.emplace(
			_uiObjects._buttonMinimizationSettings,
			"Minimization Settings",
			&parameters.minimization,
			_mdEngineFieldPlacerCallback( E_FIELD_SECTION::minimization )
		);
	}

	void FormBasic::SettingsButtons::_openSettingsNvt() noexcept
	{
		_settingsDialog.emplace(
			_uiObjects._buttonEquilibrationNvtSettings,
			"Nvt Equilibration Settings",
			&parameters.nvt,
			_mdEngineFieldPlacerCallback( E_FIELD_SECTION::equilibrationNvt )
		);
	}

	void FormBasic::SettingsButtons::_openSettingsNpt() noexcept
	{
		_settingsDialog.emplace(
			_uiObjects._buttonEquilibrationNptSettings,
			"Npt Equilibration Settings",
			&parameters.npt,
			_mdEngineFieldPlacerCallback( E_FIELD_SECTION::equilibrationNpt )
		);
	}

	void FormBasic::SettingsButtons::_openSettingsProd() noexcept
	{
		_settingsDialog.emplace(
			_uiObjects._buttonProductionSettings,
			"Production run Settings",
			&parameters.prod,
			_mdEngineFieldPlacerCallback( E_FIELD_SECTION::production )
		);
	}

	namespace
	{
		class ApplyVisitorBasic
		{
		  public:
			ApplyVisitorBasic( std::function<void()> p_callback ) : _callback( std::move( p_callback ) ) {}
			void applied() noexcept { _callback(); }

		  private:
			std::function<void()> _callback;
		};
	} // namespace

	void FormBasic::SettingsButtons::_openSettingsSystem() noexcept
	{
		_settingsDialog.emplace(
			_uiObjects._buttonSystemSettings,
			"System Settings",
			&parameters.system,
			_mdEngineFieldPlacerCallback( E_FIELD_SECTION::system ),
			lastFormData,
			ApplyVisitorBasic( [ & ]() { this->inputChecker.checkInputs( this->parameters, []( auto ) {} ); } )
		);
	}

	void FormBasic::SettingsButtons::_systemSettingsApplied() noexcept {}

} // namespace VTX::Tool::Mdprep::ui
