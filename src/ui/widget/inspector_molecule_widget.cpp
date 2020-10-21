#include "inspector_molecule_widget.hpp"
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
			InspectorMoleculeWidget::~InspectorMoleculeWidget() {}

			void InspectorMoleculeWidget::_setupUi( const QString & p_name )
			{
				BaseManualWidget::_setupUi( p_name );
				setContentsMargins( 0, 0, 0, 0 );

				QVBoxLayout * mainLayout = new QVBoxLayout( this );
				mainLayout->setContentsMargins( 0, 0, 0, 0 );
				mainLayout->setSpacing( 1 );

				_mainWidget = WidgetFactory::get().GetWidget<CollapsingHeaderWidget>( this, "inspector_item" );
				_mainWidget->setContentsMargins( 0, 0, 0, 0 );
				_mainWidget->setHeaderHeight( Style::INSPECTOR_HEADER_HEIGHT );

				QWidget * mainContent = new QWidget();
				mainContent->setContentsMargins( 0, 0, 0, 0 );
				QVBoxLayout * contentLayout = new QVBoxLayout( mainContent );
				contentLayout->setContentsMargins( 0, 0, 0, 0 );

				_infoSection = WidgetFactory::get().GetWidget<CollapsingHeaderWidget>( this, "inspector_item_section" );
				_infoSection->setHeaderHeight( Style::INSPECTOR_HEADER_HEIGHT );
				_infoSection->displayIconInHeader( false );

				QWidget * infoSectionContent = new QWidget();
				infoSectionContent->setContentsMargins( 4, 0, 4, 0 );

				QGridLayout * infoSectionContentLayout = new QGridLayout( infoSectionContent );
				infoSectionContentLayout->setColumnStretch( 1, 100 );
				infoSectionContentLayout->setContentsMargins( 0, 0, 0, 0 );

				for ( int i = 0; i < (int)InfoFields::COUNT; i++ )
					_infoSectionFields[ i ].addInGrid( i, *infoSectionContentLayout );

				_infoSection->setBody( infoSectionContent );

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

				_infoSectionFields[ (int)InfoFields::FULL_NAME ].setValue( _model->getName() );
				_infoSectionFields[ (int)InfoFields::ATOM_COUNT ].setValue( std::to_string( _model->getAtomCount() ) );
			}

			void InspectorMoleculeWidget::localize()
			{
				_infoSection->setHeaderTitle( "INFOS" );
				for ( int i = 0; i < (int)InfoFields::COUNT; i++ )
					_infoSectionFields[ i ].localize();
			}

		} // namespace Widget
	}	  // namespace UI
} // namespace VTX
