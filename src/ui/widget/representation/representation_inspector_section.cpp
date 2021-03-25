#include "representation_inspector_section.hpp"
#include "action/action_manager.hpp"
#include "model/representation/representation.hpp"
#include "model/representation/representation_library.hpp"
#include "mvc/mvc_manager.hpp"
#include "selection/selection_manager.hpp"
#include "style.hpp"
#include "ui/widget/representation/base_representation_widget.hpp"
#include "ui/widget_factory.hpp"
#include "view/ui/widget/inspector/all_inspector_view.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <string>

namespace VTX::UI::Widget::Representation
{
	RepresentationInspectorSection::RepresentationInspectorSection( QWidget * const p_parent ) :
		BaseManualWidget( p_parent ), TMultiDataField()
	{
	}

	RepresentationInspectorSection ::~RepresentationInspectorSection()
	{
		if ( _dummyRepresentation != nullptr )
			MVC::MvcManager::get().deleteModel( _dummyRepresentation );
	}

	void RepresentationInspectorSection::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		_titleWidget = new CustomWidget::QPushButtonMultiField( this );
		_titleWidget->setFlat( true );
		_titleWidget->setIcon( Style::IconConst::get().FOLDED_PIXMAP );

		_representationWidget = new QWidget( this );

		_representationPreset = new CustomWidget::QComboBoxMultiField( _representationWidget );
		_populateRepresentationModeComboBox();

		_settingLayout = new QVBoxLayout( _representationWidget );
		_settingLayout->addWidget( _representationPreset );

		_representationWidget->setVisible( false );

		_revertButton = new QPushButton( this );
		_revertButton->setText( "Revert" );
		_applyToChildrenButton = new QPushButton( this );
		_applyToChildrenButton->setText( "Apply to children" );

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
		case Generic::REPRESENTATION::TRACE:
			_representationSettingWidget = VTX::UI::WidgetFactory::get().instantiateWidget<TraceRepresentationWidget>(
				_representationWidget, "trace_representation_widget" );
			break;
		case Generic::REPRESENTATION::VAN_DER_WAALS:
			_representationSettingWidget = VTX::UI::WidgetFactory::get().instantiateWidget<VdwRepresentationWidget>(
				_representationWidget, "vdw_representation_widget" );
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

		emit onRepresentationPresetChange( p_presetIndex );
	}
	void RepresentationInspectorSection::_representationDataChange(
		const Model::Representation::MEMBER_FLAG & p_flagDataModified )
	{
		if ( signalsBlocked() )
			return;

		emit onRepresentationChange( *_dummyRepresentation, p_flagDataModified );
	}
	void RepresentationInspectorSection::_representationColorChange( const Color::Rgb & p_color, const bool p_ssColor )
	{
		if ( signalsBlocked() )
			return;

		emit onRepresentationColorChange( *_dummyRepresentation, p_color, p_ssColor );
	}

	void RepresentationInspectorSection::_revertRepresentation() { emit onRevertRepresentation(); }
	void RepresentationInspectorSection::_applyRepresentationToChildren() { emit onApplyRepresentationToChildren(); }

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

	void RepresentationInspectorSection::resetState()
	{
		_titleWidget->resetState();
		_representationPreset->resetState();

		if ( _representationSettingWidget != nullptr )
		{
			_deleteRepresentationSettingWidget();
		}

		_baseRepresentationIndex = -1;
		_state					 = MultiDataField::State::Uninitialized;
	}

	void RepresentationInspectorSection::updateWithNewValue( const InstantiatedRepresentation & p_representation )
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
				_dummyRepresentation = InstantiatedRepresentation::instantiateCopy( &p_representation );
			}
			else
			{
				MVC::MvcManager::get().deleteView( _dummyRepresentation,
												   ID::View::UI_INSPECTOR_INSTANTIATED_REPRESENTATION );

				_dummyRepresentation->setLinkedRepresentation( p_representation.getLinkedRepresentation() );
				_dummyRepresentation->applyData( p_representation, Model::Representation::MEMBER_FLAG::ALL, false );
			}

			VTX::View::Inspector::InstantiatedRepresentationView * const representationView
				= MVC::MvcManager::get().instantiateView<VTX::View::Inspector::InstantiatedRepresentationView>(
					_dummyRepresentation, ID::View::UI_INSPECTOR_INSTANTIATED_REPRESENTATION );

			representationView->setLinkedInspector( this );

			_instantiateRepresentationSettingWidget(
				p_representation.getLinkedRepresentation()->getRepresentationType() );

			_representationSettingWidget->setRepresentation( _dummyRepresentation );

			_baseRepresentationIndex = baseRepresentationIndex;
		}

		if ( !hasDifferentData() )
		{
			if ( baseRepresentationIndex == _baseRepresentationIndex )
			{
				_representationSettingWidget->updateWithNewValue( p_representation );
				_state = MultiDataField::State::Identical;
			}
			else // Can't managed modifications on multiple representation presets
			{
				_state = MultiDataField::State::Different;
				_deleteRepresentationSettingWidget();
			}
		}

		blockSignals( oldBlockState );
	}
	void RepresentationInspectorSection::_displayDifferentsDataFeedback() {}

} // namespace VTX::UI::Widget::Representation
