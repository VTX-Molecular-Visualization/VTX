#ifndef __VTX_APP_ACTION_APPLICATION__
#define __VTX_APP_ACTION_APPLICATION__

#include <util/types.hpp>
#include <vector>

namespace VTX::App::Action::Application
{

	/**
	 * @brief Exit the application.
	 */
	struct Quit
	{
		void execute();
	};

	/**
	 * @brief Resize the renderer window.
	 */
	struct Resize
	{
		void execute( const size_t p_width, const size_t p_height );
	};

	/**
	 * @brief Disable force renderer update.
	 */
	struct SetSavePower
	{
		void execute( const bool p_enable );
	};

	/**
	 * @brief Update application.
	 */
	struct Update
	{
		void execute();
	};

} // namespace VTX::App::Action::Application
#endif
