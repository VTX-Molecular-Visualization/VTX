#include "instantiated_representation_inspector_view.hpp"
#include "action/action_manager.hpp"
#include "action/chain.hpp"
#include "action/molecule.hpp"
#include "action/representable.hpp"
#include "action/residue.hpp"
#include "model/chain.hpp"
#include "model/representation/representation.hpp"
#include "model/representation/representation_library.hpp"
#include "model/residue.hpp"
#include "mvc/mvc_manager.hpp"
#include "selection/selection_manager.hpp"
#include "style.hpp"
#include "ui/widget/representation/base_representation_widget.hpp"
#include "ui/widget_factory.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <string>

namespace VTX::View::UI::Widget::Representation
{
	ItemRepresentationInspector::ItemRepresentationInspector( QWidget * const p_parent ) :
		InspectorItemWidget( p_parent )
	{
		_representationSettingsWidget = std::vector<BaseRepresentationWidget *>();
		_representationSettingsWidget.resize( int( Generic::REPRESENTATION::COUNT ) );
	}
	ItemRepresentationInspector ::~ItemRepresentationInspector() {}

	void ItemRepresentationInspector::_setupUi( const QString & p_name )
	{
		const int INDENT_SIZE = 20;

		InspectorItemWidget::_setupUi( p_name );

		_titleWidget = new QPushButton( this );
		_titleWidget->setFlat( true );
		_titleWidget->setIcon( Style::IconConst::get().FOLDED_PIXMAP );

		_representationWidget = new QWidget( this );

		_representationPreset = new QComboBox( _representationWidget );
		_populateRepresentationModeComboBox();

		QHBoxLayout * const representationSettingsHLayout = new QHBoxLayout();
		QVBoxLayout * const representationSettingsVLayout = new QVBoxLayout();

		_representationSettingsWidget[ int( Generic::REPRESENTATION::BALL_AND_STICK ) ]
			= VTX::UI::WidgetFactory::get().instantiateWidget<BallAndStickRepresentationWidget>(
				_representationWidget, "ball_and_stick_representation_widget" );
		_representationSettingsWidget[ int( Generic::REPRESENTATION::BALL_AND_STICK_AND_CARTOON ) ]
			= VTX::UI::WidgetFactory::get().instantiateWidget<BallStickAndCartoonRepresentationWidget>(
				_representationWidget, "ball_stick_and_cartoon_representation_widget" );
		_representationSettingsWidget[ int( Generic::REPRESENTATION::CARTOON ) ]
			= VTX::UI::WidgetFactory::get().instantiateWidget<CartoonRepresentationWidget>(
				_representationWidget, "cartoon_representation_widget" );
		_representationSettingsWidget[ int( Generic::REPRESENTATION::SAS ) ]
			= VTX::UI::WidgetFactory::get().instantiateWidget<SasRepresentationWidget>( _representationWidget,
																						"sas_representation_widget" );
		_representationSettingsWidget[ int( Generic::REPRESENTATION::STICK ) ]
			= VTX::UI::WidgetFactory::get().instantiateWidget<StickRepresentationWidget>(
				_representationWidget, "stick_representation_widget" );
		_representationSettingsWidget[ int( Generic::REPRESENTATION::STICK_AND_CARTOON ) ]
			= VTX::UI::WidgetFactory::get().instantiateWidget<StickAndCartoonRepresentationWidget>(
				_representationWidget, "stick_and_cartoon_representation_widget" );
		_representationSettingsWidget[ int( Generic::REPRESENTATION::TRACE ) ]
			= VTX::UI::WidgetFactory::get().instantiateWidget<TraceRepresentationWidget>(
				_representationWidget, "trace_representation_widget" );
		_representationSettingsWidget[ int( Generic::REPRESENTATION::VAN_DER_WAALS ) ]
			= VTX::UI::WidgetFactory::get().instantiateWidget<VdwRepresentationWidget>( _representationWidget,
																						"vdw_representation_widget" );

		for ( int i = 0; i < _representationSettingsWidget.size(); i++ )
		{
			representationSettingsVLayout->addWidget( _representationSettingsWidget[ i ] );
			_representationSettingsWidget[ i ]->setVisible( false );
		}

		representationSettingsHLayout->addSpacing( INDENT_SIZE );
		representationSettingsHLayout->addItem( representationSettingsVLayout );

		QHBoxLayout * const colorModeHorizontalLayout = new QHBoxLayout();
		_colorModeLabel								  = new QLabel( _representationWidget );
		_colorModeLabel->setText( "Color Mode" );
		_colorModeWidget = VTX::UI::WidgetFactory::get().instantiateWidget<ColorModeFieldWidget>( _representationWidget,
																								  "colorModeWidget" );

		colorModeHorizontalLayout->addWidget( _colorModeLabel );
		colorModeHorizontalLayout->addWidget( _colorModeWidget );

		QVBoxLayout * const settingLayout = new QVBoxLayout( _representationWidget );
		settingLayout->addWidget( _representationPreset );
		settingLayout->addItem( representationSettingsHLayout );
		settingLayout->addItem( colorModeHorizontalLayout );

		_representationWidget->setVisible( false );

		QHBoxLayout * titleLayout = new QHBoxLayout();
		titleLayout->addWidget( _titleWidget );
		titleLayout->addStretch( 1000 );

		QVBoxLayout * const mainLayout = new QVBoxLayout( this );
		mainLayout->addItem( titleLayout );
		mainLayout->addWidget( _representationWidget );
	}
	void ItemRepresentationInspector::_setupSlots()
	{
		InspectorItemWidget::_setupSlots();

		connect( _titleWidget, &QPushButton::clicked, this, &ItemRepresentationInspector::_toggleSettingDisplay );

		connect( _representationPreset,
				 QOverload<int>::of( &QComboBox::currentIndexChanged ),
				 this,
				 &ItemRepresentationInspector::_representationPresetChange );

		for ( int i = 0; i < _representationSettingsWidget.size(); i++ )
		{
			connect( _representationSettingsWidget[ i ],
					 &VTX::UI::Widget::Representation::BaseRepresentationWidget::onDataChange,
					 this,
					 &ItemRepresentationInspector::_representationDataChange );
		}

		connect( _colorModeWidget,
				 &ColorModeFieldWidget::colorModeChanged,
				 this,
				 &ItemRepresentationInspector::_colorModeChanged );

		connect(
			_colorModeWidget, &ColorModeFieldWidget::colorChanged, this, &ItemRepresentationInspector::_colorChanged );
	}

