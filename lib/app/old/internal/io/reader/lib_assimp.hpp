#ifndef __VTX_APP_OLD_INTERNAL_IO_READER_LIB_ASSIMP__
#define __VTX_APP_OLD_INTERNAL_IO_READER_LIB_ASSIMP__

#include "app/old/component/chemistry/_fwd.hpp"
#include "app/old/component/object3d/_fwd.hpp"
#include "app/old/core/io/reader/base_reader.hpp"

namespace VTX::App::Old::Internal::IO::Reader
{
	class LibAssimp :
		public Core::IO::Reader::BaseReader<App::Old::Component::Chemistry::Molecule>,
		public Core::IO::Reader::BaseReader<App::Old::Component::Object3D::MeshTriangle>
	{
	  public:
		void readFile( const FilePath &, Component::Object3D::MeshTriangle & ) override;
		void readFile( const FilePath &, Component::Chemistry::Molecule & ) override;
	};
} // namespace VTX::App::Old::Internal::IO::Reader
#endif
