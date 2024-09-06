#include <qboxlayout.h>
#include <qcombobox.h>
#include <qdialog.h>
#include <qformlayout.h>
#include <qpushbutton.h>
#include <qspinbox.h>
//
#include <ui/qt/util.hpp>
//
#include "tool/mdprep/gateway/form_data.hpp"
#include "tool/mdprep/ui/md_engine_specific_field_placer.hpp"
#include "tool/mdprep/ui/shared.hpp"
//
#include "tool/mdprep/ui/form_basic/settings_dialog.hpp"

namespace VTX::Tool::Mdprep::ui::form_basic
{
	using namespace VTX::Tool::Mdprep::Gateway;

	SettingsDialog::SettingsDialog(
		QWidget *												p_parent,
		const char *											p_title,
		SupportedData											p_dataFields,
		MdEngineSpecificFieldPlacer								p_engineFieldPlacer,
		std::optional<Gateway::EngineSpecificCommonInformation> p_engineInfo,
		ApplyVisitor											p_applyVisitor
	) :
		_data( std::move( p_dataFields ) ), _engineFieldPlacer( std::move( p_engineFieldPlacer ) ),
		_uiObjects( UiData { .engineInfo = std::move( p_engineInfo ) } ), _applyVisitor( std::move( p_applyVisitor ) )
	{
		_setupUi( p_parent, p_title );
		_setupSlots();
	}
	SettingsDialog::~SettingsDialog() noexcept
	{
		if ( _uiObjects.container )
			_uiObjects.container->close();
	}

	namespace
	{
		template<typename UiData>
		void setupUi( QVBoxLayout * p_qMainLayout, UiData & p_uiObjects, const MdParametersStepData * p_data )
		{
			QWidget *	  label	  = nullptr;
			QFormLayout * qLayout = new QFormLayout;
			p_qMainLayout->addLayout( qLayout );

			VTX::Tool::Mdprep::ui::get( E_COMMON_FIELDS_DOUBLE::dt, label, p_uiObjects.fieldDt );
			qLayout->addRow( label, p_uiObjects.fieldDt );

			p_uiObjects.fieldDt->setValue( p_data->dt );

			VTX::Tool::Mdprep::ui::get(
				E_COMMON_FIELDS_UINT64::coordinateSaveInterval, label, p_uiObjects.fieldSaveInterval
			);
			qLayout->addRow( label, p_uiObjects.fieldSaveInterval );
			p_uiObjects.fieldSaveInterval->setText( QString::number( p_data->nsteps ) );
		}
		template<typename UiData>
		void setupUi( QVBoxLayout * p_qMainLayout, UiData & p_uiObjects, const MinimizationData * p_data )
		{
			QWidget *	  label	  = nullptr;
			QFormLayout * qLayout = new QFormLayout;
			p_qMainLayout->addLayout( qLayout );

			VTX::Tool::Mdprep::ui::get(
				E_COMMON_FIELDS_DOUBLE::minimizationEnergyBreakpoint, label, p_uiObjects.fieldMinEnergyBreakpoint
			);
			qLayout->addRow( label, p_uiObjects.fieldMinEnergyBreakpoint );
			p_uiObjects.fieldMinEnergyBreakpoint->setValue( p_data->energyBreakpoint );

			VTX::Tool::Mdprep::ui::get(
				E_COMMON_FIELDS_DOUBLE::minimizationStepSize, label, p_uiObjects.fieldMinStepSize
			);
			qLayout->addRow( label, p_uiObjects.fieldMinStepSize );
			p_uiObjects.fieldMinStepSize->setValue( p_data->stepSize );
		}
		template<typename UiData>
		void setupUi( QVBoxLayout * p_qMainLayout, UiData & p_uiObjects, const SystemData * p_data )
		{
			QFormLayout * qLayout = new QFormLayout;
			p_qMainLayout->addLayout( qLayout );
			QWidget * w = nullptr;
			get( E_COMMON_FIELDS_COMBOBOX ::bioForcefield, w, p_uiObjects.fieldForceFieldBio );
			for ( auto & it_bioFf : p_uiObjects.engineInfo->bioForcefieldCollection )
			{
				p_uiObjects.fieldForceFieldBio->addItem( QString::fromStdString( it_bioFf ) );
				if ( it_bioFf == p_data->forcefieldBio )
					p_uiObjects.fieldForceFieldBio->setCurrentIndex( p_uiObjects.fieldForceFieldBio->count() - 1 );
			}
			qLayout->addRow( w, p_uiObjects.fieldForceFieldBio );

			get( E_COMMON_FIELDS_COMBOBOX::chemForcefield, w, p_uiObjects.fieldForceFieldChem );
			p_uiObjects.fieldForceFieldChem->addItem( "Not supported by VTX yet" );
			qLayout->addRow( w, p_uiObjects.fieldForceFieldChem );

			get( E_COMMON_FIELDS_COMBOBOX::boxShape, w, p_uiObjects.fieldBoxShape );
			for ( auto & it_shapeStr : p_uiObjects.engineInfo->boxShapeCollection )
			{
				p_uiObjects.fieldBoxShape->addItem( QString::fromStdString( it_shapeStr ) );
			}
			if ( p_data->boxShapeIdx < static_cast<size_t>( p_uiObjects.fieldBoxShape->count() ) )
				p_uiObjects.fieldBoxShape->setCurrentIndex( static_cast<int>( p_data->boxShapeIdx ) );
			qLayout->addRow( w, p_uiObjects.fieldBoxShape );

			get( E_COMMON_FIELDS_COMBOBOX ::waterModel, w, p_uiObjects.fieldWaterModel );
			for ( auto & it_waterModelsStr : p_uiObjects.engineInfo->waterModels )
			{
				p_uiObjects.fieldWaterModel->addItem( QString::fromStdString( it_waterModelsStr ) );
				if ( it_waterModelsStr == p_data->waterModel )
					p_uiObjects.fieldWaterModel->setCurrentIndex( p_uiObjects.fieldWaterModel->count() - 1 );
			}
			qLayout->addRow( w, p_uiObjects.fieldWaterModel );
		}
	} // namespace

