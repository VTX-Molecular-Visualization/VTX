#include "ui/qt/dock_widget/scene.hpp"
#include "ui/qt/model.hpp"
#include "ui/qt/selection_model.hpp"
#include "ui/qt/services.hpp"

namespace VTX::UI::QT::DockWidget
{

	Scene::Scene( QWidget * p_parent ) : BaseDockWidget( "Scene", p_parent )
	{
		_tree = new Widget::Tree( this );
		_tree->setModel( &MODEL() );
		_tree->setSelectionModel( &SELECTION() );
		_layout->addWidget( _tree );
	}

} // namespace VTX::UI::QT::DockWidget
