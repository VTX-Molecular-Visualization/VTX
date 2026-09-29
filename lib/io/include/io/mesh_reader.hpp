#ifndef __VTX_IO_MESH_READER__
#define __VTX_IO_MESH_READER__

#include <util/filesystem.hpp>
#include <util/thread/base_thread.hpp>
#include <vector>

namespace VTX::Core::Struct
{
	struct Mesh;
} // namespace VTX::Core::Struct

namespace VTX::IO
{
	/**
	 * @brief Load a mesh file using Assimp.
	 */
	class MeshReader
	{
	  public:
		MeshReader() = delete;
		MeshReader( const FilePath &, const Util::Thread::StopToken & );

		/**
		 * @brief Read mesh structure.
		 */
		void get( std::vector<VTX::Core::Struct::Mesh> & );

		/**
		 * @brief Check available extensions.
		 */
		static bool isMeshFileFormat( const FilePath & );

	  private:
		FilePath				_filePath;
		Util::Thread::StopToken _stopToken;
	};

} // namespace VTX::IO

#endif