#ifndef __VTX_APP_EXTRACTOR_MESH__
#define __VTX_APP_EXTRACTOR_MESH__

#include <util/filesystem.hpp>
#include <util/thread/base_thread.hpp>
#include <util/thread/stop_token.hpp>

namespace VTX::App::Extractor
{
	/**
	 * @brief Extract a mesh from IO data.
	 */
	class Mesh
	{
	  public:
		Mesh() = delete;
		explicit Mesh( FilePath );

		/**
		 * @brief Stop current execution until the system is extracted.
		 */
		uint operator()( Util::Thread::StopToken, Util::Thread::OptionalThreadReference );

	  private:
		/**
		 * @brief File on disk.
		 */
		FilePath _sourcePath;
	};
} // namespace VTX::App::Extractor

#endif // !__VTX_APP_EXTRACTOR_MESH__
