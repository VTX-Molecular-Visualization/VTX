#ifndef __VTX_APP_CORE_ECS_CONCEPT__
#define __VTX_APP_CORE_ECS_CONCEPT__

#include "base_entity.hpp"
#include <concepts>

namespace VTX::App::Core::ECS
{
	template<typename E>
	concept ECS_Entity = requires( E p_component ) { std::derived_from<E, BaseEntity>; };
	template<typename C>
	concept ECS_Component = requires( C p_component ) { true; };
} // namespace VTX::App::Core::ECS

#endif
