#include "representation_inspector_section.hpp"
#include "action/action_manager.hpp"
#include "action/chain.hpp"
#include "action/molecule.hpp"
#include "action/residue.hpp"
#include "model/chain.hpp"
#include "model/molecule.hpp"
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

namespace VTX::UI::Widget::Representation
{
	RepresentationInspectorSection::RepresentationInspectorSection( QWidget * const p_parent ) :
		BaseManualWidget( p_parent )
	{
	}

	RepresentationInspectorSection ::~RepresentationInspectorSection() {}

	void RepresentationInspectorSection::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		_titleWidget = new QPushButton( this );
		_titleWidget->setFlat( true );
		_titleWidget->setIcon( Style::IconConst::get().FOLDED_PIXMAP );

		_representationWidget = new QWidget( this );

		_representationPreset = new QComboBox( _representationWidget );
		_populateRepresentationModeComboBox();

		_settingLayout = new QVBoxLayout( _representationWidget );
		_settingLayout->addWidget( _representationPreset );

		_representationWidget->setVisible( false );

		QHBoxLayout * titleLayout = new QHBoxLayout();
		titleLayout->addWidget( _titleWidget );
		titleLayout->addStretch( 1000 );

		QVBoxLayout * const mainLayout = new QVBoxLayout( this );
		mainLayout->addItem( titleLayout );
		mainLayout->addWidget( _representationWidget );
	}
	void RepresentationInspectorSection::_setupSlots()
	{
		connect( _titleWidget, &QPushButton::clicked, this, &RepresentationInspectorSection::_toggleSettingDisplay );

		connect( _representationPreset,
				 QOverload<int>::of( &QComboBox::currentIndexChanged ),
				 this,
				 &RepresentationInspectorSection::_representationPresetChange );
	}

	void RepresentationInspectorSection::_setTarget( Model::BaseModel * const			p_model,
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

	void RepresentationInspectorSection::_refresh()
	{
		_titleWidget->setText( QString::fromStdString( _representation->getName() ) );

		const Model::Representation::BaseRepresentation * const linkedRep = _representation->getLinkedRepresentation();

		const int representationPresetIndex
			= Model::Representation::RepresentationLibrary::get().getRepresentationIndex( linkedRep );

		const int representationTypeIndex = int( linkedRep->getRepresentationType() );

		_representationPreset->setCurrentIndex( representationPresetIndex );

		if ( _representationSettingWidget != nullptr )
		{
			_settingLayout->takeAt( _settingLayout->count() - 1 );
			delete _representationSettingWidget;
		}

		_representationSettingWidget
			= _instantiateRepresentationSettingWidget( Generic::REPRESENTATION( representationTypeIndex ) );

		_representationSettingWidget->setRepresentation( _representation );

		_settingLayout->addWidget( _representationSettingWidget );

		connect( _representationSettingWidget,
				 &VTX::UI::Widget::Representation::BaseRepresentationWidget::onDataChange,
				 this,
				 &RepresentationInspectorSection::_representationDataChange );
	}

	BaseRepresentationWidget * RepresentationInspectorSection::_instantiateRepresentationSettingWidget(
		const Generic::REPRESENTATION & p_representation )
	{
		BaseRepresentationWidget * res = nullptr;
		switch ( p_representation )
		{
		case Generic::REPRESENTATION::BALL_AND_STICK:
			res = VTX::UI::WidgetFactory::get().instantiateWidget<BallAndStickRepresentationWidget>(
				_representationWidget, "ball_and_stick_representation_widget" );
			break;
		case Generic::REPRESENTATION::BALL_AND_STICK_AND_CARTOON:
			res = VTX::UI::WidgetFactory::get().instantiateWidget<BallStickAndCartoonRepresentationWidget>(
				_representationWidget, "ball_stick_and_cartoon_representation_widget" );
			break;
		case Generic::REPRESENTATION::CARTOON:
			res = VTX::UI::WidgetFactory::get().instantiateWidget<CartoonRepresentationWidget>(
				_representationWidget, "cartoon_representation_widget" );
			break;
		case Generic::REPRESENTATION::SAS:
			res = VTX::UI::WidgetFactory::get().instantiateWidget<SasRepresentationWidget>(
				_representationWidget, "sas_representation_widget" );
			break;
		case Generic::REPRESENTATION::STICK:
			res = VTX::UI::WidgetFactory::get().instantiateWidget<StickRepresentationWidget>(
				_representationWidget, "stick_representation_widget" );
			break;
		case Generic::REPRESENTATION::STICK_AND_CARTOON:
			res = VTX::UI::WidgetFactory::get().instantiateWidget<StickAndCartoonRepresentationWidget>(
				_representationWidget, "stick_and_cartoon_representation_widget" );
			break;
		case Generic::REPRESENTATION::TRACE:
			res = VTX::UI::WidgetFactory::get().instantiateWidget<TraceRepresentationWidget>(
				_representationWidget, "trace_representation_widget" );
			break;
		case Generic::REPRESENTATION::VAN_DER_WAALS:
			res = VTX::UI::WidgetFactory::get().instantiateWidget<VdwRepresentationWidget>(
				_representationWidget, "vdw_representation_widget" );
			break;
		}

		return res;
	}

	void RepresentationInspectorSection::localize() {}

	void RepresentationInspectorSection::_toggleSettingDisplay()
	{
		bool newVisibleState = !_representationWidget->isVisible();
		_representationWidget->setVisible( newVisibleState );

		const QIcon & icon
			= newVisibleState ? Style::IconConst::get().UNFOLDED_PIXMAP : Style::IconConst::get().FOLDED_PIXMAP;

		_titleWidget->setIcon( icon );
	}

	void RepresentationInspectorSection::_representationPresetChange( const int p_presetIndex )
	{
		if ( signalsBlocked() )
			return;

		_representationSettingWidget->setRepresentation( nullptr );

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
	void RepresentationInspectorSection::_representationDataChange()
	{
		if ( signalsBlocked() )
			return;

		_setRepresentationIfNeeded();
	}

	void RepresentationInspectorSection::_setRepresentationIfNeeded()
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
	void RepresentationInspectorSection::_populateRepresentationModeComboBox()
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

} // namespace VTX::UI::Widget::Representation
