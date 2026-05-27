#ifndef __VTX_APP_SYSTEM_LOAD__
#define __VTX_APP_SYSTEM_LOAD__

#include "app/system/trajectory.hpp"
#include "app/threading/base_thread.hpp"
#include <atomic>
#include <core/struct/topology.hpp>
#include <functional>
#include <future>
#include <io/metadata.hpp>
#include <io/reader.hpp>
#include <latch>
#include <optional>
#include <util/thread.hpp>
#include <variant>

namespace VTX::App::System
{
	/**
	 * @brief Datastruct that will hold data while the system is being filled
	 */
	struct PendingSystem
	{
		std::optional<Entity> entity;				   // Used for attaching a trajectory to an existing system
		bool					   onlyTrajectory = false; // Set to true when the goal is only to set a new trajectory
		FilePath				   sourcePath;
		IO::READER_OPTION		   readerOption;
		std::optional<std::string> buffer;
		std::optional<IO::SystemReader> reader;
		Core::Struct::Topology			topology;
		IO::Metadata					metadata;
		// Two pass on the system : when the topology is ready, the decision of what kind of trajectory to have in made
		// on the main loop. Then, the trajectory is red asynchronously. Then, when trajectory is ready, the main loop
		// proceed with the system creation.
		std::atomic_bool readyToDeliver { false };

		std::variant<System::TrajectorySingleFrame, System::TrajectoryFullBuffer> trajectoryData;
	};

	/**
	 * @brief Responsible for extracting data from the IO reader to the PendingSystem datastruct. Copies of this object
	 * will actually copy a reference.
	 */
	class SystemExtractor
	{
	  public:
		SystemExtractor() = delete;
		/**
		 * @brief Extract a system into a new entity
		 * @param p_path
		 */
		SystemExtractor( FilePath p_path, IO::READER_OPTION = IO::READER_OPTION::ALL );
		/**
		 * @brief Extract a system into a new entity from memory
		 * @param p_path
		 */
		SystemExtractor( FilePath p_path, std::string && p_buffer, IO::READER_OPTION = IO::READER_OPTION::ALL );
		/**
		 * @brief Associate a trajectory to an existing system
		 * @param p_path
		 */
		SystemExtractor( Entity p_entity, FilePath p_path, IO::READER_OPTION = IO::READER_OPTION::ALL );

		/**
		 * @brief Meant to be used as a thread callable. Actually perform the extraction
		 */
		uint operator()( Util::StopToken, Threading::OptionalThreadReference );

		/**
		 * @brief Stop current execution until the system is extracted.
		 */
		void wait() noexcept;

	  private:
		struct _Data;
		std::shared_ptr<_Data> _attributesPtr;

		void _clean();
	};

} // namespace VTX::App::System
#endif
