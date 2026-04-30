#ifndef __VTX_APP_ACTION_IO__
#define __VTX_APP_ACTION_IO__

#include "app/ecs.hpp"
#include "app/threading/base_thread.hpp"
#include <app/action/action_manager.hpp>
#include <util/image.hpp>
#include <util/thread.hpp>
#include <util/types.hpp>
#include <util/url.hpp>

namespace VTX::Util::Url
{

	class SystemId;
	class UrlFull;
} // namespace VTX::Util::Url

namespace VTX::App::Action::IO
{

	/**
	 * @brief Open a file or folder with the system default application.
	 */
	struct Open
	{
		void execute( const FilePath & p_path );
		void execute( const std::string & p_path );
	};

	struct _SystemIo;
	struct _SystemIoDel
	{
		void operator()( _SystemIo * ) noexcept;
	};
	using _SystemIoPtr = std::unique_ptr<_SystemIo, _SystemIoDel>;

	/**
	 * @brief Load a molecular system from disk or buffer.
	 */
	struct LoadSystem
	{
		LoadSystem();
		LoadSystem( Util::StopToken, Threading::OptionalThreadReference );

		void execute( FilePath p_path );
		void execute( FilePath p_path, std::string && p_buffer );
		void wait() noexcept;

	  private:
		_SystemIoPtr _data;
	};

	/**
	 * @brief Write a file containing atoms from selection
	 */
	struct WriteSelection
	{
		WriteSelection();
		WriteSelection( Util::StopToken, Threading::OptionalThreadReference );

		void execute( FilePath p_path );
		void wait() noexcept;

	  private:
		struct _WriterIo;
		struct _del
		{
			void operator()( _WriterIo * ) const noexcept;
		};
		std::unique_ptr<_WriterIo, _del> _data = nullptr;
	};

	/**
	 * @brief Associate a trajectory file with an existing system, conserving the topology but discarding current
	 * positions.
	 */
	struct AssociateTrajectory
	{
		AssociateTrajectory();
		AssociateTrajectory( Util::StopToken, Threading::OptionalThreadReference );
		void execute( const FilePath & p_path, const ECS::Entity & );
		void execute( const std::string & p_path, const ECS::Entity & );
		void wait() noexcept;

	  private:
		_SystemIoPtr _data;
	};

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
		void execute( VTX::Util::Url::SystemId );
		void execute( VTX::Util::Url::SystemId, FilePath );
		void execute( VTX::Util::Url::UrlFull, FilePath );
		void execute( const std::string & p_systemId );
	};

	/**
	 * @brief Take a snapshot from actual camera view and save it to disk.
	 */
	class Snapshot
	{
	  public:
		void execute();
		void execute( const FilePath, const Util::Image::E_FORMAT, const size_t, const size_t );

	  private:
		FilePath			  _path;
		Util::Image::E_FORMAT _format;
		size_t				  _width;
		size_t				  _height;
	};

} // namespace VTX::App::Action::IO
#endif
