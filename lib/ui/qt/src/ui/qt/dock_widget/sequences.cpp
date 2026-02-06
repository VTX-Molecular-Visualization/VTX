#include "ui/qt/dock_widget/sequences.hpp"
#include "app/scene/color_layout.hpp"
#include "ui/qt/services.hpp"
#include <app/events.hpp>
#include <app/services.hpp>
#include <app/system/selection.hpp>
#include <renderer/color.hpp>
#include <util/event_hub.hpp>

namespace VTX::UI::QT::DockWidget
{
	Sequences::Sequences( QWidget * p_parent ) : BaseDockWidget( p_parent, "Sequence" )
	{
		setAllowedAreas( Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea );

		App::HUB().connect<App::Events::SystemLoad, &Sequences::_onSystemLoad>( this );
		App::REG().on_destroy<Core::Struct::System>().connect<&Sequences::_onSystemDestroy>( this );

		// Refresh widget when selection changed.
		App::REG().on_update<App::System::Selection>().connect<&Sequences::_onUpdateSelection>( this );

		// Refresh widget when colors changed.
		App::REG().on_construct<App::Scene::ColorLayout>().connect<&Sequences::_onColorsChanged>( this );
		App::REG().on_update<App::Scene::ColorLayout>().connect<&Sequences::_onColorsChanged>( this );
		App::REG().on_update<Renderer::Color::Layout>().connect<&Sequences::_onColorsPresetChanged>( this );
	}

	void Sequences::_onSystemLoad( const App::Events::SystemLoad & p_e )
	{
		const auto entity		  = p_e.system;
		auto *	   sequenceWidget = new Widget::Sequence( entity, this );

		// Create Widget.
		assert( not _mapSequenceWidgets.contains( entity ) );
		_mapSequenceWidgets.emplace( entity, sequenceWidget );
		_layout->addWidget( sequenceWidget );
	}

	void Sequences::_onSystemDestroy( App::ECS::Registry & p_r, App::ECS::Entity p_e )
	{
		// Remove from map and delete widget.
		assert( _mapSequenceWidgets.contains( p_e ) );
		_mapSequenceWidgets[ p_e ]->deleteLater();
		_mapSequenceWidgets.erase( p_e );
	}

	void Sequences::_onUpdateSelection( App::ECS::Registry &, const App::ECS::Entity p_e )
	{
		assert( _mapSequenceWidgets.contains( p_e ) );
		_mapSequenceWidgets[ p_e ]->viewport()->update();
	}

	void Sequences::_onColorsChanged( App::ECS::Registry &, const App::ECS::Entity p_e )
	{
		for ( auto & [ _, w ] : _mapSequenceWidgets )
		{
			w->viewport()->update();
		}
	}

	void Sequences::_onColorsPresetChanged( App::ECS::Registry & p_reg, const App::ECS::Entity p_e )
	{
		const auto & colorLayout = App::ECS::getFirstComponent<App::Scene::ColorLayout>();

		if ( colorLayout.preset == p_e )
		{
			for ( auto & [ _, w ] : _mapSequenceWidgets )
			{
				w->viewport()->update();
			}
		}
	}

} // namespace VTX::UI::QT::DockWidget
