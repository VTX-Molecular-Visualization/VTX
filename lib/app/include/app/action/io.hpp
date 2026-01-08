#ifndef __VTX_APP_ACTION_IO__
#define __VTX_APP_ACTION_IO__

#include <util/image.hpp>
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
		void execute( const char * p_systemId );
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
