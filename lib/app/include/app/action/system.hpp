#pragma once

#include "app/ecs.hpp"
#include <util/types.hpp>

namespace VTX::App::Action::System
{
	/**
	 * @brief Set system name.
	 */
	struct SetName
	{ void execute( Entity, std::string_view ); };

	/**
	 * @brief Set system position.
	 */
	struct SetPosition
	{ void execute( Entity, const Vec3f & ); };

	/**
	 * @brief Set system rotation (euler angles).
	 */
	struct SetRotation
	{ void execute( Entity, const Quatf & ); };

	/**
	 * @brief Set system scale.
	 */
	struct SetScale
	{
		/**
		 * Clamp scale.
		 */
		static constexpr float SCALE_MIN = 0.001f;
		static constexpr float SCALE_MAX = 1000.f;

		void execute( Entity, const Vec3f & );
	};

} // namespace VTX::App::Action::System
