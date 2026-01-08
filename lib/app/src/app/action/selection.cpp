#include "app/action/selection.hpp"
#include "app/action/action_manager.hpp"

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
} // namespace VTX::App::Action::Selection
