#ifndef __VTX_IO_MESH_READER__
#define __VTX_IO_MESH_READER__

#include <functional>
#include <util/filesystem.hpp>
#include <util/thread/stop_token.hpp>
#include <vector>

namespace VTX::Core::Struct
{
	struct Mesh;
}

namespace VTX::IO
{
	/**
	 * @brief Load a mesh file using Assimp.
	 */
	class MeshReader
	{
	  public:
		MeshReader() = delete;
		MeshReader( FilePath, Util::Thread::StopToken & );

		/**
		 * @brief Read mesh structure.
		 */
		void get( std::vector<VTX::Core::Struct::Mesh> & );

	  private:
		FilePath										_filePath;
		std::reference_wrapper<Util::Thread::StopToken> _stopToken;
	};

	/**
	 * @brief Check available extensions.
	 */
	bool isMeshFileFormat( const FilePath & );
} // namespace VTX::IO

#endif // !__VTX_IO_MESH_READER__
