#include "inspector_representation_widget.hpp"
#include "action/action_manager.hpp"
#include "action/representable.hpp"
#include "inspector_representation_widget.hpp"
#include "representation/representation_manager.hpp"
#include "style.hpp"
#include "ui/widget_factory.hpp"
#include <QBoxLayout>
#include <QFont>
#include <QGridLayout>
#include <QPixmap>

namespace VTX::UI::Widget::Inspector
{
	InspectorRepresentationWidget::~InspectorRepresentationWidget() {}

	void InspectorRepresentationWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );
		setContentsMargins( 0, 0, 0, 0 );

		QVBoxLayout * const mainLayout = new QVBoxLayout( this );
		mainLayout->setContentsMargins( 0, 0, 0, 0 );
		mainLayout->setSpacing( 1 );

		_mainWidget
			= WidgetFactory::get().instantiateWidget<CustomWidget::CollapsingHeaderWidget>( this, "inspector_item" );
		_mainWidget->setContentsMargins( 0, 0, 0, 0 );
		_mainWidget->setHeaderHeight( Style::INSPECTOR_HEADER_HEIGHT );

		QWidget * const mainContent = new QWidget();
		mainContent->setContentsMargins( 0, 0, 0, 0 );
		QVBoxLayout * const contentLayout = new QVBoxLayout( mainContent );
		contentLayout->setContentsMargins( 0, 0, 0, 0 );

		_targetSection = WidgetFactory::get().instantiateWidget<InspectorSection>( this, "inspector_item_section" );
		_targetsField
			= WidgetFactory::get().instantiateWidget<CustomWidget::RepresentableFieldWidget>( this, "inspector_item" );
		_targetSection->appendField( "Target", _targetsField );

		_colorModeField
			= WidgetFactory::get().instantiateWidget<CustomWidget::ColorModeFieldWidget>( this, "inspector_item" );
		_targetSection->appendField( "Color Mode", _colorModeField );

		contentLayout->addWidget( _targetSection );

		contentLayout->addStretch( 1000 );
		_mainWidget->setBody( mainContent );
		mainLayout->addWidget( _mainWidget );
	}

	void InspectorRepresentationWidget::_setupSlots()
	{
		connect( _targetsField,
				 &CustomWidget::RepresentableFieldWidget::dataChanged,
				 this,
				 &InspectorRepresentationWidget::_onTargetChange );
		connect( _colorModeField,
				 &CustomWidget::ColorModeFieldWidget::colorModeChanged,
				 this,
				 &InspectorRepresentationWidget::_onColorModeChange );
		connect( _colorModeField,
				 &CustomWidget::ColorModeFieldWidget::colorChanged,
				 this,
				 &InspectorRepresentationWidget::_onColorChange );
	};

	void InspectorRepresentationWidget::refresh()
	{
		if ( _model == nullptr )
			return;

		_isRefreshing = true;

		_mainWidget->setHeaderTitle( QString::fromStdString( _model->getName() ) );
		const QPixmap symbolPixmap = Style::IconConst::get().REPRESENTATION_SYMBOL;
		_mainWidget->setHeaderIcon( symbolPixmap );

		// TODO Manage multiple selection
		const std::unordered_set<Generic::BaseRepresentable *> & targets
			= Representation::RepresentationManager::get().getTargets( _model );
		if ( targets.size() > 0 )
			_targetsField->setValue( *targets.begin() );

		_colorModeField->setColorMode( _model->getColorMode() );
		_colorModeField->setColor( _model->getColor() );

		_isRefreshing = false;
	}

	void InspectorRepresentationWidget::_onTargetChange()
	{
		if ( _isRefreshing )
			return;

		Generic::BaseRepresentable * const previousRepresentable = _targetsField->getPreviousRepresentable();
		Generic::BaseRepresentable * const representable		 = _targetsField->getRepresentable();

		if ( previousRepresentable != nullptr )
			Representation::RepresentationManager::get().removeRepresentation( _model, previousRepresentable, false );

		if ( representable != nullptr )
			Representation::RepresentationManager::get().addToRepresentation( _model, representable );

		VTX_INFO( "On Target Change" );
	}
	void InspectorRepresentationWidget::_onColorModeChange()
	{
		if ( _isRefreshing )
			return;

		if ( _model->getColorMode() != _colorModeField->getColorMode() )
			VTX_ACTION( new Action::ChangeRepresentationColorMode( _model, _colorModeField->getColorMode() ) );
	}
	void InspectorRepresentationWidget::_onColorChange()
	{
		if ( _isRefreshing )
			return;

		if ( _model->getColor() != _colorModeField->getColor() )
			VTX_ACTION( new Action::ChangeRepresentationColor( _model, _colorModeField->getColor() ) );
	}

	void InspectorRepresentationWidget::localize() { _targetSection->setHeaderTitle( "TARGETS" ); }

} // namespace VTX::UI::Widget::Inspector
