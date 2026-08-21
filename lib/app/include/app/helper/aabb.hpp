#ifndef __VTX_APP_HELPER_AABB__
#define __VTX_APP_HELPER_AABB__

#include "app/ecs.hpp"
#include <core/struct/topology.hpp>
#include <core/struct/trajectory.hpp>
#include <util/math/aabb.hpp>
#include <util/math/bitset.hpp>

namespace VTX::App::Helper::AABB
{
	/**
	 * @brief Compute an atom AABB from positions.
	 */
	Util::Math::AABB compute( const Core::Struct::FrameView p_positions ) noexcept;

	/**
	 * @brief Get cached system AABB.
	 */
	Util::Math::AABB get( const Entity p_entity );

	/**
	 * @brief Compute selected atoms AABB.
	 */
	Util::Math::AABB get( const Entity p_entity, const Util::Math::BitSet & p_atoms );

	/**
	 * @brief Compute topology item ranges AABB.
	 */
	Util::Math::AABB get(
		const Entity						 p_entity,
		const Core::Struct::E_SYSTEM_ITEM	 p_item,
		const Core::Struct::IndexRangeList & p_ranges
	);

	/**
	 * @brief Compute bond ranges AABB.
	 */
	Util::Math::AABB getBonds( const Entity p_entity, const Core::Struct::IndexRangeList & p_ranges );
} // namespace VTX::App::Helper::AABB

#endif
