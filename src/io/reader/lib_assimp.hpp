#ifndef __VTX_READER_LIB_ASSIMP__
#define __VTX_READER_LIB_ASSIMP__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_reader.hpp"

namespace VTX
{
	namespace Model
	{
		class MeshTriangle;
		class Molecule;
	} // namespace Model

	namespace IO
	{
		namespace Reader
		{
			class LibAssimp : public BaseReader<Model::Molecule>, public BaseReader<Model::MeshTriangle>
			{
			  public:
				void readFile( const FilePath &, Model::MeshTriangle & ) override;
				void readFile( const FilePath &, Model::Molecule & ) override;
			};
		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
#endif
