#ifndef __VTX_APP_CORE_ECS_CONCEPT__
#define __VTX_APP_CORE_ECS_CONCEPT__

#include "base_component.hpp"
#include "base_entity.hpp"
#include <concepts>
#include <type_traits>

namespace VTX::App::Core::ECS
{
	template<typename E>
	concept ECS_Entity = std::derived_from<E, BaseEntity>;

	template<typename C>
	concept ECS_Component = requires() { !std::is_pointer<C>(); };
	// concept ECS_Component = std::derived_from<C, BaseComponent>;

} // namespace VTX::App::Core::ECS

#endif
