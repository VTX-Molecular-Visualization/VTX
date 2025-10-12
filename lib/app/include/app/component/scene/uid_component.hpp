#ifndef __VTX_APP_COMPONENT_SCENE_UID_COMPONENT__
#define __VTX_APP_COMPONENT_SCENE_UID_COMPONENT__

#include "app/core/ecs/ecs_system.hpp"
#include "app/core/ecs/base_component.hpp"
#include "app/uid/uid.hpp"
#include <util/math/range_list.hpp>

namespace VTX::App::Component::Scene
{
	class UIDComponent : public Core::ECS::BaseComponent
	{
	  public:
		UIDComponent() = default;
		UIDComponent( const App::Uid::uid & p_uid );
		UIDComponent( const Uid::UIDRange & p_range );

		~UIDComponent() = default;

		void referenceUID( const App::Uid::uid & p_uid );
		void referenceUID( const Uid::UIDRange & p_range );

		bool contains( const App::Uid::uid & p_uid ) const;
		bool contains( const App::Uid::UIDRange & p_uidRange ) const;

	  private:
		Util::Math::RangeList<App::Uid::uid> _uids = Util::Math::RangeList<App::Uid::uid>();
	};
} // namespace VTX::App::Component::Scene
#endif
