// #include <app/ecs/component/molecule_component.hpp>
// #include <app/ecs/entity/molecule_entity.hpp>
// #include <app/ecs/io/lib_chemfiles.hpp>
// #include <app/ecs/scene.hpp>
// #include <app/ecs/test.hpp>
#include <app/internal/io/filesystem.hpp>
#include <app/model/chemistry/molecule.hpp>
#include <app/model/io/molecule_loader.hpp>
#include <app/render/proxy_builder.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
// #include <core/struct/atom.hpp>
// #include <core/struct/molecule.hpp>
//  #include <entt/entt.hpp>
#include <string>
// #include <util/logger.hpp>
#include <util/types.hpp>
#include <vector>

// TEST_CASE( "EnTT", "Perfs" )
//{
//	// using entity_t = uint64_t;
//	//  using registry_t = entt::basic_registry<entity_t>;
//	using entity_t	 = entt::entity;
//	using registry_t = entt::registry;
//
//	using Position = VTX::Vec3f;
//
//	const VTX::uint atomCount = 1000000;
//
//	BENCHMARK( "Populate registry" )
//	{
//		registry_t registry = registry_t();
//
//		for ( VTX::uint i = 0; i < atomCount; i++ )
//		{
//			const entity_t entity = registry.create();
//			registry.emplace<VTX::Core::Struct::Atom>( entity );
//		}
//	};
//
//	registry_t registry	 = registry_t();
//	auto	   atomGroup = registry.group<VTX::Core::Struct::Atom>( entt::get<Position> );
//
//	for ( VTX::uint i = 0; i < atomCount; i++ )
//	{
//		const entity_t entity = registry.create();
//		registry.emplace<VTX::Core::Struct::Atom>( entity );
//	}
//
//	BENCHMARK( "Browse all atoms with view" )
//	{
//		auto view = registry.view<VTX::Core::Struct::Atom>();
//		for ( entity_t entity : view )
//		{
//			VTX::Core::Struct::Atom & atom		   = registry.get<VTX::Core::Struct::Atom>( entity );
//			const int				  randomSymbol = std::rand() % int( VTX::Core::ChemDB::Atom::SYMBOL::COUNT );
//			atom.setSymbol( VTX::Core::ChemDB::Atom::SYMBOL( randomSymbol ) );
//
//			// VTX::Core::Struct::setVisible( atom, true );
//		}
//	};
//
//	BENCHMARK( "Browse all atoms with group" )
//	{
//		auto tmpGroup = registry.group<VTX::Core::Struct::Atom>( entt::get<Position> );
//		for ( entity_t entity : tmpGroup )
//		{
//			VTX::Core::Struct::Atom & atom		   = tmpGroup.get<VTX::Core::Struct::Atom>( entity );
//			const int				  randomSymbol = std::rand() % int( VTX::Core::ChemDB::Atom::SYMBOL::COUNT );
//			atom.setSymbol( VTX::Core::ChemDB::Atom::SYMBOL( randomSymbol ) );
//
//			// VTX::Core::Struct::setVisible( atom, true );
//		}
//	};
//
//	BENCHMARK( "Browse all atoms with prepared group" )
//	{
//		for ( entity_t entity : atomGroup )
//		{
//			VTX::Core::Struct::Atom & atom		   = atomGroup.get<VTX::Core::Struct::Atom>( entity );
//			const int				  randomSymbol = std::rand() % int( VTX::Core::ChemDB::Atom::SYMBOL::COUNT );
//			atom.setSymbol( VTX::Core::ChemDB::Atom::SYMBOL( randomSymbol ) );
//
//			// VTX::Core::Struct::setVisible( atom, true );
//		}
//	};
// }
//
// TEST_CASE( "VTX", "Perfs" )
//{
//	// using entity_t = uint64_t;
//	//  using registry_t = entt::basic_registry<entity_t>;
//
//	const VTX::uint atomCount = 1000000;
//
//	BENCHMARK( "Populate vector" )
//	{
//		std::vector<VTX::Core::Struct::Atom> atoms = std::vector<VTX::Core::Struct::Atom>();
//		atoms.resize( atomCount );
//
//		for ( VTX::uint i = 0; i < atomCount; i++ )
//		{
//			atoms[ i ] = VTX::Core::Struct::Atom();
//		}
//	};
//
//	std::vector<VTX::Core::Struct::Atom> atoms = std::vector<VTX::Core::Struct::Atom>();
//	atoms.resize( atomCount );
//
//	for ( int i = 0; i < atomCount; i++ )
//	{
//		atoms[ i ] = VTX::Core::Struct::Atom();
//	}
//
//	BENCHMARK( "Browse all atoms" )
//	{
//		for ( VTX::uint i = 0; i < atomCount; i++ )
//		{
//			const int randomSymbol = std::rand() % int( VTX::Core::ChemDB::Atom::SYMBOL::COUNT );
//			atoms[ i ].setSymbol( VTX::Core::ChemDB::Atom::SYMBOL( randomSymbol ) );
//		}
//	};
// }
//
// TEST_CASE( "App::ECS", "[ecs]" )
//{
//	using namespace VTX;
//
//	const std::string moleculeName = "8ai8";
//
//	App::ECS::Scene scene = App::ECS::Scene();
//
//	VTX::Core::Struct::Molecule * const molecule = new VTX::Core::Struct::Molecule();
//	App::ECS::IO::LibChemfiles			loader	 = App::ECS::IO::LibChemfiles();
//	const FilePath moleculePath = App::Internal::IO::Filesystem::getInternalDataDir() / ( moleculeName + ".pdb" );
//	loader.readFile( moleculePath, *molecule );
//
//	REQUIRE( molecule );
//
//	App::ECS::Core::BaseEntity entity = App::ECS::Entity::generateMoleculeEntity( *molecule );
//
//	// App::ECS::Component::SceneItemComponent & sceneItemComponent
//	//	= App::ECS::Manager::ECSManager::get().getComponent<App::ECS::Component::SceneItemComponent>( entity );
//
//	// scene.addItem( sceneItemComponent );
//
//	// REQUIRE( !scene.isEmpty() );
//
//	// App::ECS::Component::MoleculeComponent * const moleculeComponent
//	//	= App::ECS::Test::_actionGetMoleculeByName( moleculeName );
//
//	// if ( moleculeComponent == nullptr )
//	//{
//	//	VTX_INFO( "{} not found.", moleculeName );
//	//	return;
//	// }
// }
//

TEST_CASE( "VTX", "New Archi" )
{
	using namespace VTX::App;

	const std::string moleculeName	   = "1AGA";
	const std::string moleculePathname = moleculeName + ".mmtf";

	Model::Chemistry::Molecule *	  moleculePtr  = new Model::Chemistry::Molecule();
	Model::IO::Reader::MoleculeLoader libChemfiles = Model::IO::Reader::MoleculeLoader();
	const VTX::FilePath				  moleculePath = Internal::IO::Filesystem::getInternalDataDir() / moleculePathname;

	BENCHMARK( "Load file" )
	{
		// Load Molecule
		moleculePtr = new Model::Chemistry::Molecule();
		libChemfiles.readFile( moleculePath, *moleculePtr );
		delete moleculePtr;
	};

	// Load Molecule
	moleculePtr = new Model::Chemistry::Molecule();
	libChemfiles.readFile( moleculePath, *moleculePtr );

	//// Setup GPU Proxy
	VTX::Renderer::GL::StructProxyMolecule * const molGpuProxy = Render::GPUProxyBuilder::setupProxy( *moleculePtr );

	delete molGpuProxy;
	delete moleculePtr;

	// Render
};
