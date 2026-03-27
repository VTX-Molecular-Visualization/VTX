#ifndef __VTX_APP_ACTION_SYSTEM__
#define __VTX_APP_ACTION_SYSTEM__

#include "app/ecs.hpp"
#include <util/types.hpp>

namespace VTX::App::Action::System
{
	/**
	 * @brief Set system position.
	 */
	struct SetPosition
	{
		void execute( const ECS::Entity, const Vec3f & );
	};

	/**
	 * @brief Set system rotation (euler angles).
	 */
	struct SetRotation
	{
		void execute( const ECS::Entity, const Quatf & );
	};

	/**
	 * @brief Set system scale.
	 */
	struct SetScale
	{
		void execute( const ECS::Entity, const Vec3f & );
	};

} // namespace VTX::App::Action::System

#endif
