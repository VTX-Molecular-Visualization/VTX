#include "app/internal/io/reader/flat_molecule_loader.hpp"
#include "app/component/chemistry/flat/atom.hpp"
#include "app/component/chemistry/flat/bond.hpp"
#include "app/component/chemistry/flat/chain.hpp"
#include "app/component/chemistry/flat/molecule.hpp"
#include "app/component/chemistry/flat/residue.hpp"
#include "app/internal/util/molecule.hpp"
#include <algorithm>
#include <core/chemdb/category.hpp>
#include <core/chemdb/chain.hpp>
#include <core/struct/trajectory.hpp>
#include <io/reader/flat_molecule.hpp>
#include <iostream>
#include <magic_enum.hpp>
#include <thread>
#include <unordered_map>
#include <util/chrono.hpp>
#include <util/color/rgba.hpp>
#include <util/logger.hpp>

namespace VTX::App::Internal::IO::Reader
{
	namespace ChemDB = VTX::Core::ChemDB;

	void FlatMoleculeLoader::readFile( const FilePath & p_path, App::Component::Chemistry::Flat::Molecule & p_molecule )
	{
		_reader										  = VTX::IO::Reader::FlatMolecule();
		Core::FlatStruct::Molecule flatMoleculeStruct = Core::FlatStruct::Molecule();

		_reader.readFile( p_path, flatMoleculeStruct );
		VTX::Util::Chrono chrono = VTX::Util::Chrono();
		chrono.start();
		p_molecule.setMoleculeStruct( flatMoleculeStruct );
		chrono.stop();
		VTX_INFO( "Build Molecule convenient structure : {}", chrono.elapsedTimeStr() );
	}

	void FlatMoleculeLoader::readBuffer( const std::string &						 p_buffer,
										 const FilePath &							 p_path,
										 App::Component::Chemistry::Flat::Molecule & p_molecule )
	{
		_reader										  = VTX::IO::Reader::FlatMolecule();
		Core::FlatStruct::Molecule flatMoleculeStruct = Core::FlatStruct::Molecule();

		_reader.readBuffer( p_buffer, p_path, flatMoleculeStruct );
		p_molecule.setMoleculeStruct( flatMoleculeStruct );
	}

} // namespace VTX::App::Internal::IO::Reader
