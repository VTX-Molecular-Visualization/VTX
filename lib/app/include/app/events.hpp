#ifndef __VTX_APP_EVENTS__
#define __VTX_APP_EVENTS__

#include "app/ecs.hpp"
#include <string>
#include <util/types.hpp>

namespace VTX::App::Events
{
	/**
	 * @brief Application.
	 */
	struct ApplicationStart
	{
	};

	struct ApplicationStop
	{
	};

	struct UpdateAvailable
	{
		uint major;
		uint minor;
		uint patch;
	};

	struct ApplicationError
	{
		std::string message;
	};

	struct Update
	{
		float delta;
		float elapsed;
	};

	struct PostUpdate
	{
		float delta;
		float elapsed;
	};

	struct PostRender
	{
		float delta;
		float elapsed;
	};

	struct BlockingOperationStart
	{
		std::string message;
	};

	struct BlockingOperationEnd
	{
	};

	struct BlockingOperationProgress
	{
		float progress;
	};

	/**
	 * @brief Camera / controllers.
	 */
	struct CameraControllerChange
	{
		uint type;
	};

	struct CameraAnimationEnd
	{
	};

	/**
	 * @brief System.
	 * Used instead of on_construct to avoid component creation order problems.
	 */
	struct SystemLoad
	{
		ECS::Entity system;
	};

	/**
	 * @brief Event triggered when a trajectory file is submitted to vtx while at least one system is already loaded.
	 */
	struct TrajectoryFileAssociation
	{
		FilePath path;
	};

	struct PresetRename
	{
		ECS::Entity preset;
		std::string name;
	};

} // namespace VTX::App::Events

#endif
