#ifndef __VTX_APP_SYSTEM_WRITER__
#define __VTX_APP_SYSTEM_WRITER__

#include <memory>
#include <util/filesystem.hpp>

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
		~SelectionWriter();

		void operator()();

	  private:
		struct _impl;
		std::unique_ptr<_impl> _ptr = nullptr;
	};

	/**
	 * @brief Responsible for gathering visible atoms and writing it them into a file.
	 */
	class VisibleWriter
	{
	  public:
		VisibleWriter() = delete;

		/**
		 * @brief Gather visibles.
		 * @param
		 */
		VisibleWriter( FilePath );
		~VisibleWriter();

		void operator()();

	  private:
		struct _impl;
		std::unique_ptr<_impl> _ptr = nullptr;
	};
} // namespace VTX::App::System

#endif
