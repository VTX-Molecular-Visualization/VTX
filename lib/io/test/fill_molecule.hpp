#ifndef __VTX_IO_TEST_FILL_MOLECULE__
#define __VTX_IO_TEST_FILL_MOLECULE__

#include "_fwd.hpp"
#include "io/reader/chemfiles.hpp"
#include <core/chemdb/category.hpp>
#include <core/struct/_fwd.hpp>
#include <core/struct/trajectory.hpp>
#include <string>
#include <util/types.hpp>
#include <utility>
#include <vector>

namespace VTX::IO::Test
{
	void fillStructure( IO::Reader::Chemfiles & p_chemfileStruct, Core::Struct::Molecule & p_molecule );

	VTX::Core::ChemDB::Category::TYPE _findCategoryType( const std::string & p_fileExtension,
														 const std::string & p_residueSymbol );

	// TODO do this function here or in IO::Reader::Chemfiles ?
	VTX::Core::ChemDB::Category::TYPE _findCategoryType( const std::string & p_fileExtension,
														 const std::string & p_residueSymbol );

	// TODO do this function here or in IO::Reader::Chemfiles ?
	void _readTrajectoryFrames( IO::Reader::Chemfiles &											 p_chemfileStruct,
								const std::vector<std::pair<Core::Struct::Molecule *, size_t>> & p_targets,
								const size_t p_trajectoryFrameStart );
} // namespace VTX::IO::Test
#endif
