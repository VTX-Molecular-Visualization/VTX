#include "tool/mdprep/ui/form_basic/settings_dialog.hpp"
#include "tool/mdprep/gateway/form_data.hpp"
#include "tool/mdprep/ui/md_engine_specific_field_placer.hpp"
#include "tool/mdprep/ui/shared.hpp"
#include <memory>
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qdialog.h>
#include <qformlayout.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <ui/qt/helper.hpp>
#include <ui/qt/util.hpp>
#include <utility>

namespace VTX::Tool::Mdprep::ui::form_basic
{
	using namespace VTX::Tool::Mdprep::Gateway;

	SettingsDialog::SettingsDialog( QWidget * const											p_parent,
									const char *											p_title,
									SupportedData											p_dataFields,
									MdEngineSpecificFieldPlacer								p_engineFieldPlacer,
									std::optional<Gateway::EngineSpecificCommonInformation> p_engineInfo,
									ApplyVisitor											p_applyVisitor ) :
		_data( p_dataFields ), _engineFieldPlacer( std::move( p_engineFieldPlacer ) ),
		_uiObjects( UiData { .engineInfo = std::move( p_engineInfo ) } ), _applyVisitor( std::move( p_applyVisitor ) )
	{
		_setupUi( p_parent, p_title );
		_setupSlots();
	}

	SettingsDialog::~SettingsDialog() noexcept
	{
		if ( _uiObjects.container )
		{
			_uiObjects.container->close();
		}
	}

	void SettingsDialog::_setupDataUi( QVBoxLayout * const				  p_qMainLayout,
									   UiData &							  p_uiObjects,
									   const MdParametersStepData * const p_data )
	{
		QWidget *			label	= nullptr;
		QFormLayout * const qLayout = VTX::UI::QT::Helper::makeChildLayout<QFormLayout>( *p_qMainLayout );

		VTX::Tool::Mdprep::ui::get( E_COMMON_FIELDS_DOUBLE::dt, label, p_uiObjects.fieldDt );
		qLayout->addRow( label, p_uiObjects.fieldDt );

		p_uiObjects.fieldDt->setValue( p_data->dt );

		VTX::Tool::Mdprep::ui::get(
			E_COMMON_FIELDS_UINT64::coordinateSaveInterval, label, p_uiObjects.fieldSaveInterval );
		qLayout->addRow( label, p_uiObjects.fieldSaveInterval );
		p_uiObjects.fieldSaveInterval->setText( QString::number( p_data->nsteps ) );
	}

	void SettingsDialog::_setupDataUi( QVBoxLayout * const			  p_qMainLayout,
									   UiData &						  p_uiObjects,
									   const MinimizationData * const p_data )
	{
		QWidget *			label	= nullptr;
		QFormLayout * const qLayout = VTX::UI::QT::Helper::makeChildLayout<QFormLayout>( *p_qMainLayout );

		VTX::Tool::Mdprep::ui::get(
			E_COMMON_FIELDS_DOUBLE::minimizationEnergyBreakpoint, label, p_uiObjects.fieldMinEnergyBreakpoint );
		qLayout->addRow( label, p_uiObjects.fieldMinEnergyBreakpoint );
		p_uiObjects.fieldMinEnergyBreakpoint->setValue( p_data->energyBreakpoint );

		VTX::Tool::Mdprep::ui::get( E_COMMON_FIELDS_DOUBLE::minimizationStepSize, label, p_uiObjects.fieldMinStepSize );
		qLayout->addRow( label, p_uiObjects.fieldMinStepSize );
		p_uiObjects.fieldMinStepSize->setValue( p_data->stepSize );
	}

	void SettingsDialog::_setupDataUi( QVBoxLayout * const		p_qMainLayout,
									   UiData &					p_uiObjects,
									   const SystemData * const p_data )
	{
		QFormLayout * const qLayout = VTX::UI::QT::Helper::makeChildLayout<QFormLayout>( *p_qMainLayout );
		QWidget *			w		= nullptr;
		// Engine info is optional : without it, the comboboxes are simply left empty.
		const Gateway::EngineSpecificCommonInformation * const engineInfo
			= p_uiObjects.engineInfo.has_value() ? &p_uiObjects.engineInfo.value() : nullptr;

		get( E_COMMON_FIELDS_COMBOBOX ::bioForcefield, w, p_uiObjects.fieldForceFieldBio );
		if ( engineInfo != nullptr )
		{
			for ( const auto & it_bioFf : engineInfo->bioForcefieldCollection )
			{
				p_uiObjects.fieldForceFieldBio->addItem( QString::fromStdString( it_bioFf ) );
				if ( it_bioFf == p_data->forcefieldBio )
				{
					p_uiObjects.fieldForceFieldBio->setCurrentIndex( p_uiObjects.fieldForceFieldBio->count() - 1 );
				}
			}
		}
		qLayout->addRow( w, p_uiObjects.fieldForceFieldBio );

		get( E_COMMON_FIELDS_COMBOBOX::chemForcefield, w, p_uiObjects.fieldForceFieldChem );
		p_uiObjects.fieldForceFieldChem->addItem( "Not supported by VTX yet" );
		qLayout->addRow( w, p_uiObjects.fieldForceFieldChem );

		get( E_COMMON_FIELDS_COMBOBOX::boxShape, w, p_uiObjects.fieldBoxShape );
		if ( engineInfo != nullptr )
		{
			for ( const auto & it_shapeStr : engineInfo->boxShapeCollection )
			{
				p_uiObjects.fieldBoxShape->addItem( QString::fromStdString( it_shapeStr ) );
			}
		}
		if ( std::cmp_less( p_data->boxShapeIdx, p_uiObjects.fieldBoxShape->count() ) )
		{
			p_uiObjects.fieldBoxShape->setCurrentIndex( static_cast<int>( p_data->boxShapeIdx ) );
		}
		qLayout->addRow( w, p_uiObjects.fieldBoxShape );

		get( E_COMMON_FIELDS_COMBOBOX ::waterModel, w, p_uiObjects.fieldWaterModel );
		if ( engineInfo != nullptr )
		{
			for ( const auto & it_waterModelsStr : engineInfo->waterModels )
			{
				p_uiObjects.fieldWaterModel->addItem( QString::fromStdString( it_waterModelsStr ) );
				if ( it_waterModelsStr == p_data->waterModel )
				{
					p_uiObjects.fieldWaterModel->setCurrentIndex( p_uiObjects.fieldWaterModel->count() - 1 );
				}
			}
		}
		qLayout->addRow( w, p_uiObjects.fieldWaterModel );
	}

