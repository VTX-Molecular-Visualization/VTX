#ifndef __VTX_READER_LIB_MMTF__
#define __VTX_READER_LIB_MMTF__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_reader.hpp"
#pragma warning( push, 0 )
#include <mmtf/mmtf.hpp>
#pragma warning( pop )
#include "model/molecule.hpp"

namespace VTX
{
	namespace IO
	{
		namespace Reader
		{
			class LibMMTF : public BaseReader<Model::Molecule>
			{
			  public:
				virtual bool readFile( const Path &, Model::Molecule & ) override;
				virtual bool readBuffer( const std::string &, Model::Molecule & ) override;

			  private:
				bool _readStructureData( const mmtf::StructureData &, Model::Molecule & );
			};
		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
#endif
