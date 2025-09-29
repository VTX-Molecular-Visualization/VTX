#ifndef __VTX_APP_EVENTS__
#define __VTX_APP_EVENTS__

namespace VTX::App::Events
{
	struct ApplicationStarted
	{
	};

	struct ApplicationStopped
	{
	};

	struct FrameStarted
	{
	};

	struct PreUpdate
	{
		float delta;
		float elapsed;
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

	struct Render
	{
		float delta;
		float elapsed;
	};

	struct PostRender
	{
		float delta;
		float elapsed;
	};

	struct FrameEnded
	{
	};

	struct BlockingOperationStarted
	{
		std::string message;
	};

	struct BlockingOperationEnded
	{
	};

	struct BlockingOperationProgress
	{
		float progress;
	};
} // namespace VTX::App::Events

#endif
