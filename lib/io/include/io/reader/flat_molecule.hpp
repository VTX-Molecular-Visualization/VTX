#ifndef __VTX_IO_READER_FLAT_MOLECULE__
#define __VTX_IO_READER_FLAT_MOLECULE__

#include "io/core/reader/base_reader.hpp"
#include "io/metadata/molecule.hpp"
#include "io/reader/chemfiles.hpp"
#include <core/chemdb/category.hpp>
#include <core/flat_struct/molecule.hpp>
#include <map>
#include <string>
#include <util/types.hpp>
// #include <utility>
// #include <vector>

namespace VTX::IO::Reader
{
	class FlatMolecule : public IO::Core::Reader::BaseReader<VTX::Core::FlatStruct::Molecule>
	{
	  public:
		FlatMolecule() {};

		void readFile( const FilePath & p_path, VTX::Core::FlatStruct::Molecule & p_molecule )
		{
			std::unique_ptr<Reader::Chemfiles> chemfileReader = Reader::Chemfiles::readFile( p_path );
			_fillStructure( *chemfileReader, p_molecule );
		}
		void readBuffer( const std::string &			   p_buffer,
						 const FilePath &				   p_path,
						 VTX::Core::FlatStruct::Molecule & p_molecule )
		{
			std::unique_ptr<Reader::Chemfiles> chemfileReader = Reader::Chemfiles::readBuffer( p_buffer, p_path );
			_fillStructure( *chemfileReader, p_molecule );
		}

	  private:
		void _fillStructure( IO::Reader::Chemfiles & p_chemfileStruct, VTX::Core::FlatStruct::Molecule & p_molecule );

		// TODO do this function here or in IO::Reader::Chemfiles ?
		VTX::Core::ChemDB::Category::TYPE _findCategoryType( const std::string & p_fileExtension,
															 const std::string & p_residueSymbol );

		// TODO do this function here or in IO::Reader::Chemfiles ?
		void _readTrajectoryFrames( IO::Reader::Chemfiles & p_chemfileStruct,
									const std::vector<std::pair<VTX::Core::FlatStruct::Molecule *, size_t>> & p_targets,
									const size_t p_trajectoryFrameStart );
	};
} // namespace VTX::IO::Reader
#endif
