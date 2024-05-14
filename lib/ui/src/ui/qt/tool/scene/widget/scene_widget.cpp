#include "ui/qt/tool/scene/widget/scene_widget.hpp"
#include "ui/qt/widget_factory.hpp"
#include <QVBoxLayout>
#include <iostream>

namespace VTX::UI::QT::Tool::Scene::Widget
{
	SceneWidget::SceneWidget( QWidget * p_parent ) {}

	void SceneWidget::localize() { setWindowTitle( "Scene" ); };

	void SceneWidget::_setupUi( const QString & p_name )
	{
		BaseManualWidget::_setupUi( p_name );

		QWidget * const mainWidget = _instantiateMainWidget( QSize( 200, 0 ), QSize( 200, 0 ) );

		_treeWidget = WidgetFactory::get().instantiateWidget<SceneTreeWidget>( this, "_treeWidget" );

		QVBoxLayout * const mainLayout = new QVBoxLayout( mainWidget );
		mainLayout->setContentsMargins( 0, 0, 0, 0 );
		mainLayout->addWidget( _treeWidget );

		setWidget( mainWidget );
	}

	void SceneWidget::_setupSlots() {}

} // namespace VTX::UI::QT::Tool::Scene::Widget
