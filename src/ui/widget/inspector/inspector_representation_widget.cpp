#include "inspector_representation_widget.hpp"
#include "representation/representation_manager.hpp"
#include "style.hpp"
#include "ui/widget_factory.hpp"
#include <QBoxLayout>
#include <QFont>
#include <QGridLayout>
#include <QPixmap>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Inspector
			{
				InspectorRepresentationWidget::~InspectorRepresentationWidget() {}

				void InspectorRepresentationWidget::_setupUi( const QString & p_name )
				{
					BaseManualWidget::_setupUi( p_name );
					setContentsMargins( 0, 0, 0, 0 );

					QVBoxLayout * const mainLayout = new QVBoxLayout( this );
					mainLayout->setContentsMargins( 0, 0, 0, 0 );
					mainLayout->setSpacing( 1 );

					_mainWidget = WidgetFactory::get().getWidget<CustomWidget::CollapsingHeaderWidget>( this, "inspector_item" );
					_mainWidget->setContentsMargins( 0, 0, 0, 0 );
					_mainWidget->setHeaderHeight( Style::INSPECTOR_HEADER_HEIGHT );

					QWidget * const mainContent = new QWidget();
					mainContent->setContentsMargins( 0, 0, 0, 0 );
					QVBoxLayout * const contentLayout = new QVBoxLayout( mainContent );
					contentLayout->setContentsMargins( 0, 0, 0, 0 );

					_targetSection = WidgetFactory::get().getWidget<InspectorSection>( this, "inspector_item_section" );
					_targetsField  = WidgetFactory::get().getWidget<CustomWidget::RepresentableFieldWidget>( this, "inspector_item" );
					_targetSection->appendField( "Target", _targetsField );

					_colorModeField = WidgetFactory::get().getWidget<CustomWidget::ColorModeFieldWidget>( this, "inspector_item" );
					_targetSection->appendField( "Color Mode", _colorModeField );

					contentLayout->addWidget( _targetSection );

					contentLayout->addStretch( 1000 );
					_mainWidget->setBody( mainContent );
					mainLayout->addWidget( _mainWidget );
				}

				void InspectorRepresentationWidget::_setupSlots()
				{
					connect( _targetsField, &CustomWidget::RepresentableFieldWidget::dataChanged, this, &InspectorRepresentationWidget::_onTargetChange );
					connect( _colorModeField, &CustomWidget::ColorModeFieldWidget::dataChanged, this, &InspectorRepresentationWidget::_onColorModeChange );
				};

				void InspectorRepresentationWidget::refresh()
				{
					if ( _model == nullptr )
						return;

					_mainWidget->setHeaderTitle( QString::fromStdString( _model->getName() ) );
					const QPixmap symbolPixmap = Style::IconConst::get().REPRESENTATION_SYMBOL;
					_mainWidget->setHeaderIcon( symbolPixmap );

					_colorModeField->setColorMode( _model->getColorMode() );
				}

				void InspectorRepresentationWidget::_onTargetChange()
				{
					Generic::BaseRepresentable * const representable = _targetsField->getRepresentable();
					Representation::RepresentationManager::get().addRepresentation( _model, representable );

					VTX_INFO( "On Target Change" );
				}
				void InspectorRepresentationWidget::_onColorModeChange() { VTX_INFO( "On Target Change" ); }

				void InspectorRepresentationWidget::localize() { _targetSection->setHeaderTitle( "TARGETS" ); }

			} // namespace Inspector
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
