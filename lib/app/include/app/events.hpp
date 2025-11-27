#ifndef __VTX_APP_EVENTS__
#define __VTX_APP_EVENTS__

#include "app/ecs.hpp"
#include <string>

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
	template<typename T>
	struct CameraControllerChange
	{
	};

	template<int T>
	struct CameraProjectionChange
	{
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

} // namespace VTX::App::Events

#endif