	void SettingsDialog::_setupUi( QWidget * p_parent, const char * p_title )
	{
		_uiObjects.container = new QDialog( p_parent );
		_uiObjects.container->setWindowTitle( p_title );
		QVBoxLayout * qMainLayout = new QVBoxLayout( _uiObjects.container );

		// The variant part
		std::visit( [ & ]( auto p_ ) { setupUi( qMainLayout, _uiObjects, p_ ); }, _data );

		QFormLayout * qEngineSpecificLayout = new QFormLayout;
		qMainLayout->addLayout( qEngineSpecificLayout );

		_engineFieldPlacer.placeFields( qEngineSpecificLayout );

		if ( std::holds_alternative<SystemData *>( _data ) && _uiObjects.engineInfo.has_value() )
		{
			this->_engineFieldPlacer.update( EngineSpecificCommonInformationFieldUpdate {
				.field	  = E_ENGINE_SPECIFIC_COMMON_INFORMATION_FIELD ::boxShape,
				.newValue = std::get<SystemData *>( this->_data )->boxShapeIdx } );
			QObject::connect(
				_uiObjects.fieldBoxShape,
				&QComboBox::currentTextChanged,
				[ & ]
				{
					this->_engineFieldPlacer.update( EngineSpecificCommonInformationFieldUpdate {
						.field	  = E_ENGINE_SPECIFIC_COMMON_INFORMATION_FIELD ::boxShape,
						.newValue = static_cast<size_t>( _uiObjects.fieldBoxShape->currentIndex() ) } );
				}
			);
		}

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
	void SettingsDialog::_setupSlots() noexcept
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
			p_data->waterModel	  = p_uiObjects.fieldWaterModel->currentText().toStdString();
		}
	} // namespace
	void SettingsDialog::_applyChanges() noexcept
	{
		std::visit( [ & ]( auto p_ ) { applyChange( _uiObjects, p_ ); }, _data );
		_applyVisitor.applied();
		_engineFieldPlacer.apply();
		_uiObjects.container->close();
	}
	void SettingsDialog::_cancelChanges() noexcept { _uiObjects.container->close(); }
} // namespace VTX::Tool::Mdprep::ui::form_basic
