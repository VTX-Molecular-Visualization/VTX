#ifndef VTX_APP_ACTION_IO_HPP
#define VTX_APP_ACTION_IO_HPP

#include "app/ecs.hpp"
#include <optional>
#include <util/image.hpp>
#include <util/thread/base_thread.hpp>
#include <util/types.hpp>

namespace VTX::App::Action::IO
{

	/**
	 * @brief Open a file or folder with the system default application.
	 */
	struct Open
	{
		void execute( FilePath p_path );
		void execute( const std::string & p_path );
	};

	/**
	 * @brief Load a molecular system from disk or buffer.
	 */
	struct LoadSystem
	{
		LoadSystem() = default;
		LoadSystem( Util::Thread::ThreadData );

		void execute( FilePath p_path ) const;
		void execute( FilePath p_path, std::string && p_buffer ) const;

	  private:
		Util::Thread::ThreadData _threadData;
	};

	/**
	 * @brief Load a triangle mesh from disk.
	 */
	struct LoadMesh
	{
		LoadMesh() = default;
		LoadMesh( Util::Thread::ThreadData );

		void execute( FilePath p_path );

	  private:
		Util::Thread::ThreadData _threadData;
	};

	/**
	 * @brief Write a file containing atoms from selection
	 */
	struct WriteSelection
	{
		void execute( FilePath p_path );
	};

	/**
	 * @brief Write a file containing atoms from selection
	 */
	struct WriteVisible
	{
		void execute( FilePath p_path );
	};

	/**
	 * @brief Associate a trajectory file with an existing system, conserving the topology but discarding current
	 * positions.
	 */
	struct AssociateTrajectory
	{
		AssociateTrajectory() = default;
		AssociateTrajectory( Util::Thread::ThreadData );
		void execute( FilePath p_path, const Entity & );
		void execute( const std::string & p_path, const Entity & );

	  private:
		Util::Thread::ThreadData _threadData;
	};

	/**
	 * @brief Run script.
	 */
	struct RunPythonScript
	{
		void execute( const FilePath & );
	};

	/**
	 * @brief Download a molecular system from a remote source.
	 */
	struct DownloadSystem
	{
	  public:
		DownloadSystem() = default;
		DownloadSystem( Util::Thread::ThreadData );

		void execute( const std::string & p_url, FilePath p_path );
		void execute( const std::string & p_systemId );

	  private:
		Util::Thread::ThreadData _threadData;
	};

	/**
	 * @brief Take a snapshot from actual camera view and save it to disk.
	 */
	class Snapshot
	{
	  public:
		void execute();
		void execute(
			const std::optional<FilePath> & p_path,
			const std::optional<FilePath> & p_filename,
			Util::Image::E_FORMAT			p_format,
			std::optional<size_t>			p_width,
			std::optional<size_t>			p_height,
			std::optional<float>			p_backgroundOpacity
		);
		void execute( const FilePath &, Util::Image::E_FORMAT, size_t, size_t, std::optional<float> = std::nullopt );
	};

} // namespace VTX::App::Action::IO
#endif
