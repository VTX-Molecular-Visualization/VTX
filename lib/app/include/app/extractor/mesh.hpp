#ifndef VTX_APP_EXTRACTOR_MESH_HPP
#define VTX_APP_EXTRACTOR_MESH_HPP

#include <util/filesystem.hpp>
#include <util/thread/base_thread.hpp>

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
		uint operator()( const Util::Thread::StopToken &, Util::Thread::OptionalThreadReference );

	  private:
		/**
		 * @brief File on disk.
		 */
		FilePath _sourcePath;
	};
} // namespace VTX::App::Extractor

#endif // !VTX_APP_EXTRACTOR_MESH_HPP
