#include <qboxlayout.h>
#include <qcombobox.h>
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
		QWidget *												p_parent,
		const char *											p_title,
		SupportedData											p_dataFields,
		MdEngineSpecificFieldPlacer								p_engineFieldPlacer,
		std::optional<Gateway::EngineSpecificCommonInformation> p_engineInfo
	) :
		_data( std::move( p_dataFields ) ), _engineFieldPlacer( std::move( p_engineFieldPlacer ) ),
		_uiObjects( UiData { .engineInfo = std::move( p_engineInfo ) } )
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
		template<typename UiData>
		void setupUi( QVBoxLayout * p_qMainLayout, UiData & p_uiObjects, const MdParametersStepData * p_data )
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
		template<typename UiData>
		void setupUi( QVBoxLayout * p_qMainLayout, UiData & p_uiObjects, const MinimizationData * p_data )
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
		template<typename UiData>
		void setupUi( QVBoxLayout * p_qMainLayout, UiData & p_uiObjects, const SystemData * p_data )
		{
			QFormLayout * qLayout = new QFormLayout;
			p_qMainLayout->addLayout( qLayout );
			p_uiObjects.fieldForceFieldBio = new QComboBox;
			for ( auto & it_bioFf : p_uiObjects.engineInfo->bioForcefieldCollection )
			{
				p_uiObjects.fieldForceFieldBio->addItem( QString::fromStdString( it_bioFf ) );
				if ( it_bioFf == p_data->forcefieldBio )
					p_uiObjects.fieldForceFieldBio->setCurrentIndex( p_uiObjects.fieldForceFieldBio->count() - 1 );
			}
			qLayout->addRow(
				VTX::UI::QT::Util::LabelWithHelper(
					"Force Field for biological molecules",
					"Sets of values used by the engine to compute biological molecules motion.",
					VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
				),
				p_uiObjects.fieldForceFieldBio
			);
			p_uiObjects.fieldForceFieldChem = new QComboBox;
			p_uiObjects.fieldForceFieldChem->addItem( "Not supported by VTX yet" );
			qLayout->addRow(
				VTX::UI::QT::Util::LabelWithHelper(
					"Force Field for Small Organic molecules",
					"Sets of values used by the engine to compute small organic molecules motion.",
					VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
				),
				p_uiObjects.fieldForceFieldChem
			);
			p_uiObjects.fieldBoxShape = new QComboBox;
			for ( auto & it_shapeStr : p_uiObjects.engineInfo->boxShapeCollection )
			{
				p_uiObjects.fieldBoxShape->addItem( QString::fromStdString( it_shapeStr ) );
			}
			if ( p_data->boxShapeIdx < p_uiObjects.fieldBoxShape->count() )
				p_uiObjects.fieldBoxShape->setCurrentIndex( static_cast<int>( p_data->boxShapeIdx ) );
			qLayout->addRow(
				VTX::UI::QT::Util::LabelWithHelper(
					"System Periodic Box shape",
					"Defines the shape of the box used to set up periodic boundary conditions. The rule of thumbs it "
					"to choose a shape that is the closest match to the shape of the elements of interest.",
					VTX::UI::QT::Util::LabelWithHelper::E_QUESTIONMARK_POSITION::left
				),
				p_uiObjects.fieldBoxShape
			);

			/*
for ( int it_idx = 0; _uiObjects._fieldForceFieldBio->count(); it_idx++ )
if ( _uiObjects._fieldForceFieldBio->itemText( it_idx ).toStdString() == p_defaults.forcefieldBio )
{
_uiObjects._fieldForceFieldBio->setCurrentIndex( it_idx );
break;
}
*/

			/*
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
			*/
		}
	} // namespace

	void BasicFormSettingsDialog::_setupUi( QWidget * p_parent, const char * p_title )
	{
		_uiObjects.container = new QDialog( p_parent );
		_uiObjects.container->setWindowTitle( p_title );
		QVBoxLayout * qMainLayout = new QVBoxLayout( _uiObjects.container );
		std::visit( [ & ]( auto p_ ) { setupUi( qMainLayout, _uiObjects, p_ ); }, _data );
		// setupUi( qMainLayout, _uiObjects, _data );

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
	namespace
	{
		template<typename UiData>
		void applyChange( const UiData & p_uiObjects, MinimizationData * p_data )
		{
			p_data->energyBreakpoint = p_uiObjects.fieldMinEnergyBreakpoint->value();
			p_data->stepSize		 = p_uiObjects.fieldMinStepSize->value();
		}
		template<typename UiData>
		void applyChange( const UiData & p_uiObjects, MdParametersStepData * p_data )
		{
			p_data->dt	   = p_uiObjects.fieldDt->value();
			p_data->nsteps = p_uiObjects.fieldSaveInterval->text().toUInt();
		}
		template<typename UiData>
		void applyChange( const UiData & p_uiObjects, SystemData * p_data )
		{
			p_data->forcefieldBio = p_uiObjects.fieldForceFieldBio->currentText().toStdString();
			p_data->boxShapeIdx	  = p_uiObjects.fieldBoxShape->currentIndex();

			// p_data->dt	   = p_uiObjects.fieldDt->value();
			// p_data->nsteps = p_uiObjects.fieldSaveInterval->text().toUInt();
		}
	} // namespace
	void BasicFormSettingsDialog::_applyChanges() noexcept
	{
		std::visit( [ & ]( auto p_ ) { applyChange( _uiObjects, p_ ); }, _data );
		_engineFieldPlacer.apply();
		_uiObjects.container->close();
	}
	void BasicFormSettingsDialog::_cancelChanges() noexcept { _uiObjects.container->close(); }
} // namespace VTX::Tool::Mdprep::ui
