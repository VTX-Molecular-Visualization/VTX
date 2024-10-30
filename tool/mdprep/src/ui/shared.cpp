#include <qgridlayout.h>
#include <qlineedit.h>
#include <qspinbox.h>
//
#include <app/vtx_app.hpp>
#include <ui/qt/util.hpp>
#include <ui/qt/validator.hpp>
#include <util/sentry.hpp>
//
#include "tool/mdprep/gateway/form_data.hpp"
#include "tool/mdprep/gateway/shared.hpp"
//
#include "tool/mdprep/ui/shared.hpp"

namespace VTX::Tool::Mdprep::ui
{
	void get( const E_COMMON_FIELDS_COMBOBOX & p_enum, QWidget *& p_label, QComboBox *& p_field ) noexcept
	{
		switch ( p_enum )
		{
		case E_COMMON_FIELDS_COMBOBOX::bioForcefield:
			p_label = VTX::UI::QT::Util::LabelWithHelper(
				"Force Field for biological systems",
				"Sets of values used by the engine to compute biological systems motion.",
				VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			);
			break;
		case E_COMMON_FIELDS_COMBOBOX::chemForcefield:
			p_label = VTX::UI::QT::Util::LabelWithHelper(
				"Force Field for Small Organic systems",
				"Sets of values used by the engine to compute small organic systems motion.",
				VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			);
			break;
		case E_COMMON_FIELDS_COMBOBOX::boxShape:
			p_label = VTX::UI::QT::Util::LabelWithHelper(
				"System Periodic Box shape",
				"Defines the shape of the box used to set up periodic boundary conditions. The rule of thumbs it "
				"to choose a shape that is the closest match to the shape of the elements of interest.",
				VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			);
			break;
		case E_COMMON_FIELDS_COMBOBOX::waterModel:
			p_label = VTX::UI::QT::Util::LabelWithHelper(
				"Water model",
				"States how the water should be modelised. More complex water model allow more realistic water "
				"behavior, but with a huge computational cost.",
				VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			);
			break;
		default: break;
		}
		p_field = new QComboBox;
	}
	void get( const E_COMMON_FIELDS_DOUBLE & p_enum, QWidget *& p_label, QDoubleSpinBox *& field ) noexcept
	{
		switch ( p_enum )
		{
		case E_COMMON_FIELDS_DOUBLE::minimizationStepSize:
			p_label = VTX::UI::QT::Util::LabelWithHelper(
				"Minimization step size",
				"Defines how big the delta between each steps should be.<br>A bigger step size means quicker "
				"minimization but with reduced result stability.",
				VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			);
			field = new QDoubleSpinBox;
			field->setMinimum( 1e-4 );
			field->setMaximum( 1e1 );
			field->setSingleStep( 1e-3 );
			field->setDecimals( 4 );
			break;
		case E_COMMON_FIELDS_DOUBLE::minimizationEnergyBreakpoint:
			p_label = VTX::UI::QT::Util::LabelWithHelper(
				"Early termination threshold",
				"Energy delta between steps from which the minimization is considered <i>converged</i>.",
				VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			);
			field = new QDoubleSpinBox;
			field->setMinimum( 1e-1 );
			field->setMaximum( 1e6 );
			field->setSingleStep( 1e2 );
			field->setDecimals( 1 );
			break;
		case E_COMMON_FIELDS_DOUBLE::dt:

			p_label = VTX::UI::QT::Util::LabelWithHelper(
				"Time step interval (ns)",
				"Quantity of time passed between each steps. <br><br>2fs (0.002 ns) is a good default as it allows "
				"C-H bonds length to remain constant throughout the simulation.",
				VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			);
			field = new QDoubleSpinBox;
			field->setMinimum( 1e-3 );
			field->setMaximum( 1. );
			field->setSingleStep( 1e-3 );
			field->setDecimals( 3 );

			break;

		default: break;
		}
	}
	void get( const E_COMMON_FIELDS_UINT64 & p_enum, QWidget *& p_label, QLineEdit *& p_field ) noexcept
	{
		switch ( p_enum )
		{
		case E_COMMON_FIELDS_UINT64::stepNumberMinimization:

			p_label = VTX::UI::QT::Util::LabelWithHelper(
				"Number of Minimization Steps",
				"Dictates how long the minimization can be. Bigger systems need higher number of "
				"steps.<br>Minimization shall still end earlier if the energy plateau is reached.",
				VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			);
			break;
		case E_COMMON_FIELDS_UINT64::stepNumber:

			p_label = VTX::UI::QT::Util::LabelWithHelper(
				"Number of computed steps",
				"Defines how long this part of the simulation will be.",
				VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			);
			break;
		case E_COMMON_FIELDS_UINT64::coordinateSaveInterval:
			p_label = VTX::UI::QT::Util::LabelWithHelper(
				"Coordinate Save Interval",
				"Number of steps between each snapshot of system coordinate. The smaller the number, the smoother the "
				"output will look but with larger file. A value of 0 will instruct the engine not to save is at all.",
				VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			);

		default: break;
		}
		p_field = new QLineEdit;
		p_field->setValidator( new VTX::UI::QT::QUInt64Validator( p_field ) );
	}
	IonicFields::IonicFields( QVBoxLayout * p_target, const Gateway::SystemData & p_defaults )
	{
		_setupUi( p_target );
		_loadValues( p_defaults );
	}
	void IonicFields::get( Gateway::SystemData & p_out ) const noexcept
	{
		if ( _isDefault() )
			return;
		p_out.anionCharge  = static_cast<int8_t>( _uiObjects.anionCharge->currentText().toInt() );
		p_out.cationCharge = static_cast<int8_t>( _uiObjects.cationCharge->currentText().toInt() );
		p_out.anionName	   = _uiObjects.anionName->text().toStdString();
		p_out.cationName   = _uiObjects.cationName->text().toStdString();
	}

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
	bool IonicFields::_isDefault() const noexcept { return _uiObjects.container == nullptr; }
	void IonicFields::_setupUi( QVBoxLayout * p_target ) noexcept
	{
		_uiObjects.container = new QWidget;
		p_target->addWidget( _uiObjects.container );
		_toDelete = _uiObjects.container;

		QGridLayout * qLayoutIons = new QGridLayout;
		qLayoutIons->setContentsMargins( { 0, 0, 0, 0 } );
		_uiObjects.container->setLayout( qLayoutIons );

		qLayoutIons->addWidget(
			UI::QT::Util::LabelWithHelper(
				"Anion Symbol",
				"Chemical symbol of the anion used to balance system charges and/or to buffer the system.",
				UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			),
			0,
			0
		);
		_uiObjects.anionName = new QLineEdit;
		_uiObjects.anionName->setValidator( new QChemicalSymbolValidator( _uiObjects.anionName ) );
		qLayoutIons->addWidget( _uiObjects.anionName, 0, 1 );

		qLayoutIons->addWidget(
			UI::QT::Util::LabelWithHelper(
				"Cation Symbol",
				"Chemical symbol of the cation used to balance system charges and/or to buffer the system.",
				UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			),
			1,
			0
		);
		_uiObjects.cationName = new QLineEdit;
		_uiObjects.cationName->setValidator( new QChemicalSymbolValidator( _uiObjects.cationName ) );
		qLayoutIons->addWidget( _uiObjects.cationName, 1, 1 );

		qLayoutIons->addWidget(
			UI::QT::Util::LabelWithHelper(
				"Anion Charge",
				"Formal Charge of the forementionned Anion.",
				UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			),
			0,
			2
		);

		_uiObjects.anionCharge	= new QComboBox;
		_uiObjects.cationCharge = new QComboBox;
		for ( auto & it_chrgStr : g_IonicChargeValues )
		{
			if ( std::stol( it_chrgStr.data() ) > 0 )
				_uiObjects.cationCharge->addItem( QString( it_chrgStr.data() ) );

			if ( std::stol( it_chrgStr.data() ) < 0 )
				_uiObjects.anionCharge->addItem( QString( it_chrgStr.data() ) );
		}

		qLayoutIons->addWidget( _uiObjects.anionCharge, 0, 3 );

		qLayoutIons->addWidget(
			UI::QT::Util::LabelWithHelper(
				"Cation Charge",
				"Formal Charge of the forementionned Cation.",
				UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
			),
			1,
			2
		);
		qLayoutIons->addWidget( _uiObjects.cationCharge, 1, 3 );
	}
	void IonicFields::_loadValues( const Gateway::SystemData & p_defaults ) noexcept
	{
		_uiObjects.anionName->setText( QString::fromStdString( p_defaults.anionName ) );
		_uiObjects.cationName->setText( QString::fromStdString( p_defaults.cationName ) );

		for ( size_t it_idx = 0; it_idx < g_IonicChargeValues.size(); it_idx++ )
		{
			auto & it_chrgStr = g_IonicChargeValues[ it_idx ];
			if ( std::stol( it_chrgStr.data() ) == p_defaults.anionCharge )
				_uiObjects.anionCharge->setCurrentText( g_IonicChargeValues[ it_idx ].data() );
			if ( std::stol( it_chrgStr.data() ) == p_defaults.cationCharge )
				_uiObjects.cationCharge->setCurrentText( g_IonicChargeValues[ it_idx ].data() );
		}
	}

} // namespace VTX::Tool::Mdprep::ui
