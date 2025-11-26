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
	 */
	struct SystemLoad
	{
		ECS::Entity system;
	};

	struct SelectionChange
	{
		ECS::Entity system;
	};

	/**
	 * @brief Libraries.
	 */
	struct LibraryPresetAdd
	{
		std::string name;
	};

	struct LibraryPresetDelete
	{
		std::string name;
	};

	struct LibraryPresetRename
	{
		std::string oldName;
		std::string newName;
	};

} // namespace VTX::App::Events

#endif
