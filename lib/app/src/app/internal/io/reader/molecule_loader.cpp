#include "app/internal/io/reader/molecule_loader.hpp"
#include "app/component/chemistry/atom.hpp"
#include "app/component/chemistry/bond.hpp"
#include "app/component/chemistry/chain.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/component/chemistry/residue.hpp"
#include "app/internal/util/molecule.hpp"
#include <algorithm>
#include <core/chemdb/category.hpp>
#include <core/chemdb/chain.hpp>
#include <core/struct/trajectory.hpp>
#include <io/reader/molecule.hpp>
#include <iostream>
#include <thread>
#include <unordered_map>
#include <util/chrono.hpp>
#include <util/color/rgba.hpp>
#include <util/enum.hpp>
#include <util/logger.hpp>

namespace VTX::App::Internal::IO::Reader
{
	namespace Core	 = VTX::Core;
	namespace ChemDB = VTX::Core::ChemDB;

	void MoleculeLoader::readFile( const FilePath & p_path, App::Component::Chemistry::Molecule & p_molecule )
	{
		_reader								  = VTX::IO::Reader::Molecule();
		Core::Struct::Molecule moleculeStruct = Core::Struct::Molecule();

		_reader.readFile( p_path, moleculeStruct );
		VTX::Util::Chrono chrono = VTX::Util::Chrono();
		chrono.start();
		p_molecule.setMoleculeStruct( moleculeStruct );
		chrono.stop();
		VTX_INFO( "Build Molecule convenient structure : {}", chrono.elapsedTimeStr() );
	}

	void MoleculeLoader::readBuffer(
		const std::string &					  p_buffer,
		const FilePath &					  p_path,
		App::Component::Chemistry::Molecule & p_molecule
	)
	{
		_reader								  = VTX::IO::Reader::Molecule();
		Core::Struct::Molecule moleculeStruct = Core::Struct::Molecule();

		_reader.readBuffer( p_buffer, p_path, moleculeStruct );
		p_molecule.setMoleculeStruct( moleculeStruct );
	}

} // namespace VTX::App::Internal::IO::Reader
