#ifndef __VTX_APP_ACTION_APPLICATION__
#define __VTX_APP_ACTION_APPLICATION__

#include "app/threading/base_thread.hpp"
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Action::Application
{

	/**
	 * @brief Resize the renderer window.
	 */
	struct Resize
	{
		void execute( const size_t p_width, const size_t p_height );
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
		void execute( const bool p_enable );
	};

	/**
	 * @brief Disable force renderer update.
	 */
	struct SetSavePower
	{
		void execute( const bool p_enable );
	};

	/**
	 * @brief Stop a thread by ID.
	 */
	struct StopThread
	{
		void execute( const Threading::BaseThread::ID & );
	};

} // namespace VTX::App::Action::Application
#endif
