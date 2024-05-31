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
#include "tools/mdprep/ui/form_data.hpp"
#include "util/logger.hpp"
#include <ui/qt/util.hpp>
#include <ui/qt/validator.hpp>
#include <ui/qt/widget_factory.hpp>
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
	void MdBasicParamForm::setupUi( QWidget * p_container, const MdParameters & p_defaults )
	{
		_createAndPlaceUiItems( p_container );
		_loadValues( p_defaults );
		_eventCallbacks.registerUpdate( [ & ]() { this->_sendUpdateSignal(); } );
		_eventCallbacks.registerSettingPushedNvt( [ & ]() { this->_openSettingsNvt(); } );
		_eventCallbacks.registerSettingPushedNpt( [ & ]() { this->_openSettingsNpt(); } );
		_eventCallbacks.registerSettingPushedProd( [ & ]() { this->_openSettingsProd(); } );
		_eventToggler.toggle( _uiObjects );
	}
	void MdBasicParamForm::update( const EngineSpecificCommonFormData & p_data ) noexcept
	{
		_uiObjects._fieldForceFieldBio->clear();
		for ( auto & it_forcefieldStr : p_data.bioForcefieldCollection )
		{
			_uiObjects._fieldForceFieldBio->addItem( QString::fromStdString( it_forcefieldStr ) );
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
			*p_out_buttonSettings = new QPushButton;
			( *p_out_buttonSettings )->setFlat( true );
			( *p_out_buttonSettings )->setCursor( Qt::CursorShape::PointingHandCursor );
			( *p_out_buttonSettings )->setIcon( QIcon( ":/sprite/settings_icon.png" ) );
			qLayoutRightCell->addWidget( *p_out_buttonSettings );
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
		_eventToggler.toggle( _uiObjects );

		_lastSeenAdvancedDataSample = p_data;
		figureBestValueUnitRelation(
			p_data.dtNvt,
			p_data.stepNumberNvt,
			_uiObjects._fieldDurationValueEquilibrationNvt,
			_uiObjects._fieldDurationUnitEquilibrationNvt
		);
		figureBestValueUnitRelation(
			p_data.dtNpt,
			p_data.stepNumberNpt,
			_uiObjects._fieldDurationValueEquilibrationNpt,
			_uiObjects._fieldDurationUnitEquilibrationNpt
		);
		figureBestValueUnitRelation(
			p_data.dtProd,
			p_data.stepNumberProd,
			_uiObjects._fieldDurationValueProduction,
			_uiObjects._fieldDurationUnitProduction
		);
		_eventToggler.toggle( _uiObjects );
	}
	void MdBasicParamForm::subscribe( UpdateCallback p_ ) noexcept { _updateCallback = p_; }
	void MdBasicParamForm::_createAndPlaceUiItems( QWidget * p_container ) noexcept
	{
		QVBoxLayout * qMainLayout = new QVBoxLayout( p_container );

		VTX::UI::QT::Util::addLabeledHLineSeparator( qMainLayout, "Force Fields" );

		QFormLayout * qFormForceField = new QFormLayout;
		qMainLayout->addLayout( qFormForceField );

		_uiObjects._fieldForceFieldBio = new QComboBox;
		qFormForceField->addRow(
			VTX::UI::QT::Util::LabelWithHelper(
				"Force Field for biological molecules",
				"Sets of values used by the engine to compute biological molecules motion.",
				VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			)

				,
			_uiObjects._fieldForceFieldBio
		);
		QComboBox * fieldForceFieldChem = new QComboBox;
		fieldForceFieldChem->addItem( "Not supported by VTX yet" );
		qFormForceField->addRow(
			VTX::UI::QT::Util::LabelWithHelper(
				"Force Field for Small Organic molecules",
				"Sets of values used by the engine to compute small organic molecules motion.",
				VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			)
				.container,
			fieldForceFieldChem
		);

		qMainLayout->addSpacerItem( new QSpacerItem( 0, 10 ) );
		VTX::UI::QT::Util::addLabeledHLineSeparator( qMainLayout, "MD Duration" );

		QFormLayout * qFormSteps = new QFormLayout;
		qMainLayout->addLayout( qFormSteps );
		_uiObjects._fieldStepNumberMinimization = addUInt64Field(
			qFormSteps,
			"Number of Minimization Steps",
			"Dictates how long the minimization can be. Bigger systems need higher number of "
			"steps.<br>Minimization shall still end earlier if the energy plateau is reached."
		);
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
		VTX::UI::QT::Util::addLabeledHLineSeparator( qMainLayout, "Other Parameters" );

		QGridLayout * qLayoutIons = new QGridLayout;
		qMainLayout->addLayout( qLayoutIons );

		qLayoutIons->addWidget(
			UI::QT::Util::LabelWithHelper(
				"Anion Symbol",
				"Chemical symbol of the anion used to balance system charges and/or to buffer the system.",
				UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			),
			0,
			0
		);
		_uiObjects._fieldAnionName = new QLineEdit;
		_uiObjects._fieldAnionName->setValidator( new QChemicalSymbolValidator( _uiObjects._fieldAnionName ) );
		qLayoutIons->addWidget( _uiObjects._fieldAnionName, 0, 1 );

		qLayoutIons->addWidget(
			UI::QT::Util::LabelWithHelper(
				"Cation Symbol",
				"Chemical symbol of the cation used to balance system charges and/or to buffer the system.",
				UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			),
			1,
			0
		);
		_uiObjects._fieldCationName = new QLineEdit;
		_uiObjects._fieldCationName->setValidator( new QChemicalSymbolValidator( _uiObjects._fieldCationName ) );
		qLayoutIons->addWidget( _uiObjects._fieldCationName, 1, 1 );

		qLayoutIons->addWidget(
			UI::QT::Util::LabelWithHelper(
				"Anion Charge",
				"Formal Charge of the forementionned Anion.",
				UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			),
			0,
			2
		);

		_uiObjects._fieldAnionCharge  = new QComboBox;
		_uiObjects._fieldCationCharge = new QComboBox;
		for ( auto & it_chrgStr : g_IonicChargeValues )
		{
			if ( std::stol( it_chrgStr.data() ) > 0 )
				_uiObjects._fieldCationCharge->addItem( QString( it_chrgStr.data() ) );

			if ( std::stol( it_chrgStr.data() ) < 0 )
				_uiObjects._fieldAnionCharge->addItem( QString( it_chrgStr.data() ) );
		}

		qLayoutIons->addWidget( _uiObjects._fieldAnionCharge, 0, 3 );

		qLayoutIons->addWidget(
			UI::QT::Util::LabelWithHelper(
				"Cation Charge",
				"Formal Charge of the forementionned Cation.",
				UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			),
			1,
			2
		);
		qLayoutIons->addWidget( _uiObjects._fieldCationCharge, 1, 3 );

		_uiObjects._layoutFieldEngineSpecific = new QFormLayout;
		qMainLayout->addLayout( _uiObjects._layoutFieldEngineSpecific );

		qMainLayout->addStretch( 1 );
	}
	void MdBasicParamForm::_loadValues( const MdParameters & p_defaults ) noexcept
	{
		_parameters = p_defaults;
		for ( int it_idx = 0; _uiObjects._fieldForceFieldBio->count(); it_idx++ )
			if ( _uiObjects._fieldForceFieldBio->itemText( it_idx ).toStdString() == p_defaults.forcefieldBio )
			{
				_uiObjects._fieldForceFieldBio->setCurrentIndex( it_idx );
				break;
			}
		_uiObjects._fieldStepNumberMinimization->setText( QString::number( p_defaults.minimizationEmstep ) );
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

		_uiObjects._fieldAnionName->setText( QString::fromStdString( p_defaults.anionName ) );
		_uiObjects._fieldCationName->setText( QString::fromStdString( p_defaults.cationName ) );

		for ( int it_idx = 0; it_idx < g_IonicChargeValues.size(); it_idx++ )
		{
			auto & it_chrgStr = g_IonicChargeValues[ it_idx ];
			if ( std::stol( it_chrgStr.data() ) == p_defaults.anionCharge )
				_uiObjects._fieldAnionCharge->setCurrentText( g_IonicChargeValues[ it_idx ].data() );
			if ( std::stol( it_chrgStr.data() ) == p_defaults.cationCharge )
				_uiObjects._fieldCationCharge->setCurrentText( g_IonicChargeValues[ it_idx ].data() );
		}
	}
	void MdBasicParamForm::_sendUpdateSignal() noexcept
	{
		_eventToggler.toggle( _uiObjects );

		MdBasicDataSample dataSample;
		dataSample.durationEquilibrationNvt = _uiObjects._fieldDurationValueEquilibrationNvt->text().toInt();
		dataSample.unitEquilibrationNvt
			= static_cast<E_MD_DURATION_UNIT>( _uiObjects._fieldDurationUnitEquilibrationNvt->currentIndex() );
		dataSample.durationEquilibrationNpt = _uiObjects._fieldDurationValueEquilibrationNpt->text().toInt();
		dataSample.unitEquilibrationNpt
			= static_cast<E_MD_DURATION_UNIT>( _uiObjects._fieldDurationUnitEquilibrationNpt->currentIndex() );
		dataSample.durationProduction = _uiObjects._fieldDurationValueProduction->text().toInt();
		dataSample.unitProduction
			= static_cast<E_MD_DURATION_UNIT>( _uiObjects._fieldDurationUnitProduction->currentIndex() );
		_updateCallback( dataSample );

		_eventToggler.toggle( _uiObjects );
	}
	void		  MdBasicParamForm::_openSettingsNvt() noexcept {}
	void		  MdBasicParamForm::_openSettingsNpt() noexcept {}
	void		  MdBasicParamForm::_openSettingsProd() noexcept {}
	void		  MdBasicParamForm::get( MdParameters & p_in ) const noexcept { p_in = _parameters; }
	QFormLayout * MdBasicParamForm::layoutFieldsMdEngine() noexcept { return _uiObjects._layoutFieldEngineSpecific; }

	MdBasicParamForm::EventToggler::EventToggler( const EventCallbacks & p_ ) : _callbacks( &p_ ) {}
	void MdBasicParamForm::EventToggler::toggle( UiObjects & p_ui ) noexcept
	{
		if ( _eventsOn )
			_disconnect( p_ui );
		else
			_connect( p_ui );
		_eventsOn ^= true;
	}
	void MdBasicParamForm::EventToggler::_connect( UiObjects & p_uiObjects ) noexcept
	{
		QPushButton::connect(
			p_uiObjects._buttonEquilibrationNvtSettings, &QPushButton::clicked, _callbacks->signalSettingPushedNvt()
		);
		return; // Temporarily deactivates events for improvements purposes

		/*
		QLineEdit::connect(
			p_uiObjects._fieldDurationValueEquilibrationNvt, &QLineEdit::textChanged, _lambdaToConnect
		);
		QComboBox::connect(
			p_uiObjects._fieldDurationUnitEquilibrationNvt, &QComboBox::currentIndexChanged, _lambdaToConnect
		);
		QLineEdit::connect(
			p_uiObjects._fieldDurationValueEquilibrationNpt, &QLineEdit::textChanged, _lambdaToConnect
		);
		QComboBox::connect(
			p_uiObjects._fieldDurationUnitEquilibrationNpt, &QComboBox::currentIndexChanged, _lambdaToConnect
		);
		QLineEdit::connect( p_uiObjects._fieldDurationValueProduction, &QLineEdit::textChanged, _lambdaToConnect );
		QComboBox::connect(
			p_uiObjects._fieldDurationUnitProduction, &QComboBox::currentIndexChanged, _lambdaToConnect
		);
		*/
	}
	void MdBasicParamForm::EventToggler::_disconnect( UiObjects & p_uiObjects ) noexcept
	{
		p_uiObjects._buttonEquilibrationNvtSettings->disconnect();
		return; // Temporarily deactivates events for improvements purposes
		p_uiObjects._fieldDurationValueEquilibrationNvt->disconnect();
		p_uiObjects._fieldDurationUnitEquilibrationNvt->disconnect();
		p_uiObjects._fieldDurationValueEquilibrationNpt->disconnect();
		p_uiObjects._fieldDurationUnitEquilibrationNpt->disconnect();
		p_uiObjects._fieldDurationValueProduction->disconnect();
		p_uiObjects._fieldDurationUnitProduction->disconnect();
	}
	void MdBasicParamForm::EventCallbacks::registerUpdate( Callback p_ ) noexcept { _update = p_; }
	void MdBasicParamForm::EventCallbacks::registerSettingPushedNvt( Callback p_ ) noexcept { _pushedNvt = p_; }
	void MdBasicParamForm::EventCallbacks::registerSettingPushedNpt( Callback p_ ) noexcept { _pushedNpt = p_; }
	void MdBasicParamForm::EventCallbacks::registerSettingPushedProd( Callback p_ ) noexcept { _pushedProd = p_; }
	const MdBasicParamForm::EventCallbacks::Callback & MdBasicParamForm::EventCallbacks::signalUpdate() const noexcept
	{
		return _update;
	}
	const MdBasicParamForm::EventCallbacks::Callback & MdBasicParamForm::EventCallbacks::signalSettingPushedNvt(
	) const noexcept
	{
		return _pushedNvt;
	}
	const MdBasicParamForm::EventCallbacks::Callback & MdBasicParamForm::EventCallbacks::signalSettingPushedNpt(
	) const noexcept
	{
		return _pushedNpt;
	}
	const MdBasicParamForm::EventCallbacks::Callback & MdBasicParamForm::EventCallbacks::signalSettingPushedProd(
	) const noexcept
	{
		return _pushedProd;
	}

} // namespace VTX::Tool::Mdprep::ui
