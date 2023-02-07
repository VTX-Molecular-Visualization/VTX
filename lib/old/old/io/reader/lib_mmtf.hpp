#ifndef __VTX_READER_LIB_MMTF__
#define __VTX_READER_LIB_MMTF__

#include "base_reader.hpp"
#pragma warning( push, 0 )
// #include <mmtf/mmtf.hpp>
#pragma warning( pop )

namespace VTX
{
	namespace Model
	{
		class Molecule;
	}

	namespace IO
	{
		namespace Reader
		{
			class LibMMTF : public BaseReader<Model::Molecule>
			{
			  public:
				[[deprecated]] void readFile( const Util::FilePath &, Model::Molecule & ) override {}
				[[deprecated]] void readBuffer( const std::string &, const Util::FilePath &, Model::Molecule & ) override
				{
				}

			  private:
				// void _readStructureData( const mmtf::StructureData &, Model::Molecule & );
			};
		} // namespace Reader
	}	  // namespace IO
} // namespace VTX
#endif
