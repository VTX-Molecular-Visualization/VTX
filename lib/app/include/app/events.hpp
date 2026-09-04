#ifndef __VTX_APP_EVENTS__
#define __VTX_APP_EVENTS__

#include "app/controller/animation.hpp"
#include "app/ecs.hpp"
#include <core/struct/trajectory.hpp>
#include <string>
#include <thread>
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
		std::string changelog;
		size_t		size;
	};

	struct UpdateDownloadProgress
	{
		uint progress;
	};

	struct UpdateReadyToRestart
	{
	};

	struct UpdateDownloadFailed
	{
		std::string message;
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

	struct RendererResize
	{
		size_t width;
		size_t height;
		bool   resizeMainWindow;
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
	 * @brief Camera.
	 */
	struct CameraTransformChange
	{
	};

	/**
	 * @brief Start a camera animation.
	 */
	struct CameraAnimationStart
	{
		Controller::AnimationData	   start;
		Controller::AnimationData	   end;
		float						   duration;
		Controller::InterpPositionFunc interpPosition;
		Controller::InterpRotationFunc interpRotation;
	};

	/**
	 * @brief System.
	 * Used instead of on_construct to avoid component creation order problems.
	 */
	struct SystemLoad
	{
		Entity system;
	};

	struct MeshLoad
	{
		Entity mesh;
	};

	struct TrajectoryCurrentFrameChange
	{
		Entity					system;
		Core::Struct::FrameView frame;
	};

	template<typename P>
	struct PresetRename
	{
		Entity		preset;
		std::string name;
	};

	struct ThreadProgress
	{
		std::jthread::id id;
		float			 progress;
		std::string		 text;
	};

	struct ThreadTerminated
	{
		std::jthread::id id;
		bool			 manuallyStopped;
	};

	struct PythonInitialized
	{
		std::string version;
	};

	struct ViewPointRenamed
	{
		Entity		viewpoint;
		std::string name;
	};

} // namespace VTX::App::Events

#endif
