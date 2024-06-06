#include <qboxlayout.h>
#include <qdialog.h>
#include <qformlayout.h>
#include <qpushbutton.h>
#include <qspinbox.h>
//
#include <ui/qt/util.hpp>
//
#include "tools/mdprep/gateway/form_data.hpp"
#include "tools/mdprep/ui/md_engine_specific_field_placer.hpp"
//
#include "tools/mdprep/ui/basic_form_settings_dialog.hpp"

namespace VTX::Tool::Mdprep::ui
{
	using namespace VTX::Tool::Mdprep::Gateway;

	BasicFormSettingsDialog::BasicFormSettingsDialog(
		QWidget *					p_parent,
		const char *				p_title,
		MdParametersStepData &		p_dataFields,
		MdEngineSpecificFieldPlacer p_engineFieldPlacer
	) :
		_dataStep( &p_dataFields ),
		_engineFieldPlacer( std::move( p_engineFieldPlacer ) )
	{
		_setupUi( p_parent, p_title );
		_setupSlots();
	}
	BasicFormSettingsDialog::BasicFormSettingsDialog(
		QWidget *					p_parent,
		const char *				p_title,
		MinimizationData &			p_dataMinimization,
		MdEngineSpecificFieldPlacer p_engineFieldPlacer
	) :
		_dataMinimization( &p_dataMinimization ),
		_engineFieldPlacer( std::move( p_engineFieldPlacer ) )
	{
		_setupUi( p_parent, p_title );
		_setupSlots();
	}
	BasicFormSettingsDialog::~BasicFormSettingsDialog() noexcept
	{
		if ( _uiObjects.container )
			_uiObjects.container->close();
	}

	namespace
	{
		template<typename UiObjects>
		void setupUi( QVBoxLayout * p_qMainLayout, UiObjects & p_uiObjects, const MdParametersStepData * p_data )
		{
			QFormLayout * qLayout = new QFormLayout;
			p_qMainLayout->addLayout( qLayout );

			p_uiObjects.fieldDt = new QDoubleSpinBox;
			p_uiObjects.fieldDt->setMinimum( 1e-3 );
			p_uiObjects.fieldDt->setMaximum( 1e-1 );
			p_uiObjects.fieldDt->setSingleStep( 1e-3 );
			p_uiObjects.fieldDt->setDecimals( 3 );
			qLayout->addRow(
				VTX::UI::QT::Util::LabelWithHelper(
					"Time step interval (ns)",
					"Quantity of time passed between each steps. <br><br>2fs (0.002 ns) is a good default as it allows "
					"C-H "
					"bonds length to "
					"remain constant throughout the simulation.",
					VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
				),
				p_uiObjects.fieldDt
			);
			p_uiObjects.fieldDt->setValue( p_data->dt );

			p_uiObjects.fieldSaveInterval = VTX::UI::QT::Util::addUInt64Field(
				qLayout, "Save Interval", "Number of steps between each system save."
			);
			p_uiObjects.fieldSaveInterval->setText( QString::number( p_data->nsteps ) );
		}
		template<typename UiObjects>
		void setupUi( QVBoxLayout * p_qMainLayout, UiObjects & p_uiObjects, const MinimizationData * p_data )
		{
			QFormLayout * qLayout = new QFormLayout;
			p_qMainLayout->addLayout( qLayout );

			p_uiObjects.fieldMinEnergyBreakpoint = new QDoubleSpinBox;
			p_uiObjects.fieldMinEnergyBreakpoint->setMinimum( 1e-1 );
			p_uiObjects.fieldMinEnergyBreakpoint->setMaximum( 1e6 );
			p_uiObjects.fieldMinEnergyBreakpoint->setSingleStep( 1e2 );
			p_uiObjects.fieldMinEnergyBreakpoint->setDecimals( 1 );
			qLayout->addRow(
				VTX::UI::QT::Util::LabelWithHelper(
					"Early termination threshold",
					"Energy delta between steps from which the minimization is considered <i>converged</i>.",
					VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
				),
				p_uiObjects.fieldMinEnergyBreakpoint
			);
			p_uiObjects.fieldMinEnergyBreakpoint->setValue( p_data->energyBreakpoint );
			p_uiObjects.fieldMinStepSize = new QDoubleSpinBox;
			p_uiObjects.fieldMinStepSize->setMinimum( 1e-4 );
			p_uiObjects.fieldMinStepSize->setMaximum( 1e1 );
			p_uiObjects.fieldMinStepSize->setSingleStep( 1e-3 );
			p_uiObjects.fieldMinStepSize->setDecimals( 4 );
			qLayout->addRow(
				VTX::UI::QT::Util::LabelWithHelper(
					"Minimization step size",
					"Defines how big the delta between each steps should be.<br>A bigger step size means quicker "
					"minimization but with reduced result stability.",
					VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
				),
				p_uiObjects.fieldMinStepSize
			);
			p_uiObjects.fieldMinStepSize->setValue( p_data->stepSize );
		}
	} // namespace

	void BasicFormSettingsDialog::_setupUi( QWidget * p_parent, const char * p_title )
	{
		_uiObjects.container = new QDialog( p_parent );
		_uiObjects.container->setWindowTitle( p_title );
		QVBoxLayout * qMainLayout = new QVBoxLayout( _uiObjects.container );
		if ( _dataStep )
			setupUi( qMainLayout, _uiObjects, _dataStep );
		else if ( _dataMinimization )
			setupUi( qMainLayout, _uiObjects, _dataMinimization );

		QFormLayout * qEngineSpecificLayout = new QFormLayout;
		qMainLayout->addLayout( qEngineSpecificLayout );
		_engineFieldPlacer.placeFields( qEngineSpecificLayout );

		qMainLayout->addSpacerItem( new QSpacerItem( 0, 20 ) );
		QHBoxLayout * qButtonLayout = new QHBoxLayout;
		qMainLayout->addLayout( qButtonLayout );
		qButtonLayout->addStretch( 1 );
		_uiObjects.buttonApply	= new QPushButton( "Apply" );
		_uiObjects.buttonCancel = new QPushButton( "Cancel" );
		qButtonLayout->addWidget( _uiObjects.buttonApply );
		qButtonLayout->addWidget( _uiObjects.buttonCancel );
		_uiObjects.container->open();
	}
	void BasicFormSettingsDialog::_setupSlots() noexcept
	{
		QPushButton::connect( _uiObjects.buttonApply, &QPushButton::clicked, [ & ]() { this->_applyChanges(); } );
		QPushButton::connect( _uiObjects.buttonCancel, &QPushButton::clicked, [ & ]() { this->_cancelChanges(); } );
	}
	void BasicFormSettingsDialog::_applyChanges() noexcept
	{
		if ( _dataStep )
		{
			_dataStep->dt	  = _uiObjects.fieldDt->value();
			_dataStep->nsteps = _uiObjects.fieldSaveInterval->text().toUInt();
		}
		else if ( _dataMinimization )
		{
			_dataMinimization->energyBreakpoint = _uiObjects.fieldMinEnergyBreakpoint->value();
			_dataMinimization->stepSize			= _uiObjects.fieldMinStepSize->value();
		}
		_engineFieldPlacer.apply();
		_uiObjects.container->close();
	}
	void BasicFormSettingsDialog::_cancelChanges() noexcept { _uiObjects.container->close(); }
} // namespace VTX::Tool::Mdprep::ui
