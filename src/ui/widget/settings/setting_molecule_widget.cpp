#include "setting_molecule_widget.hpp"
#include "action/action_manager.hpp"
#include "action/main.hpp"
#include "action/setting.hpp"
#include "io/struct/image_export.hpp"
#include "setting.hpp"
#include "style.hpp"
#include "trajectory/trajectory_enum.hpp"
#include "ui/dialog.hpp"
#include "ui/main_window.hpp"
#include "ui/widget_factory.hpp"
#include "util/ui.hpp"
#include "vtx_app.hpp"
#include <QLabel>

namespace VTX::UI::Widget::Settings
{
	SettingMoleculeWidget::SettingMoleculeWidget( QWidget * const p_parent ) : BaseManualWidget( p_parent )
	{
		_registerEvent( Event::Global::SETTINGS_CHANGE );
	}

	void SettingMoleculeWidget::receiveEvent( const Event::VTXEvent & p_event )
	{
		if ( p_event.name == Event::Global::SETTINGS_CHANGE )
		{
			const Event::VTXEventRef<std::set<Setting::PARAMETER>> & castedEvent
				= static_cast<const Event::VTXEventRef<std::set<Setting::PARAMETER>> &>( p_event );

			if ( castedEvent.ref.find( Setting::PARAMETER::DEFAULT_REPRESENTATION_PER_CATEGORY )
				 != castedEvent.ref.end() )
			{
				_refreshData();
			}
		}
	}

	void SettingMoleculeWidget::_setupUi( const QString & p_name )
	{
		QScrollArea * const settingsScrollArea = new QScrollArea( this );
		QWidget * const		viewport		   = new QWidget( settingsScrollArea );

		QVBoxLayout * const mainLayout = new QVBoxLayout( this );
		_layout						   = new Layout::AttributeListLayout( viewport );

		QHBoxLayout * const bottomLayout = new QHBoxLayout();

		_defaultRepresentationForPolymer = WidgetFactory::get().instantiateWidget<RepresentationLibraryComboBox>(
			this, "defaultRepresentationForPolymer" );
		_defaultRepresentationForCarbohydrate = WidgetFactory::get().instantiateWidget<RepresentationLibraryComboBox>(
			this, "defaultRepresentationForCarbohydrate" );
		_defaultRepresentationForLigand = WidgetFactory::get().instantiateWidget<RepresentationLibraryComboBox>(
			this, "defaultRepresentationForLigand" );
		_defaultRepresentationForIon = WidgetFactory::get().instantiateWidget<RepresentationLibraryComboBox>(
			this, "defaultRepresentationForIon" );
		_defaultRepresentationForSolvent = WidgetFactory::get().instantiateWidget<RepresentationLibraryComboBox>(
			this, "defaultRepresentationForSolvent" );
		_defaultRepresentationForWater = WidgetFactory::get().instantiateWidget<RepresentationLibraryComboBox>(
			this, "defaultRepresentationForWater" );

		_restoreButton = new QPushButton( this );

		_layout->startAttributeSection( "Default Representation" );
		_layout->addAttribute( _defaultRepresentationForPolymer, "Polymer" );
		_layout->addAttribute( _defaultRepresentationForCarbohydrate, "Carbohydrate" );
		_layout->addAttribute( _defaultRepresentationForLigand, "Ligand" );
		_layout->addAttribute( _defaultRepresentationForIon, "Ion" );
		_layout->addAttribute( _defaultRepresentationForSolvent, "Solvent" );
		_layout->addAttribute( _defaultRepresentationForWater, "Water" );
		_layout->finishAttributeSection( true );

		bottomLayout->addStretch( 1000 );
		bottomLayout->addWidget( _restoreButton );

		mainLayout->addWidget( settingsScrollArea, 1000 );
		mainLayout->addSpacing( 10 );
		mainLayout->addItem( bottomLayout );

		settingsScrollArea->setWidget( viewport );

		_refreshData();
	}

