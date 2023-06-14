#include "ui/old_ui/ui/widget/settings/setting_molecule_widget.hpp"
#include "ui/old_ui/style.hpp"
#include "ui/old_ui/ui/dialog.hpp"
#include "ui/old_ui/ui/main_window.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include "ui/old_ui/util/ui.hpp"
#include "ui/old_ui/vtx_app.hpp"
#include <QLabel>
#include <app/old/action/main.hpp>
#include <app/old/action/setting.hpp>
#include <app/old/event/global.hpp>
#include <app/old/internal/io/serialization/image_export.hpp>
#include <app/old/application/setting.hpp>
#include <app/old/component/chemistry/enum_trajectory.hpp>

namespace VTX::UI::Widget::Settings
{
	SettingMoleculeWidget::SettingMoleculeWidget( QWidget * const p_parent ) : BaseManualWidget( p_parent )
	{
		_registerEvent( VTX::App::Old::Event::Global::SETTINGS_CHANGE );
	}

	void SettingMoleculeWidget::receiveEvent( const VTX::App::Old::Core::Event::VTXEvent & p_event )
	{
		if ( p_event.name == VTX::App::Old::Event::Global::SETTINGS_CHANGE )
		{
			const VTX::App::Old::Core::Event::VTXEventArg<const std::set<VTX::App::Old::Application::Setting::PARAMETER> &> & castedEvent
				= static_cast<const VTX::App::Old::Core::Event::VTXEventArg<const std::set<VTX::App::Old::Application::Setting::PARAMETER> &> &>(
					p_event );

			if ( castedEvent.get().find( VTX::App::Old::Application::Setting::PARAMETER::DEFAULT_REPRESENTATION_PER_CATEGORY )
				 != castedEvent.get().end() )
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
		_refreshRepresentationComboBox( _defaultRepresentationForPolymer, VTX::Core::ChemDB::Category::TYPE::POLYMER );
		_refreshRepresentationComboBox( _defaultRepresentationForCarbohydrate, VTX::Core::ChemDB::Category::TYPE::CARBOHYDRATE );
		_refreshRepresentationComboBox( _defaultRepresentationForLigand, VTX::Core::ChemDB::Category::TYPE::LIGAND );
		_refreshRepresentationComboBox( _defaultRepresentationForIon, VTX::Core::ChemDB::Category::TYPE::ION );
		_refreshRepresentationComboBox( _defaultRepresentationForSolvent, VTX::Core::ChemDB::Category::TYPE::SOLVENT );
		_refreshRepresentationComboBox( _defaultRepresentationForWater, VTX::Core::ChemDB::Category::TYPE::WATER );
	}

	void SettingMoleculeWidget::_refreshRepresentationComboBox( RepresentationLibraryComboBox * const p_comboBox,
																const VTX::Core::ChemDB::Category::TYPE					  p_categoryEnum )
	{
		p_comboBox->setCurrentIndex( VTX_SETTING().getDefaultRepresentationIndexPerCategory( p_categoryEnum ) );
	}

	void SettingMoleculeWidget::_defaultRepresentationForPolymerChange( int p_index )
	{
		VTX_ACTION(
			new App::Old::Action::Setting::ChangeDefaultRepresentationPerCategory( VTX::Core::ChemDB::Category::TYPE::POLYMER, p_index ) );
	}
	void SettingMoleculeWidget::_defaultRepresentationForCarbohydrateChange( int p_index )
	{
		VTX_ACTION(
			new App::Old::Action::Setting::ChangeDefaultRepresentationPerCategory( VTX::Core::ChemDB::Category::TYPE::CARBOHYDRATE, p_index ) );
	}
	void SettingMoleculeWidget::_defaultRepresentationForLigandChange( int p_index )
	{
		VTX_ACTION(
			new App::Old::Action::Setting::ChangeDefaultRepresentationPerCategory( VTX::Core::ChemDB::Category::TYPE::LIGAND, p_index ) );
	}
	void SettingMoleculeWidget::_defaultRepresentationForIonChange( int p_index )
	{
		VTX_ACTION( new App::Old::Action::Setting::ChangeDefaultRepresentationPerCategory( VTX::Core::ChemDB::Category::TYPE::ION, p_index ) );
	}
	void SettingMoleculeWidget::_defaultRepresentationForSolventChange( int p_index )
	{
		VTX_ACTION(
			new App::Old::Action::Setting::ChangeDefaultRepresentationPerCategory( VTX::Core::ChemDB::Category::TYPE::SOLVENT, p_index ) );
	}
	void SettingMoleculeWidget::_defaultRepresentationForWaterChange( int p_index )
	{
		VTX_ACTION( new App::Old::Action::Setting::ChangeDefaultRepresentationPerCategory( VTX::Core::ChemDB::Category::TYPE::WATER, p_index ) );
	}

	void SettingMoleculeWidget::_restoreDefaultRepresentations()
	{
		UI::Dialog::confirmActionDialog(
			new App::Old::Action::Setting::RestoreDefaultRepresentationPerCategory(),
			"Confirm",
			"Are you sure you want to restore default representation ? All changes will be lost." );
	}

} // namespace VTX::UI::Widget::Settings
