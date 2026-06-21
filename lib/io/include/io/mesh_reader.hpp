#ifndef __VTX_IO_MESH_READER__
#define __VTX_IO_MESH_READER__

#include <functional>
#include <util/filesystem.hpp>

namespace VTX::Util
{
	class StopToken;
}

namespace VTX::Core::Struct
{
	struct Mesh;
}

namespace VTX::IO
{
	class MeshReader
	{
	  public:
		MeshReader() = delete;
		MeshReader( FilePath, VTX::Util::StopToken & );

		void get( VTX::Core::Struct::Mesh & );

	  private:
		FilePath									 _filePath;
		std::reference_wrapper<VTX::Util::StopToken> _stopToken;
	};

	bool isMeshFileFormat( const FilePath & );
} // namespace VTX::IO

#endif // !__VTX_IO_MESH_READER__
