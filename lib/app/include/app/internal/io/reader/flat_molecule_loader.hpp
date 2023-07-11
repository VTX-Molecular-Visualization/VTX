#ifndef __VTX_APP_INTERNAL_IO_FLAT_MOLECULE_LOADER__
#define __VTX_APP_INTERNAL_IO_FLAT_MOLECULE_LOADER__

#include "app/component/chemistry/flat/_fwd.hpp"
#include "app/component/io/molecule_metadata.hpp"
#include <io/core/reader/base_reader.hpp>
#include <io/reader/flat_molecule.hpp>
#include <string>
#include <util/types.hpp>
#include <utility>
#include <vector>

namespace VTX::App::Internal::IO::Reader
{
	class FlatMoleculeLoader : public VTX::IO::Core::Reader::BaseReader<App::Component::Chemistry::Flat::Molecule>
	{
	  public:
		FlatMoleculeLoader() : VTX::IO::Core::Reader::BaseReader<App::Component::Chemistry::Flat::Molecule>() {};

		void readFile( const FilePath & p_path, App::Component::Chemistry::Flat::Molecule & p_molecule ) override;
		void readBuffer( const std::string &, const FilePath &, App::Component::Chemistry::Flat::Molecule & ) override;

		const VTX::IO::Reader::Chemfiles & getChemfilesReader() { return _reader.getChemfilesReader(); }

	  private:
		VTX::IO::Reader::FlatMolecule _reader = VTX::IO::Reader::FlatMolecule();
	};
} // namespace VTX::App::Internal::IO::Reader
#endif
