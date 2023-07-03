#ifndef __VTX_APP_OLD_RENDERER_RT_CONSOLE_PROGRESS_BAR__
#define __VTX_APP_OLD_RENDERER_RT_CONSOLE_PROGRESS_BAR__

#include <mutex>

namespace VTX::App::Old::Render::Renderer::RT
{
	class ConsoleProgressBar
	{
	  public:
		ConsoleProgressBar() = default;

		void start( const int p_nbTasks, const int p_barWidth );
		void next( const int n = 1 );
		void stop();

	  private:
		int _barWidth	 = 50;
		int _nbTasksDone = 0;
		int _nbTasks	 = 1;

		std::mutex _mutex;
	};
} // namespace VTX::App::Old::Render::Renderer::RT
#endif
