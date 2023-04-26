#ifndef __VTX_READER_LIB_ASSIMP__
#define __VTX_READER_LIB_ASSIMP__

#include "app/component/chemistry/_fwd.hpp"
#include "base_reader.hpp"

namespace VTX
{
	namespace Model
	{
		class MeshTriangle;
	} // namespace Model

	namespace IO
	{
		namespace Reader
		{
			class LibAssimp :
				public BaseReader<App::Component::Chemistry::Molecule>,
				public BaseReader<Model::MeshTriangle>
			{
			  public:
				void readFile( const FilePath &, Model::MeshTriangle & ) override;
				void readFile( const FilePath &, App::Component::Chemistry::Molecule & ) override;
			};
		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
#endif
