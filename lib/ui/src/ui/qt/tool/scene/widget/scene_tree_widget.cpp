#include "ui/qt/tool/scene/widget/scene_tree_widget.hpp"

namespace VTX::UI::QT::Tool::Scene::Widget
{
	SceneTreeWidget::SceneTreeWidget( QWidget * p_parent ) {}

	void SceneTreeWidget::localize() {};

	void SceneTreeWidget::_setupUi( const QString & p_name )
	{
		setColumnCount( 1 );
		QList<QTreeWidgetItem *> items;
		for ( int i = 0; i < 10; ++i )
			items.append( new QTreeWidgetItem(
				static_cast<QTreeWidget *>( nullptr ), QStringList( QString( "item: %1" ).arg( i ) )
			) );
		insertTopLevelItems( 0, items );
	}

	void SceneTreeWidget::_setupSlots() {}

} // namespace VTX::UI::QT::Tool::Scene::Widget
