#ifndef __VTX_READER_LIB_ASSIMP__
#define __VTX_READER_LIB_ASSIMP__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_reader.hpp"
#include "model/molecule.hpp"
#include "model/mesh_triangle.hpp"

namespace VTX
{
	namespace IO
	{
		namespace Reader
		{
			class LibAssimp : public BaseReader<Model::Molecule>, public BaseReader<Model::MeshTriangle>
			{
			  public:
				virtual bool readFile( const Path &, Model::MeshTriangle & ) override { return false; }
				virtual bool readBuffer( const std::string &, Model::MeshTriangle & ) override { return false; }
				virtual bool readFile( const Path &, Model::Molecule & ) override;
				virtual bool readBuffer( const std::string &, Model::Molecule & ) override { return false; }
			};
		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
#endif