	void ItemRepresentationInspector::_setTarget( Model::BaseModel * const			 p_model,
												  Generic::BaseRepresentable * const p_representable )
	{
		_targetModel = p_model;
		_target		 = p_representable;

		if ( _target->hasCustomRepresentation() )
		{
			_representation				   = _target->getCustomRepresentation();
			_representationHasBeenModified = true;
		}
		else
		{
			_representation
				= MVC::MvcManager::get().instantiateModel<Model::Representation::InstantiatedRepresentation>(
					_target->getRepresentation() );
			_representation->setTarget( _target );
			_representationHasBeenModified = false;
		}

		blockSignals( true );
		_refresh();
		blockSignals( false );
	}

	void ItemRepresentationInspector::_refresh()
	{
		_titleWidget->setText( QString::fromStdString( _representation->getName() ) );

		const Model::Representation::BaseRepresentation * const linkedRep = _representation->getLinkedRepresentation();

		const int representationPresetIndex
			= Model::Representation::RepresentationLibrary::get().getRepresentationIndex( linkedRep );

		const int representationTypeIndex = int( linkedRep->getRepresentationType() );

		_representationPreset->setCurrentIndex( representationPresetIndex );
		_representationSettingsWidget[ representationTypeIndex ]->setRepresentation( _representation );

		for ( int i = 0; i < _representationSettingsWidget.size(); i++ )
		{
			const bool visible = i == representationTypeIndex;
			_representationSettingsWidget[ i ]->setVisible( visible );
		}

		_colorModeWidget->setColorMode( _representation->getColorMode() );
		_colorModeWidget->setColor( _representation->getColor() );
	}

	void ItemRepresentationInspector::localize() { InspectorItemWidget::localize(); }

	void ItemRepresentationInspector::_toggleSettingDisplay()
	{
		bool newVisibleState = !_representationWidget->isVisible();
		_representationWidget->setVisible( newVisibleState );

		const QIcon & icon
			= newVisibleState ? Style::IconConst::get().UNFOLDED_PIXMAP : Style::IconConst::get().FOLDED_PIXMAP;

		_titleWidget->setIcon( icon );
	}