	void SettingsDialog::_setupUi( QWidget * const p_parent, const char * p_title )
	{
		_uiObjects.container = VTX::UI::QT::Helper::makeQObject<QDialog>( p_parent );
		_uiObjects.container->setWindowTitle( p_title );
		QVBoxLayout * qMainLayout = VTX::UI::QT::Helper::makeQObject<QVBoxLayout>( _uiObjects.container );

		// The variant part
		std::visit( [ & ]( auto p_ ) { _setupDataUi( qMainLayout, _uiObjects, p_ ); }, _data );

		QFormLayout * qEngineSpecificLayout = VTX::UI::QT::Helper::makeChildLayout<QFormLayout>( *qMainLayout );

		_engineFieldPlacer.placeFields( qEngineSpecificLayout );

		if ( std::holds_alternative<SystemData *>( _data ) && _uiObjects.engineInfo.has_value() )
		{
			this->_engineFieldPlacer.update( EngineSpecificCommonInformationFieldUpdate {
				.field	  = E_ENGINE_SPECIFIC_COMMON_INFORMATION_FIELD ::boxShape,
				.newValue = std::get<SystemData *>( this->_data )->boxShapeIdx } );
			QObject::connect( _uiObjects.fieldBoxShape,
							  &QComboBox::currentTextChanged,
							  [ & ]
							  {
								  this->_engineFieldPlacer.update( EngineSpecificCommonInformationFieldUpdate {
									  .field	= E_ENGINE_SPECIFIC_COMMON_INFORMATION_FIELD ::boxShape,
									  .newValue = static_cast<size_t>( _uiObjects.fieldBoxShape->currentIndex() ) } );
							  } );
		}

		qMainLayout->addSpacerItem( std::make_unique<QSpacerItem>( 0, 20 ).release() );
		QHBoxLayout * qButtonLayout = VTX::UI::QT::Helper::makeChildLayout<QHBoxLayout>( *qMainLayout );
		qButtonLayout->addStretch( 1 );
		_uiObjects.buttonApply	= VTX::UI::QT::Helper::makeQObject<QPushButton>( "Apply", _uiObjects.container );
		_uiObjects.buttonCancel = VTX::UI::QT::Helper::makeQObject<QPushButton>( "Cancel", _uiObjects.container );
		qButtonLayout->addWidget( _uiObjects.buttonApply );
		qButtonLayout->addWidget( _uiObjects.buttonCancel );
		_uiObjects.container->open();
	}

	void SettingsDialog::_setupSlots()
	{
		QPushButton::connect( _uiObjects.buttonApply, &QPushButton::clicked, [ & ]() { this->_applyChanges(); } );
		QPushButton::connect( _uiObjects.buttonCancel, &QPushButton::clicked, [ & ]() { this->_cancelChanges(); } );
	}

	void SettingsDialog::_applyDataChange( const UiData & p_uiObjects, MinimizationData * const p_data )
	{
		p_data->energyBreakpoint = p_uiObjects.fieldMinEnergyBreakpoint->value();
		p_data->stepSize		 = p_uiObjects.fieldMinStepSize->value();
	}

	void SettingsDialog::_applyDataChange( const UiData & p_uiObjects, MdParametersStepData * const p_data )
	{
		p_data->dt	   = p_uiObjects.fieldDt->value();
		p_data->nsteps = p_uiObjects.fieldSaveInterval->text().toUInt();
	}

	void SettingsDialog::_applyDataChange( const UiData & p_uiObjects, SystemData * const p_data )
	{
		p_data->forcefieldBio = p_uiObjects.fieldForceFieldBio->currentText().toStdString();
		p_data->boxShapeIdx	  = p_uiObjects.fieldBoxShape->currentIndex();
		p_data->waterModel	  = p_uiObjects.fieldWaterModel->currentText().toStdString();
	}

	void SettingsDialog::_applyChanges()
	{
		std::visit( [ & ]( auto p_ ) { _applyDataChange( _uiObjects, p_ ); }, _data );
		_applyVisitor.applied();
		_engineFieldPlacer.apply();
		_uiObjects.container->close();
	}

	void SettingsDialog::_cancelChanges() const { _uiObjects.container->close(); }
} // namespace VTX::Tool::Mdprep::ui::form_basic
