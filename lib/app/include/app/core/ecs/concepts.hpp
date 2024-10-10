#ifndef __VTX_APP_CORE_ECS_CONCEPT__
#define __VTX_APP_CORE_ECS_CONCEPT__

#include "base_component.hpp"
#include "base_entity.hpp"
#include <concepts>
#include <type_traits>

namespace VTX::App::Core::ECS
{
	template<typename E>
	concept ConceptEntity = std::derived_from<E, BaseEntity>;

	template<typename C>
	concept ConceptComponent = std::derived_from<C, BaseComponent>;

} // namespace VTX::App::Core::ECS

#endif
