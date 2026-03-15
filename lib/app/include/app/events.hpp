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
		std::string currentVersion;
		std::string newVersion;
		std::string changelogMd;
		std::string changelogHtml;
		size_t		size;
	};

	struct UpdateDownloadProgress
	{
		float progress;
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
		bool  rendered;
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
	struct CameraTransformChange
	{
	};

	struct CameraControllerChange
	{
		uint type;
	};

	/**
	 * @brief System.
	 * Used instead of on_construct to avoid component creation order problems.
	 */
	struct SystemLoad
	{
		ECS::Entity system;
	};

	template<typename P>
	struct PresetRename
	{
		ECS::Entity preset;
		std::string name;
	};

} // namespace VTX::App::Events

#endif
