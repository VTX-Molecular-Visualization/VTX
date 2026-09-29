#ifndef __VTX_APP_ACTION_SYSTEM__
#define __VTX_APP_ACTION_SYSTEM__

#include "app/ecs.hpp"
#include <util/types.hpp>

namespace VTX::App::Action::System
{
	/**
	 * @brief Set system name.
	 */
	struct SetName
	{
		void execute( Entity, std::string_view );
	};

	/**
	 * @brief Set system position.
	 */
	struct SetPosition
	{
		void execute( Entity, const Vec3f & );
	};

	/**
	 * @brief Set system rotation (euler angles).
	 */
	struct SetRotation
	{
		void execute( Entity, const Quatf & );
	};

	/**
	 * @brief Set system scale.
	 */
	struct SetScale
	{
		void execute( Entity, const Vec3f & );
	};

} // namespace VTX::App::Action::System

#endif
