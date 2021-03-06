#include "chain_inspector_view.hpp"
#include "action/action_manager.hpp"
#include "representation/representation_manager.hpp"
#include "style.hpp"
#include "ui/widget_factory.hpp"
#include <QBoxLayout>
#include <QFont>
#include <QGridLayout>
#include <QLabel>
#include <QPixmap>

namespace VTX::View::UI::Widget
{
	ChainInspectorView::ChainInspectorView( Model::Chain * const p_chain, QWidget * p_parent ) :
		BaseView( p_chain ), InspectorItemWidget( p_parent ) {};

	ChainInspectorView::~ChainInspectorView() {}

	void ChainInspectorView::notify( const Event::VTXEvent * const p_event )
	{
		if ( p_event->name == Event::Model::DATA_CHANGE )
		{
			_refresh( SectionFlag::INFOS );
		}
		else if ( p_event->name == Event::Model::REPRESENTATION_CHANGE )
		{
			_refresh( SectionFlag::REPRESENTATION );
		}
	}

	void ChainInspectorView::_setupUi( const QString & p_name )
	{
		InspectorItemWidget::_setupUi( p_name );
		setContentsMargins( 0, 0, 0, 0 );

		QVBoxLayout * mainLayout = new QVBoxLayout( this );
		mainLayout->setContentsMargins( 0, 0, 0, 0 );
		mainLayout->setSpacing( 1 );

		_mainWidget = VTX::UI::WidgetFactory::get().instantiateWidget<CollapsingHeaderWidget>( this, "inspector_item" );
		_mainWidget->setContentsMargins( 0, 0, 0, 0 );
		_mainWidget->setHeaderHeight( Style::INSPECTOR_HEADER_HEIGHT );

		QWidget * mainContent = new QWidget();
		mainContent->setContentsMargins( 0, 0, 0, 0 );
		QVBoxLayout * contentLayout = new QVBoxLayout( mainContent );
		contentLayout->setContentsMargins( 0, 0, 0, 0 );

		_representationSection
			= VTX::UI::WidgetFactory::get().instantiateWidget<InspectorSection>( this, "inspector_item_section" );
		_representationWidget
			= VTX::UI::WidgetFactory::get().instantiateWidget<Representation::ItemRepresentationInspector>(
				this, "inspector_instantiated_representation" );
		_representationSection->setBody( _representationWidget );

		_infoSection = VTX::UI::WidgetFactory::get().instantiateWidget<InspectorSectionVLayout>(
			this, "inspector_item_section" );
		_fullnameLabel = new QLabel( this );
		_fullnameLabel->setWordWrap( true );
		_infoSection->appendField( "Full Name", _fullnameLabel );

		_nbResiduesLabel = new QLabel( this );
		_nbResiduesLabel->setWordWrap( true );
		_infoSection->appendField( "Nb Residues", _nbResiduesLabel );

		contentLayout->addWidget( _representationSection );
		contentLayout->addWidget( _infoSection );
		contentLayout->addStretch( 1000 );

		_mainWidget->setBody( mainContent );
		mainLayout->addWidget( _mainWidget );

		blockSignals( true );
		_refresh();
		blockSignals( false );
	}

	void ChainInspectorView::_setupSlots() {};

	void ChainInspectorView::_refresh( const SectionFlag & p_flag )
	{
		if ( _model == nullptr )
			return;

		if ( bool( p_flag & SectionFlag::INFOS ) )
		{
			_mainWidget->setHeaderTitle( QString::fromStdString( _model->getName() ) );

			const QPixmap * symbolPixmap = Style::IconConst::get().getModelSymbol( _model->getTypeId() );
			_mainWidget->setHeaderIcon( *symbolPixmap );

			_fullnameLabel->setText( QString::fromStdString( _model->getName() ) );
			_nbResiduesLabel->setText( QString::fromStdString( std::to_string( _model->getResidueCount() ) ) );
		}

		if ( bool( p_flag & SectionFlag::REPRESENTATION ) )
		{
			_representationWidget->setTarget( _model );
		}
	}

	void ChainInspectorView::localize()
	{
		_representationSection->setHeaderTitle( "Representations" );
		_infoSection->setHeaderTitle( "Infos" );
		_infoSection->localize();
	}

	void ChainInspectorView::setModelEnableFromCheckBox( const int checkboxState )
	{
		_model->setEnable( checkboxState > 0 );
	}

} // namespace VTX::View::UI::Widget
