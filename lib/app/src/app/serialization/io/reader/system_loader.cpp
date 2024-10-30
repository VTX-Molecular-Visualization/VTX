#include "app/serialization/io/reader/system_loader.hpp"
#include "app/component/chemistry/atom.hpp"
#include "app/component/chemistry/bond.hpp"
#include "app/component/chemistry/chain.hpp"
#include "app/component/chemistry/residue.hpp"
#include "app/component/chemistry/system.hpp"
#include <algorithm>
#include <core/chemdb/category.hpp>
#include <core/chemdb/chain.hpp>
#include <core/struct/trajectory.hpp>
#include <io/reader/system.hpp>
#include <iostream>
#include <thread>
#include <unordered_map>
#include <util/chrono.hpp>
#include <util/color/rgba.hpp>
#include <util/enum.hpp>
#include <util/logger.hpp>
#include <util/string.hpp>

namespace VTX::App::Serialization::IO::Reader
{
	namespace Core	 = VTX::Core;
	namespace ChemDB = VTX::Core::ChemDB;

	void SystemLoader::readFile( const FilePath & p_path, App::Component::Chemistry::System & p_system )
	{
		_reader								= VTX::IO::Reader::System();
		Core::Struct::System systemStruct = Core::Struct::System();

		_reader.readFile( p_path, systemStruct );
		VTX::Util::Chrono chrono = VTX::Util::Chrono();
		chrono.start();
		p_system.setSystemStruct( systemStruct );
		chrono.stop();
		VTX_INFO( "Build System convenient structure : {}", Util::String::durationToStr( chrono.elapsedTime() ) );
	}

	void SystemLoader::readBuffer(
		const std::string &					p_buffer,
		const FilePath &					p_path,
		App::Component::Chemistry::System & p_system
	)
	{
		_reader								= VTX::IO::Reader::System();
		Core::Struct::System systemStruct = Core::Struct::System();

		_reader.readBuffer( p_buffer, p_path, systemStruct );
		p_system.setSystemStruct( systemStruct );
	}

} // namespace VTX::App::Serialization::IO::Reader
