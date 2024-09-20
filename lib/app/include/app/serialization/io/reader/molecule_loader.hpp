#ifndef __VTX_APP_SERIALIZATION_IO_READER_MOLECULE_LOADER__
#define __VTX_APP_SERIALIZATION_IO_READER_MOLECULE_LOADER__

#include "app/component/chemistry/_fwd.hpp"
#include "app/component/io/molecule_metadata.hpp"
#include <io/core/reader/base_reader.hpp>
#include <io/reader/molecule.hpp>
#include <string>
#include <util/types.hpp>
#include <utility>
#include <vector>

namespace VTX::App::Serialization::IO::Reader
{
	class MoleculeLoader : public VTX::IO::Core::Reader::BaseReader<App::Component::Chemistry::Molecule>
	{
	  public:
		MoleculeLoader() : VTX::IO::Core::Reader::BaseReader<App::Component::Chemistry::Molecule>() {};

		void readFile( const FilePath & p_path, App::Component::Chemistry::Molecule & p_molecule ) override;
		void readBuffer( const std::string &, const FilePath &, App::Component::Chemistry::Molecule & ) override;

		const VTX::IO::Reader::Chemfiles & getChemfilesReader() { return _reader.getChemfilesReader(); }

	  private:
		VTX::IO::Reader::Molecule _reader = VTX::IO::Reader::Molecule();
	};
} // namespace VTX::App::Serialization::IO::Reader
#endif
