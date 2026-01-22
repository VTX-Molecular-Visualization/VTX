#ifndef __VTX_IO_READER_SYSTEM__
#define __VTX_IO_READER_SYSTEM__

#include "io/core/reader/base_reader.hpp"
#include "io/metadata/system.hpp"
#include "io/reader/chemfiles.hpp"
#include "io/struct/_fwd.hpp"
#include <core/chemdb/atom.hpp>
#include <core/chemdb/category.hpp>
#include <core/struct/system.hpp>
#include <map>
#include <string>
#include <util/math/aabb.hpp>
#include <util/types.hpp>

namespace VTX::IO::Reader
{
	namespace ChemDB = VTX::Core::ChemDB;

	class System : public IO::Core::Reader::BaseReader<VTX::Core::Struct::System>
	{
	  public:
		System() {};

		void readFile( const FilePath & p_path, VTX::Core::Struct::System & p_system );
		void readBuffer( const std::string & p_buffer, const FilePath & p_path, VTX::Core::Struct::System & p_system );

		void setConfiguration( const Struct::SystemConfiguration & p_configuration )
		{
			_configuration = &p_configuration;
		}

		const Reader::Chemfiles & getChemfilesReader() { return *_chemfilesReader; }

		void readNextFrame() { _chemfilesReader->readNextFrame(); } // TODO : not a huge fan on this

		const VTX::Util::Math::AABB & getAABB() const { return _aabb; }

	  private:
		void _fillStructure( IO::Reader::Chemfiles & p_chemfileStruct, VTX::Core::Struct::System & p_system );

		// TODO do this function here or in IO::Reader::ChemfilesTrajectory ?
		VTX::Core::ChemDB::Category::TYPE _findCategoryType(
			const std::string & p_fileExtension,
			const std::string & p_residueSymbol
		);

		// TODO do this function here or in IO::Reader::ChemfilesTrajectory ?
		void _readTrajectoryFrames(
			IO::Reader::Chemfiles &											   p_chemfileStruct,
			const std::vector<std::pair<VTX::Core::Struct::System *, Index>> & p_targets,
			const Index														   p_trajectoryFrameStart
		);

		ChemDB::Atom::TYPE _getTypeInConfiguration( const IO::Reader::Chemfiles & p_chemfileStruct ) const;

		std::unique_ptr<Reader::Chemfiles>	_chemfilesReader = nullptr;
		const Struct::SystemConfiguration * _configuration	 = nullptr;
		VTX::Util::Math::AABB				_aabb;
	};
} // namespace VTX::IO::Reader
#endif
