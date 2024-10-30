#ifndef __VTX_APP_SERIALIZATION_IO_READER_MOLECULE_LOADER__
#define __VTX_APP_SERIALIZATION_IO_READER_MOLECULE_LOADER__

#include "app/component/chemistry/_fwd.hpp"
#include "app/component/io/system_metadata.hpp"
#include <io/core/reader/base_reader.hpp>
#include <io/reader/system.hpp>
#include <string>
#include <util/types.hpp>
#include <utility>
#include <vector>

namespace VTX::App::Serialization::IO::Reader
{
	class MoleculeLoader : public VTX::IO::Core::Reader::BaseReader<App::Component::Chemistry::System>
	{
	  public:
		MoleculeLoader() : VTX::IO::Core::Reader::BaseReader<App::Component::Chemistry::System>() {};

		void readFile( const FilePath & p_path, App::Component::Chemistry::System & p_molecule ) override;
		void readBuffer( const std::string &, const FilePath &, App::Component::Chemistry::System & ) override;

		const VTX::IO::Reader::Chemfiles & getChemfilesReader() { return _reader.getChemfilesReader(); }

	  private:
		VTX::IO::Reader::System _reader = VTX::IO::Reader::System();
	};
} // namespace VTX::App::Serialization::IO::Reader
#endif
