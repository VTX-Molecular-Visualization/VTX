#include "ui/old_ui/ui/widget/representation/representation_inspector_section.hpp"
#include "ui/old_ui/style.hpp"
#include "ui/old_ui/ui/widget/representation/base_representation_widget.hpp"
#include "ui/old_ui/ui/widget_factory.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <app/core/action/action_manager.hpp>
#include <app/old_app/model/representation/representation.hpp>
#include <app/old_app/model/representation/representation_library.hpp>
#include <app/core/mvc/mvc_manager.hpp>
#include <app/old_app/representation/representation_manager.hpp>
#include <app/old_app/selection/selection_manager.hpp>
#include <app/view/callback_view.hpp>
#include <string>

namespace VTX::UI::Widget::Representation
{
	RepresentationInspectorSection::RepresentationInspectorSection( QWidget * const p_parent ) :
		BaseManualWidget( p_parent ), TMultiDataField()
	{
		_registerEvent( VTX::Event::Global::LATE_UPDATE );
	}

	RepresentationInspectorSection::~RepresentationInspectorSection()
	{
		// Views "UI_INSPECTOR_INSTANTIATED_REPRESENTATION" delete with models

		if ( _dummyRepresentation != nullptr )
			VTX::Core::MVC::MvcManager::get().deleteModel( _dummyRepresentation );
	}

	void RepresentationInspectorSection::receiveEvent( const VTX::Event::VTXEvent & p_event )
	{
		if ( p_event.name == VTX::Event::Global::LATE_UPDATE )
		{
			if ( _isDirty )
			{
				_recomputeUi();
				_isDirty = false;
			}
		}
	}

	void RepresentationInspectorSection::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		_titleWidget = new CustomWidget::QPushButtonMultiField( this );
		_titleWidget->setFlat( true );
		_titleWidget->setIcon( Style::IconConst::get().FOLDED_PIXMAP );

		_representationPreset
			= WidgetFactory::get().instantiateWidget<RepresentationLibraryComboBox>( this, "RepresentationPreset" );

		_representationWidget = new QWidget( this );

		_settingLayout = new QVBoxLayout( _representationWidget );
		_settingLayout->addWidget( _representationPreset );

		_representationWidget->setVisible( false );

		_applyToChildrenButton = new QPushButton( this );
		_applyToChildrenButton->setText( "Apply to children" );
		_revertButton = new QPushButton( this );
		_revertButton->setText( "Revert" );

		QHBoxLayout * buttonsLayout = new QHBoxLayout();
		buttonsLayout->addStretch( 1000 );
		buttonsLayout->addWidget( _applyToChildrenButton );
		buttonsLayout->addWidget( _revertButton );

		QHBoxLayout * titleLayout = new QHBoxLayout();
		titleLayout->addWidget( _titleWidget );
		titleLayout->addStretch( 1000 );

		QVBoxLayout * const mainLayout = new QVBoxLayout( this );
		mainLayout->addItem( titleLayout );
		mainLayout->addWidget( _representationWidget );
		mainLayout->addItem( buttonsLayout );

