#include "qt/widget/scene/panel.hpp"
#include "qt/widget_factory.hpp"
#include <QVBoxLayout>
#include <iostream>

namespace VTX::UI::QT::Widget::Scene
{
	Panel::Panel( QWidget * p_parent ) {}

	void Panel::localize() { setWindowTitle( "Scene" ); };

	void Panel::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		QWidget * const mainWidget = _instantiateMainWidget( QSize( 200, 0 ), QSize( 200, 0 ) );

		_treeWidget = WidgetFactory::get().instantiateWidget<Tree>( this, "_treeWidget" );

		QVBoxLayout * const mainLayout = new QVBoxLayout( mainWidget );
		mainLayout->setContentsMargins( 0, 0, 0, 0 );
		mainLayout->addWidget( _treeWidget );

		setWidget( mainWidget );
	}

	void Panel::_setupSlots() {}

} // namespace VTX::UI::QT::Widget::Scene
