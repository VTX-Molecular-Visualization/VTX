#ifndef __VTX_APP_COMPONENT_SCENE_UID_COMPONENT__
#define __VTX_APP_COMPONENT_SCENE_UID_COMPONENT__

#include "app/core/ecs/ecs_system.hpp"
#include "app/core/ecs/base_component.hpp"
#include "app/core/uid/uid.hpp"
#include <util/math/range_list.hpp>

namespace VTX::App::Component::Scene
{
	class UIDComponent : public Core::ECS::BaseComponent
	{
	  public:
		UIDComponent() = default;
		UIDComponent( const App::Core::UID::uid & p_uid );
		UIDComponent( const Core::UID::UIDRange & p_range );

		~UIDComponent() = default;

		void referenceUID( const App::Core::UID::uid & p_uid );
		void referenceUID( const Core::UID::UIDRange & p_range );

		bool contains( const App::Core::UID::uid & p_uid ) const;
		bool contains( const App::Core::UID::UIDRange & p_uidRange ) const;

	  private:
		Util::Math::RangeList<App::Core::UID::uid> _uids = Util::Math::RangeList<App::Core::UID::uid>();
	};
} // namespace VTX::App::Component::Scene
#endif
