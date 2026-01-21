#include "app/action/selection.hpp"
#include "app/action/action_manager.hpp"
#include "app/services.hpp"
#include <renderer/renderer.hpp>

namespace VTX::App::Action::Selection
{
	void Clear::execute()
	{
		REG().view<System::Selection>().each(
			[]( const ECS::Entity p_ent, System::Selection & )
			{ ACTION().execute<SetSelected<Scene::E_ITEM::SYSTEM>>( p_ent, Core::Struct::IndexRangeList(), false ); }
		);
	}

	void Clear::execute( const ECS::Entity p_ent )
	{
		ACTION().execute<SetSelected<Scene::E_ITEM::SYSTEM>>( p_ent, Core::Struct::IndexRangeList(), false );
	}

	void Pick::execute( const Vec2i & p_mousePos )
	{
		// Get renderer picking info at mouse position.
		Vec2i picked = RENDERER().getPickedIds( p_mousePos.x, p_mousePos.y );
		VTX_DEBUG( "Renderer picked: {} {}", picked.x, picked.y );
	}
} // namespace VTX::App::Action::Selection
