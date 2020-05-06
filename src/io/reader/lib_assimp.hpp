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
				virtual void readFile( const Path &, Model::MeshTriangle & ) override;
				virtual void readBuffer( const std::string &, const std::string &, Model::MeshTriangle & ) override
				{
					throw Exception::NotImplementedException();
				}

				virtual void readFile( const Path &, Model::Molecule & ) override;
				virtual void readBuffer( const std::string &, const std::string &, Model::Molecule & ) override
				{
					throw Exception::NotImplementedException();
				}
			};
		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
#endif