	void ItemRepresentationInspector::_representationPresetChange( const int p_presetIndex )
	{
		if ( signalsBlocked() )
			return;

		const ID::VTX_ID & typeId = _targetModel->getTypeId();
		if ( typeId == ID::Model::MODEL_MOLECULE )
		{
			Model::Molecule * const molecule = static_cast<Model::Molecule *>( _targetModel );
			VTX_ACTION( new Action::Molecule::ChangeRepresentationPreset( *molecule, p_presetIndex ) );
		}
		else if ( typeId == ID::Model::MODEL_CHAIN )
		{
			Model::Chain * const chain = static_cast<Model::Chain *>( _targetModel );
			VTX_ACTION( new Action::Chain::ChangeRepresentationPreset( *chain, p_presetIndex ) );
		}
		else if ( typeId == ID::Model::MODEL_RESIDUE )
		{
			Model::Residue * const residue = static_cast<Model::Residue *>( _targetModel );
			VTX_ACTION( new Action::Residue::ChangeRepresentationPreset( *residue, p_presetIndex ) );
		}
	}
	void ItemRepresentationInspector::_representationDataChange()
	{
		if ( signalsBlocked() )
			return;

		_instantiateRepresentationIfNeeded();
	}

	void ItemRepresentationInspector::_colorModeChanged( const Generic::COLOR_MODE & p_colorMode )
	{
		if ( signalsBlocked() )
			return;

		_instantiateRepresentationIfNeeded();
		VTX_ACTION( new Action::ChangeRepresentationColorMode( _representation, p_colorMode ) );
	}
	void ItemRepresentationInspector::_colorChanged( const Color::Rgb & p_color )
	{
		if ( signalsBlocked() )
			return;

		_instantiateRepresentationIfNeeded();
		VTX_ACTION( new Action::ChangeRepresentationColor( _representation, p_color ) );
	}
	void ItemRepresentationInspector::_spheresRadiusChanged( const float p_radius )
	{
		if ( signalsBlocked() )
			return;

		_instantiateRepresentationIfNeeded();
		VTX_ACTION( new Action::ChangeRepresentationSphereRadius( _representation, p_radius ) );
	}
	void ItemRepresentationInspector::_spheresRadiusAddChanged( const float p_radius )
	{
		if ( signalsBlocked() )
			return;

		_instantiateRepresentationIfNeeded();
		VTX_ACTION( new Action::ChangeRepresentationSphereRadiusAdd( _representation, p_radius ) );
	}
	void ItemRepresentationInspector::_sticksRadiusChanged( const float p_radius )
	{
		if ( signalsBlocked() )
			return;

		_instantiateRepresentationIfNeeded();
		VTX_ACTION( new Action::ChangeRepresentationCylinderRadius( _representation, p_radius ) );
	}

	void ItemRepresentationInspector::_instantiateRepresentationIfNeeded()
	{
		if ( !_representationHasBeenModified )
		{
			const ID::VTX_ID & typeId = _targetModel->getTypeId();
			if ( typeId == ID::Model::MODEL_MOLECULE )
			{
				Model::Molecule * const molecule = static_cast<Model::Molecule *>( _targetModel );
				molecule->setRepresentation( _representation );
			}
			else if ( typeId == ID::Model::MODEL_CHAIN )
			{
				Model::Chain * const chain = static_cast<Model::Chain *>( _targetModel );
				chain->setRepresentation( _representation );
			}
			else if ( typeId == ID::Model::MODEL_RESIDUE )
			{
				Model::Residue * const residue = static_cast<Model::Residue *>( _targetModel );
				residue->setRepresentation( _representation );
			}

			_representationHasBeenModified = true;
		}
	}
	void ItemRepresentationInspector::_populateRepresentationModeComboBox()
	{
		const Model::Representation::RepresentationLibrary & representationLibrary
			= Model::Representation::RepresentationLibrary::get();

		for ( int i = 0; i < representationLibrary.getRepresentationCount(); i++ )
		{
			const Model::Representation::BaseRepresentation & representation
				= *representationLibrary.getRepresentation( i );

			_representationPreset->addItem( QString::fromStdString( representation.getName() ) );
		}
	}

} // namespace VTX::View::UI::Widget::Representation
