#ifndef __VTX_APP_SYSTEM_WRITER__
#define __VTX_APP_SYSTEM_WRITER__

#include "app/threading/base_thread.hpp"
#include <util/filesystem.hpp>
#include <util/thread.hpp>

namespace VTX::App::System
{
	/**
	 * @brief Responsible for gathering atom selection and writing it into a file.
	 */
	class SelectionWriter
	{
	  public:
		SelectionWriter() = delete;

		/**
		 * @brief Gather selection.
		 * @param
		 */
		SelectionWriter( FilePath );

		/**
		 * @brief Meant to be used as a thread callable. Actually write the file.
		 */
		uint operator()( Util::StopToken, Threading::OptionalThreadReference );

		/**
		 * @brief Stop the execution unit the selection is written
		 */
		void wait() noexcept;

	  private:
		struct _impl;
		std::shared_ptr<_impl> _ptr = nullptr;
	};
} // namespace VTX::App::System

#endif
