#ifndef __VTX_READER_LIB_ASSIMP__
#define __VTX_READER_LIB_ASSIMP__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_reader.hpp"
#include "model/mesh_triangle.hpp"
#include "model/molecule.hpp"

namespace VTX
{
	namespace IO
	{
		namespace Reader
		{
			class LibAssimp : public BaseReader<Model::Molecule>, public BaseReader<Model::MeshTriangle>
			{
			  public:
				void readFile( const Path &, Model::MeshTriangle & ) override;
				void readFile( const Path &, Model::Molecule & ) override;
			};
		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
#endif