	void SettingMoleculeWidget::_setupSlots()
	{
		connect( _defaultRepresentationForPolymer,
				 &RepresentationLibraryComboBox::currentIndexChanged,
				 this,
				 &SettingMoleculeWidget::_defaultRepresentationForPolymerChange );
		connect( _defaultRepresentationForCarbohydrate,
				 &RepresentationLibraryComboBox::currentIndexChanged,
				 this,
				 &SettingMoleculeWidget::_defaultRepresentationForCarbohydrateChange );
		connect( _defaultRepresentationForLigand,
				 &RepresentationLibraryComboBox::currentIndexChanged,
				 this,
				 &SettingMoleculeWidget::_defaultRepresentationForLigandChange );
		connect( _defaultRepresentationForIon,
				 &RepresentationLibraryComboBox::currentIndexChanged,
				 this,
				 &SettingMoleculeWidget::_defaultRepresentationForIonChange );
		connect( _defaultRepresentationForSolvent,
				 &RepresentationLibraryComboBox::currentIndexChanged,
				 this,
				 &SettingMoleculeWidget::_defaultRepresentationForSolventChange );
		connect( _defaultRepresentationForWater,
				 &RepresentationLibraryComboBox::currentIndexChanged,
				 this,
				 &SettingMoleculeWidget::_defaultRepresentationForWaterChange );

		connect( _restoreButton, &QPushButton::clicked, this, &SettingMoleculeWidget::_restoreDefaultRepresentations );
	};
	void SettingMoleculeWidget::localize() { _restoreButton->setText( "Restore" ); };

	void SettingMoleculeWidget::_refreshData()
	{
		_refreshRepresentationComboBox( _defaultRepresentationForPolymer, CATEGORY_ENUM::POLYMER );
		_refreshRepresentationComboBox( _defaultRepresentationForCarbohydrate, CATEGORY_ENUM::CARBOHYDRATE );
		_refreshRepresentationComboBox( _defaultRepresentationForLigand, CATEGORY_ENUM::LIGAND );
		_refreshRepresentationComboBox( _defaultRepresentationForIon, CATEGORY_ENUM::ION );
		_refreshRepresentationComboBox( _defaultRepresentationForSolvent, CATEGORY_ENUM::SOLVENT );
		_refreshRepresentationComboBox( _defaultRepresentationForWater, CATEGORY_ENUM::WATER );
	}

	void SettingMoleculeWidget::_refreshRepresentationComboBox( RepresentationLibraryComboBox * const p_comboBox,
																const CATEGORY_ENUM					  p_categoryEnum )
	{
		p_comboBox->setCurrentIndex( VTX_SETTING().getDefaultRepresentationIndexPerCategory( p_categoryEnum ) );
	}

	void SettingMoleculeWidget::_defaultRepresentationForPolymerChange( int p_index )
	{
		VTX_ACTION( new Action::Setting::ChangeDefaultRepresentationPerCategory( CATEGORY_ENUM::POLYMER, p_index ) );
	}
	void SettingMoleculeWidget::_defaultRepresentationForCarbohydrateChange( int p_index )
	{
		VTX_ACTION(
			new Action::Setting::ChangeDefaultRepresentationPerCategory( CATEGORY_ENUM::CARBOHYDRATE, p_index ) );
	}
	void SettingMoleculeWidget::_defaultRepresentationForLigandChange( int p_index )
	{
		VTX_ACTION( new Action::Setting::ChangeDefaultRepresentationPerCategory( CATEGORY_ENUM::LIGAND, p_index ) );
	}
	void SettingMoleculeWidget::_defaultRepresentationForIonChange( int p_index )
	{
		VTX_ACTION( new Action::Setting::ChangeDefaultRepresentationPerCategory( CATEGORY_ENUM::ION, p_index ) );
	}
	void SettingMoleculeWidget::_defaultRepresentationForSolventChange( int p_index )
	{
		VTX_ACTION( new Action::Setting::ChangeDefaultRepresentationPerCategory( CATEGORY_ENUM::SOLVENT, p_index ) );
	}
	void SettingMoleculeWidget::_defaultRepresentationForWaterChange( int p_index )
	{
		VTX_ACTION( new Action::Setting::ChangeDefaultRepresentationPerCategory( CATEGORY_ENUM::WATER, p_index ) );
	}

	void SettingMoleculeWidget::_restoreDefaultRepresentations()
	{
		UI::Dialog::confirmActionDialog(
			new Action::Setting::RestoreDefaultRepresentationPerCategory(),
			"Confirm",
			"Are you sure you want to restore default representation ? All changes will be lost." );
	}

} // namespace VTX::UI::Widget::Settings
