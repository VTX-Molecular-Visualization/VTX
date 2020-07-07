#ifndef __VTX_READER_LIB_MMTF__
#define __VTX_READER_LIB_MMTF__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_reader.hpp"
#pragma warning( push, 0 )
//#include <mmtf/mmtf.hpp>
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
				[[deprecated]] virtual void readFile( const Path &, Model::Molecule & ) override {}
				[[deprecated]] virtual void readBuffer( const std::string &, const Path &, Model::Molecule & ) override
				{
				}

			  private:
				// void _readStructureData( const mmtf::StructureData &, Model::Molecule & );
			};
		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
#endif
