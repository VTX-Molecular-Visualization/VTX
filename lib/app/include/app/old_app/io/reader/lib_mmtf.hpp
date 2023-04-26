#ifndef __VTX_READER_LIB_MMTF__
#define __VTX_READER_LIB_MMTF__

#include "app/component/chemistry/_fwd.hpp"
#include "base_reader.hpp"
#pragma warning( push, 0 )
// #include <mmtf/mmtf.hpp>
#pragma warning( pop )

namespace VTX::IO::Reader
{
	class LibMMTF : public BaseReader<App::Component::Chemistry::Molecule>
	{
	  public:
		[[deprecated]] void readFile( const FilePath &, App::Component::Chemistry::Molecule & ) override {}
		[[deprecated]] void readBuffer( const std::string &,
										const FilePath &,
										App::Component::Chemistry::Molecule & ) override
		{
		}

	  private:
		// void _readStructureData( const mmtf::StructureData &, App::Component::Chemistry::Molecule & );
	};
} // namespace VTX::IO::Reader
#endif
