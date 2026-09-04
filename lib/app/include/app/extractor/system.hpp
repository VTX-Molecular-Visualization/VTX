#ifndef __VTX_APP_EXTRACTOR_SYSTEM__
#define __VTX_APP_EXTRACTOR_SYSTEM__

#include "app/ecs.hpp"
#include "app/extractor/trajectory_buffer.hpp"
#include <io/constants.hpp>
#include <memory>
#include <string>
#include <util/filesystem.hpp>
#include <util/thread/base_thread.hpp>

namespace VTX::App::Extractor
{
	/**
	 * @brief Extract a molecular system from IO data.
	 */
	class System
	{
	  public:
		System() = delete;
		/**
		 * @brief Extract a system into a new entity
		 * @param p_path
		 */
		System( FilePath, IO::READER_OPTION = IO::READER_OPTION::ALL, TrajectoryBufferSettings = {} );
		/**
		 * @brief Extract a system into a new entity from memory
		 * @param p_path
		 */
		System( FilePath, std::string &&, IO::READER_OPTION = IO::READER_OPTION::ALL, TrajectoryBufferSettings = {} );
		/**
		 * @brief Associate a trajectory to an existing system
		 * @param p_path
		 */
		System( Entity, FilePath, IO::READER_OPTION = IO::READER_OPTION::ALL, TrajectoryBufferSettings = {} );

		/**
		 * @brief Meant to be used as a thread callable. Actually perform the extraction
		 */
		uint operator()( Util::Thread::StopToken, Util::Thread::OptionalThreadReference );

		/**
		 * @brief Stop current execution until the system is extracted.
		 */
		void wait() noexcept;

	  private:
		struct _Data;
		std::shared_ptr<_Data> _attributesPtr;

		void _clean();
	};

} // namespace VTX::App::Extractor
#endif // !__VTX_APP_EXTRACTOR_SYSTEM__
