#ifndef __VTX_IO_READER_MOLECULE__
#define __VTX_IO_READER_MOLECULE__

#include "io/core/reader/base_reader.hpp"
#include "io/metadata/molecule.hpp"
#include "io/reader/chemfiles.hpp"
#include "io/struct/_fwd.hpp"
#include <core/chemdb/atom.hpp>
#include <core/chemdb/category.hpp>
#include <core/struct/molecule.hpp>
#include <map>
#include <string>
#include <util/types.hpp>

namespace VTX::IO::Reader
{
	namespace ChemDB = VTX::Core::ChemDB;

	class Molecule : public IO::Core::Reader::BaseReader<VTX::Core::Struct::Molecule>
	{
	  public:
		Molecule() {};

		void readFile( const FilePath & p_path, VTX::Core::Struct::Molecule & p_molecule );
		void readBuffer( const std::string &		   p_buffer,
						 const FilePath &			   p_path,
						 VTX::Core::Struct::Molecule & p_molecule );

		void setConfiguration( const Struct::MoleculeConfiguration & p_configuration )
		{
			_configuration = &p_configuration;
		}

		const Reader::Chemfiles & getChemfilesReader() { return *_chemfilesReader; }

	  private:
		void _fillStructure( IO::Reader::Chemfiles & p_chemfileStruct, VTX::Core::Struct::Molecule & p_molecule );

		// TODO do this function here or in IO::Reader::Chemfiles ?
		VTX::Core::ChemDB::Category::TYPE _findCategoryType( const std::string & p_fileExtension,
															 const std::string & p_residueSymbol );

		// TODO do this function here or in IO::Reader::Chemfiles ?
		void _readTrajectoryFrames( IO::Reader::Chemfiles & p_chemfileStruct,
									const std::vector<std::pair<VTX::Core::Struct::Molecule *, size_t>> & p_targets,
									const size_t p_trajectoryFrameStart );

		ChemDB::Atom::TYPE _getTypeInConfiguration( const IO::Reader::Chemfiles & p_chemfileStruct ) const;

		std::unique_ptr<Reader::Chemfiles>	  _chemfilesReader = nullptr;
		const Struct::MoleculeConfiguration * _configuration   = nullptr;
	};
} // namespace VTX::IO::Reader
#endif
