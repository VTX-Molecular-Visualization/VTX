#ifndef VTX_APP_HELPER_AABB_HPP
#define VTX_APP_HELPER_AABB_HPP

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
	Util::Math::AABB compute( Core::Struct::FrameView p_positions ) noexcept;

	/**
	 * @brief Get system AABB.
	 */
	Util::Math::AABB get( Entity p_entity );

	/**
	 * @brief Get scene AABB.
	 * @return
	 */
	Util::Math::AABB getScene();

	/**
	 * @brief Mark system AABB as invalid.
	 */
	void invalidate( Entity p_entity );

	/**
	 * @brief Mark scene AABB as invalid.
	 */
	void invalidateScene();

	/**
	 * @brief Compute selected atoms AABB.
	 */
	Util::Math::AABB get( Entity p_entity, const Util::Math::BitSet & p_atoms );

	/**
	 * @brief Compute topology item ranges AABB.
	 */
	Util::Math::AABB get(
		Entity								 p_entity,
		Core::Struct::E_SYSTEM_ITEM			 p_item,
		const Core::Struct::IndexRangeList & p_ranges
	);

	/**
	 * @brief Compute bond ranges AABB.
	 */
	Util::Math::AABB getBonds( Entity p_entity, const Core::Struct::IndexRangeList & p_ranges );
} // namespace VTX::App::Helper::AABB

#endif
