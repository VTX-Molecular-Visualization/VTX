#include "ui/qt/widget/scene/tree.hpp"

namespace VTX::UI::QT::Widget::Scene
{
	Tree::Tree( QWidget * p_parent ) {}

	void Tree::localize() {};

	void Tree::_setupUi( const QString & p_name )
	{
		setColumnCount( 1 );
		QList<QTreeWidgetItem *> items;
		for ( int i = 0; i < 10; ++i )
			items.append( new QTreeWidgetItem(
				static_cast<QTreeWidget *>( nullptr ), QStringList( QString( "item: %1" ).arg( i ) )
			) );
		insertTopLevelItems( 0, items );

		/*
		MAIN_REGISTRY().connectSignal<Component::Chemistry::Molecule, &ProxyMolecule::_removeFromRenderer>(
			Core::ECS::SIGNAL::DESTROY, this
		);
		*/
	}

	void Tree::_setupSlots() {}

} // namespace VTX::UI::QT::Widget::Scene