		_setSettingDisplay( true );
	}
	void RepresentationInspectorSection::_setupSlots()
	{
		connect( _titleWidget, &QPushButton::clicked, this, &RepresentationInspectorSection::_toggleSettingDisplay );

		connect( _representationPreset,
				 QOverload<int>::of( &QComboBox::currentIndexChanged ),
				 this,
				 &RepresentationInspectorSection::_representationPresetChange );

		connect( _revertButton, &QPushButton::clicked, this, &RepresentationInspectorSection::_revertRepresentation );
		connect( _applyToChildrenButton,
				 &QPushButton::clicked,
				 this,
				 &RepresentationInspectorSection::_applyRepresentationToChildren );
	}

	void RepresentationInspectorSection::refresh()
	{
		const bool oldBlockState = blockSignals( true );

		if ( !hasDifferentData() )
			_titleWidget->setText( QString::fromStdString( _dummyRepresentation->getName() ) );

		if ( _representationSettingWidget != nullptr )
			_representationSettingWidget->refresh();

		blockSignals( oldBlockState );
	}

	void RepresentationInspectorSection::setActionButtonVisibility( const ActionButtons & p_buttons )
	{
		_applyToChildrenButton->setVisible( p_buttons & ActionButtons::ApplyToChildren );
		_revertButton->setVisible( p_buttons & ActionButtons::Revert );
	}

	void RepresentationInspectorSection::_instantiateRepresentationSettingWidget(
		const Generic::REPRESENTATION & p_representation )
	{
		switch ( p_representation )
		{
		case Generic::REPRESENTATION::BALL_AND_STICK:
			_representationSettingWidget
				= VTX::UI::WidgetFactory::get().instantiateWidget<BallAndStickRepresentationWidget>(
					_representationWidget, "ball_and_stick_representation_widget" );
			break;
		case Generic::REPRESENTATION::BALL_AND_STICK_AND_CARTOON:
			_representationSettingWidget
				= VTX::UI::WidgetFactory::get().instantiateWidget<BallStickAndCartoonRepresentationWidget>(
					_representationWidget, "ball_stick_and_cartoon_representation_widget" );
			break;
		case Generic::REPRESENTATION::CARTOON:
			_representationSettingWidget = VTX::UI::WidgetFactory::get().instantiateWidget<CartoonRepresentationWidget>(
				_representationWidget, "cartoon_representation_widget" );
			break;
		case Generic::REPRESENTATION::SAS:
			_representationSettingWidget = VTX::UI::WidgetFactory::get().instantiateWidget<SasRepresentationWidget>(
				_representationWidget, "sas_representation_widget" );
			break;
		case Generic::REPRESENTATION::STICK:
			_representationSettingWidget = VTX::UI::WidgetFactory::get().instantiateWidget<StickRepresentationWidget>(
				_representationWidget, "stick_representation_widget" );
			break;
		case Generic::REPRESENTATION::STICK_AND_CARTOON:
			_representationSettingWidget
				= VTX::UI::WidgetFactory::get().instantiateWidget<StickAndCartoonRepresentationWidget>(
					_representationWidget, "stick_and_cartoon_representation_widget" );
			break;
			// !V0.1
		// case Generic::REPRESENTATION::TRACE:
		//	_representationSettingWidget = VTX::UI::WidgetFactory::get().instantiateWidget<TraceRepresentationWidget>(
		//		_representationWidget, "trace_representation_widget" );
		//	break;
		case Generic::REPRESENTATION::VAN_DER_WAALS:
			_representationSettingWidget = VTX::UI::WidgetFactory::get().instantiateWidget<VdwRepresentationWidget>(
				_representationWidget, "vdw_representation_widget" );
			break;
		case Generic::REPRESENTATION::SES:
			_representationSettingWidget = VTX::UI::WidgetFactory::get().instantiateWidget<SesRepresentationWidget>(
				_representationWidget, "ses_representation_widget" );
			break;
		default:
			VTX_WARNING( "Widget setting for representation " + std::to_string( int( p_representation ) )
						 + " not managed in RepresentationInspectorSection::_instantiateRepresentationSettingWidget." );
			_representationSettingWidget = nullptr;
			break;
		}

		if ( _representationSettingWidget != nullptr )
		{
			_settingLayout->addWidget( _representationSettingWidget );

			connect( _representationSettingWidget,
					 &VTX::UI::Widget::Representation::BaseRepresentationWidget::onDataChange,
					 this,
					 &RepresentationInspectorSection::_representationDataChange );

			connect( _representationSettingWidget,
					 &VTX::UI::Widget::Representation::BaseRepresentationWidget::onColorChange,
					 this,
					 &RepresentationInspectorSection::_representationColorChange );
		}
	}
	void RepresentationInspectorSection::_deleteRepresentationSettingWidget()
	{
		if ( _representationSettingWidget != nullptr )
		{
			_settingLayout->takeAt( _settingLayout->count() - 1 );
			delete _representationSettingWidget;

			_representationSettingWidget = nullptr;
		}
	}

	void RepresentationInspectorSection::localize() {}

	void RepresentationInspectorSection::_toggleSettingDisplay() const
	{
		_setSettingDisplay( !_representationWidget->isVisible() );
	}

	void RepresentationInspectorSection::_setSettingDisplay( const bool p_expand ) const
	{
		_representationWidget->setVisible( p_expand );

		const QIcon & icon = p_expand ? Style::IconConst::get().UNFOLDED_PIXMAP : Style::IconConst::get().FOLDED_PIXMAP;

		_titleWidget->setIcon( icon );
	}

	void RepresentationInspectorSection::_representationPresetChange( const int p_presetIndex )
	{
		if ( signalsBlocked() )
			return;

		emit onRepresentationPresetChange( p_presetIndex );
	}
	void RepresentationInspectorSection::_representationDataChange(
		const Model::Representation::MEMBER_FLAG & p_flagDataModified )
	{
		if ( signalsBlocked() )
			return;

		emit onRepresentationChange( *_dummyRepresentation, p_flagDataModified );
	}
	void RepresentationInspectorSection::_representationColorChange( const Color::Rgba & p_color, const bool p_ssColor )
	{
		if ( signalsBlocked() )
			return;

		bool changeRepresentationColor
			= p_ssColor
			  && _dummyRepresentation->getRibbonData().colorMode == Generic::SECONDARY_STRUCTURE_COLOR_MODE::CUSTOM;

		changeRepresentationColor
			= changeRepresentationColor
			  || ( !p_ssColor
				   && ( _dummyRepresentation->getColorMode() == Generic::COLOR_MODE::CUSTOM
						|| _dummyRepresentation->getColorMode() == Generic::COLOR_MODE::ATOM_CUSTOM ) );

		if ( changeRepresentationColor )
		{
			_dummyRepresentation->setColor( p_color, false, true );
		}

		emit onRepresentationColorChange( *_dummyRepresentation, p_color, p_ssColor );
	}

	void RepresentationInspectorSection::_revertRepresentation() { emit onRevertRepresentation(); }
	void RepresentationInspectorSection::_applyRepresentationToChildren() { emit onApplyRepresentationToChildren(); }

	void RepresentationInspectorSection::resetState() { resetState( true, true ); }
	void RepresentationInspectorSection::resetState( const bool p_deleteViews, const bool p_deleteDataWidget )
	{
		if ( p_deleteViews )
		{
			for ( const Model::ID & representationID : _representationIDs )
			{
				if ( VTX::Core::MVC::MvcManager::get().doesModelExists( representationID ) )
				{
					const InstantiatedRepresentation & representationModel
						= VTX::Core::MVC::MvcManager::get().getModel<InstantiatedRepresentation>( representationID );

					if ( VTX::Core::MVC::MvcManager::get().hasView( &representationModel,
														 ID::View::UI_INSPECTOR_INSTANTIATED_REPRESENTATION ) )
					{
						VTX::Core::MVC::MvcManager::get().deleteView( &representationModel,
														   ID::View::UI_INSPECTOR_INSTANTIATED_REPRESENTATION );
					}
				}
			}
			_representationIDs.clear();
		}

		_titleWidget->resetState();
		_representationPreset->resetState();

		if ( _representationSettingWidget != nullptr )
		{
			if ( p_deleteDataWidget )
				_deleteRepresentationSettingWidget();
			else
				_representationSettingWidget->resetState();
		}

		_baseRepresentationIndex = -1;
		_state					 = MultiDataField::State::Uninitialized;
	}

	void RepresentationInspectorSection::updateWithNewValue( const InstantiatedRepresentation & p_representation )
	{
		updateWithNewValue( p_representation, true );
	}
	void RepresentationInspectorSection::updateWithNewValue( const InstantiatedRepresentation & p_representation,
															 const bool							p_instantiateViews )
	{
		const bool oldBlockState = blockSignals( true );

		const int baseRepresentationIndex = Model::Representation::RepresentationLibrary::get().getRepresentationIndex(
			p_representation.getLinkedRepresentation() );

		_titleWidget->updateWithNewValue( p_representation.getName() );
		_representationPreset->updateWithNewValue( baseRepresentationIndex );

		if ( _baseRepresentationIndex == -1 )
		{
			if ( _dummyRepresentation == nullptr )
			{
				_dummyRepresentation
					= VTX::Representation::RepresentationManager::get().instantiateDummy( p_representation );

				VTX::View::CallbackView<InstantiatedRepresentation, RepresentationInspectorSection> * const
					representationView
					= VTX::Core::MVC::MvcManager::get()
						  .instantiateView<
							  VTX::View::CallbackView<InstantiatedRepresentation, RepresentationInspectorSection>>(
							  _dummyRepresentation, ID::View::UI_INSPECTOR_INSTANTIATED_REPRESENTATION );

				representationView->setCallback( this, &RepresentationInspectorSection::_onDummyChange );
			}
			else
			{
				_dummyRepresentation->setLinkedRepresentation( p_representation.getLinkedRepresentation() );
				_dummyRepresentation->setTarget( p_representation.getTarget() );
				_dummyRepresentation->applyData(
					p_representation, p_representation.getOverridedMembersFlag(), false, false );
			}

			if ( _representationSettingWidget == nullptr )
				_instantiateRepresentationSettingWidget( p_representation.getRepresentationType() );

			_representationSettingWidget->setRepresentation( _dummyRepresentation );
			_representationSettingWidget->updateWithNewValue( *_dummyRepresentation );

			_baseRepresentationIndex = baseRepresentationIndex;
			_state					 = MultiDataField::State::Identical;
		}
		else
		{
			if ( !hasDifferentData() )
			{
				if ( baseRepresentationIndex == _baseRepresentationIndex )
				{
					_representationSettingWidget->updateWithNewValue( p_representation );
				}
				else // Can't managed modifications on multiple representation presets
				{
					_state = MultiDataField::State::Different;
					_deleteRepresentationSettingWidget();
				}
			}
		}

		if ( p_instantiateViews )
		{
			if ( _representationIDs.find( p_representation.getId() ) == _representationIDs.end() )
			{
				VTX::View::CallbackView<const InstantiatedRepresentation, RepresentationInspectorSection> * const
					viewOnRepresentation
					= VTX::Core::MVC::MvcManager::get()
						  .instantiateView<VTX::View::CallbackView<const InstantiatedRepresentation,
																   RepresentationInspectorSection>>(
							  &p_representation, ID::View::UI_INSPECTOR_INSTANTIATED_REPRESENTATION );

				viewOnRepresentation->setCallback( this,
												   &RepresentationInspectorSection::_onTargetedRepresentationChange );
				_representationIDs.emplace( p_representation.getId() );
			}
		}

		blockSignals( oldBlockState );
	}

	void RepresentationInspectorSection::_displayDifferentsDataFeedback() {}

	void RepresentationInspectorSection::_onTargetedRepresentationChange( const VTX::Event::VTXEvent * const p_event )
	{
		resetState( false, p_event->name == VTX::Event::Model::REPRESENTATION_TYPE_CHANGE );
		setDirty();
	}

	void RepresentationInspectorSection::setDirty() { _isDirty = true; }

	void RepresentationInspectorSection::_recomputeUi()
	{
		for ( const Model::ID & representationID : _representationIDs )
		{
			const InstantiatedRepresentation & representation
				= VTX::Core::MVC::MvcManager::get().getModel<InstantiatedRepresentation>( representationID );
			updateWithNewValue( representation, false );
		}
	}

	void RepresentationInspectorSection::_onDummyChange( const VTX::Event::VTXEvent * const p_event ) { refresh(); }

} // namespace VTX::UI::Widget::Representation
