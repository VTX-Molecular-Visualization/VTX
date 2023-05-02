#ifndef __VTX_APP_INTERNAL_IO_READER_LIB_ASSIMP__
#define __VTX_APP_INTERNAL_IO_READER_LIB_ASSIMP__

#include "app/component/chemistry/_fwd.hpp"
#include "app/component/object3d/_fwd.hpp"
#include "app/core/io/reader/base_reader.hpp"

namespace VTX::App::Internal::IO::Reader
{
	class LibAssimp :
		public Core::IO::Reader::BaseReader<App::Component::Chemistry::Molecule>,
		public Core::IO::Reader::BaseReader<App::Component::Object3D::MeshTriangle>
	{
	  public:
		void readFile( const FilePath &, Component::Object3D::MeshTriangle & ) override;
		void readFile( const FilePath &, Component::Chemistry::Molecule & ) override;
	};
} // namespace VTX::App::Internal::IO::Reader
#endif
