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

					_mainWidget = WidgetFactory::get().instanciateWidget<CustomWidget::CollapsingHeaderWidget>( this, "inspector_item" );
					_mainWidget->setContentsMargins( 0, 0, 0, 0 );
					_mainWidget->setHeaderHeight( Style::INSPECTOR_HEADER_HEIGHT );

					QWidget * const mainContent = new QWidget();
					mainContent->setContentsMargins( 0, 0, 0, 0 );
					QVBoxLayout * const contentLayout = new QVBoxLayout( mainContent );
					contentLayout->setContentsMargins( 0, 0, 0, 0 );

					_targetSection = WidgetFactory::get().instanciateWidget<InspectorSection>( this, "inspector_item_section" );
					_targetsField  = WidgetFactory::get().instanciateWidget<CustomWidget::RepresentableFieldWidget>( this, "inspector_item" );
					_targetSection->appendField( "Target", _targetsField );

					_colorModeField = WidgetFactory::get().instanciateWidget<CustomWidget::ColorModeFieldWidget>( this, "inspector_item" );
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

					_isRefreshing = true;

					_mainWidget->setHeaderTitle( QString::fromStdString( _model->getName() ) );
					const QPixmap symbolPixmap = Style::IconConst::get().REPRESENTATION_SYMBOL;
					_mainWidget->setHeaderIcon( symbolPixmap );

					// TODO Manage multiple selection
					const std::unordered_set<Generic::BaseRepresentable *> & targets = Representation::RepresentationManager::get().getTargets( _model );
					if ( targets.size() > 0 )
						_targetsField->setValue( *targets.begin() );
					_colorModeField->setColorMode( _model->getColorMode() );
					_isRefreshing = false;
				}

				void InspectorRepresentationWidget::_onTargetChange()
				{
					if ( _isRefreshing )
						return;

					Generic::BaseRepresentable * const previousRepresentable = _targetsField->getPreviousRepresentable();
					Generic::BaseRepresentable * const representable		 = _targetsField->getRepresentable();

					if ( previousRepresentable != nullptr )
						Representation::RepresentationManager::get().removeRepresentation( _model, previousRepresentable );

					if ( representable != nullptr )
						Representation::RepresentationManager::get().addToRepresentation( _model, representable );

					VTX_INFO( "On Target Change" );
				}
				void InspectorRepresentationWidget::_onColorModeChange()
				{
					if ( _isRefreshing )
						return;

					VTX_ACTION( new Action::ChangeRepresentationColorMode( _model, _colorModeField->getColorMode() ) );
				}

				void InspectorRepresentationWidget::localize() { _targetSection->setHeaderTitle( "TARGETS" ); }

			} // namespace Inspector
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
