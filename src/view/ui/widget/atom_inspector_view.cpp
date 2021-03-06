#include "atom_inspector_view.hpp"
#include "action/action_manager.hpp"
#include "action/transformable.hpp"
#include "representation/representation_manager.hpp"
#include "style.hpp"
#include "ui/widget_factory.hpp"
#include "view/ui/widget/representation/instantiated_representation_inspector_view.hpp"
#include <QBoxLayout>
#include <QFont>
#include <QGridLayout>
#include <QLabel>
#include <QPixmap>

namespace VTX::View::UI::Widget
{
	AtomInspectorView::AtomInspectorView( Model::Atom * const p_atom, QWidget * p_parent ) :
		BaseView( p_atom ), InspectorItemWidget( p_parent ) {};

	AtomInspectorView::~AtomInspectorView() {}

	void AtomInspectorView::notify( const Event::VTXEvent * const p_event )
	{
		if ( p_event->name == Event::Model::DATA_CHANGE )
		{
			_refresh( SectionFlag::INFOS );
		}
	}

	void AtomInspectorView::_setupUi( const QString & p_name )
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

		_infoSection = VTX::UI::WidgetFactory::get().instantiateWidget<InspectorSectionVLayout>(
			this, "inspector_item_section" );
		_fullnameLabel = new QLabel( this );
		_fullnameLabel->setWordWrap( true );
		_infoSection->appendField( "Full Name", _fullnameLabel );

		contentLayout->addWidget( _infoSection );
		contentLayout->addStretch( 1000 );

		_mainWidget->setBody( mainContent );
		mainLayout->addWidget( _mainWidget );

		blockSignals( true );
		_refresh();
		blockSignals( false );
	}

	void AtomInspectorView::_setupSlots() {};

	void AtomInspectorView::_refresh( const SectionFlag & p_flag )
	{
		if ( _model == nullptr )
			return;

		if ( bool( p_flag & SectionFlag::INFOS ) )
		{
			_mainWidget->setHeaderTitle( QString::fromStdString( _model->getSymbolName() ) );

			const QPixmap * symbolPixmap = Style::IconConst::get().getModelSymbol( _model->getTypeId() );
			_mainWidget->setHeaderIcon( *symbolPixmap );

			_fullnameLabel->setText( QString::fromStdString( _model->getName() ) );
		}
	}

	void AtomInspectorView::localize()
	{
		_infoSection->setHeaderTitle( "Infos" );
		_infoSection->localize();
	}
} // namespace VTX::View::UI::Widget
