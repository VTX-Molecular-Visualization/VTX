#ifndef __VTX_APP_ACTION_APPLICATION__
#define __VTX_APP_ACTION_APPLICATION__

#include <util/thread/base_thread.hpp>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Action::Application
{

	/**
	 * @brief Resize the renderer window.
	 */
	struct Resize
	{
		void execute( size_t p_width, size_t p_height, bool p_resizeMainWindow = true );
	};

	/**
	 * @brief Check for application update.
	 */
	struct CheckForUpdate
	{
		void execute();
	};

	/**
	 * @brief Update application.
	 */
	struct Update
	{
		void execute();
	};

	/**
	 * @brief Apply a downloaded update and restart the application.
	 */
	struct RestartAfterUpdate
	{
		void execute();
	};

	/**
	 * @brief Exit the application.
	 */
	struct Quit
	{
		void execute();
	};

	/**
	 * @brief Disable force renderer update.
	 */
	struct SetVSync
	{
		void execute( bool p_enable );
	};

	/**
	 * @brief Disable force renderer update.
	 */
	struct SetSavePower
	{
		void execute( bool p_enable );
	};

	/**
	 * @brief Stop a thread by ID.
	 */
	struct StopThread
	{
		void execute( const Util::Thread::ID & );
	};

} // namespace VTX::App::Action::Application
#endif
