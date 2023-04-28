#ifndef __VTX_READER_LIB_ASSIMP__
#define __VTX_READER_LIB_ASSIMP__

#include "app/component/chemistry/_fwd.hpp"
#include "app/component/object3d/_fwd.hpp"
#include "base_reader.hpp"

namespace VTX::IO::Reader
{
	class LibAssimp :
		public BaseReader<App::Component::Chemistry::Molecule>,
		public BaseReader<App::Component::Object3D::MeshTriangle>
	{
	  public:
		void readFile( const FilePath &, App::Component::Object3D::MeshTriangle & ) override;
		void readFile( const FilePath &, App::Component::Chemistry::Molecule & ) override;
	};
} // namespace VTX::IO::Reader
#endif
