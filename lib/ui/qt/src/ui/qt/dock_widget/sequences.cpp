#include "ui/qt/dock_widget/sequences.hpp"
#include "ui/qt/widget/sequence.hpp"
#include <app/events.hpp>
#include <app/services.hpp>

namespace VTX::UI::QT::DockWidget
{
	Sequences::Sequences( QWidget * p_parent ) : BaseDockWidget( "Sequence", p_parent )
	{
		setAllowedAreas( Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea );

		App::REG().on_construct<Core::Struct::System>().connect<&Sequences::_onConstructSystem>( this );
	}

	void Sequences::_onConstructSystem( App::ECS::Registry & p_r, App::ECS::Entity p_e )
	{
		auto & system		  = p_r.get<Core::Struct::System>( p_e );
		auto * sequenceWidget = new Widget::Sequence( p_e, this );
		_layout->addWidget( sequenceWidget );
	}

} // namespace VTX::UI::QT::DockWidget
