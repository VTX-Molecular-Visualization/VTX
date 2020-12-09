#include "inspector_molecule_widget.hpp"
#include "style.hpp"
#include "ui/widget_factory.hpp"
#include <QBoxLayout>
#include <QFont>
#include <QGridLayout>
#include <QLabel>
#include <QPixmap>

namespace VTX
{
	namespace UI
	{
		namespace Widget
		{
			namespace Inspector
			{
				InspectorMoleculeWidget::~InspectorMoleculeWidget() {}

				void InspectorMoleculeWidget::_setupUi( const QString & p_name )
				{
					BaseManualWidget::_setupUi( p_name );
					setContentsMargins( 0, 0, 0, 0 );

					QVBoxLayout * mainLayout = new QVBoxLayout( this );
					mainLayout->setContentsMargins( 0, 0, 0, 0 );
					mainLayout->setSpacing( 1 );

					_mainWidget = WidgetFactory::get().instanciateWidget<CustomWidget::CollapsingHeaderWidget>( this, "inspector_item" );
					_mainWidget->setContentsMargins( 0, 0, 0, 0 );
					_mainWidget->setHeaderHeight( Style::INSPECTOR_HEADER_HEIGHT );

					QWidget * mainContent = new QWidget();
					mainContent->setContentsMargins( 0, 0, 0, 0 );
					QVBoxLayout * contentLayout = new QVBoxLayout( mainContent );
					contentLayout->setContentsMargins( 0, 0, 0, 0 );

					_infoSection   = WidgetFactory::get().instanciateWidget<InspectorSection>( this, "inspector_item_section" );
					_fullnameLabel = new QLabel( this );
					_infoSection->appendField( "Full Name", _fullnameLabel );
					_nbAtomsLabel = new QLabel( this );
					_infoSection->appendField( "Nb Atoms", _nbAtomsLabel );

					contentLayout->addWidget( _infoSection );
					contentLayout->addStretch( 1000 );

					_mainWidget->setBody( mainContent );
					mainLayout->addWidget( _mainWidget );
				}

				void InspectorMoleculeWidget::_setupSlots() {};

				void InspectorMoleculeWidget::refresh()
				{
					if ( _model == nullptr )
						return;

					_mainWidget->setHeaderTitle( QString::fromStdString( _model->getDefaultName() ) );
					const QPixmap * symbolPixmap = Style::IconConst::get().getModelSymbol( _model->getTypeId() );
					_mainWidget->setHeaderIcon( *symbolPixmap );

					_fullnameLabel->setText( QString::fromStdString( _model->getName() ) );
					_nbAtomsLabel->setText( QString::fromStdString( std::to_string( _model->getAtomCount() ) ) );
				}

				void InspectorMoleculeWidget::localize() { _infoSection->localize(); }

			} // namespace Inspector
		}	  // namespace Widget
	}		  // namespace UI
} // namespace VTX
