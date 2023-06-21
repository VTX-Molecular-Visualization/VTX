// #include <app/ecs/component/molecule_component.hpp>
#include <app/application/scene.hpp>
#include <app/ecs/action/ecs.hpp>
#include <app/ecs/entity/scene/molecule_entity.hpp>
#include <app/ecs/registry_manager.hpp>
#include <app/model/chemistry/molecule.hpp>
#include <app/old/action.hpp>
#include <app/old/internal/io/filesystem.hpp>
#include <app/vtx_app.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>
#include <core/old/struct/atom.hpp>
#include <entt/entt.hpp>
#include <renderer/gl/struct_proxy_molecule.hpp>
#include <string>
#include <util/logger.hpp>
#include <util/types.hpp>
#include <vector>

TEST_CASE( "VTX_APP - EnTT perfs", "[.] [perfs]" )
{
	// using entity_t = uint64_t;
	// using registry_t = entt::basic_registry<entity_t>;
	using entity_t	 = entt::entity;
	using registry_t = entt::registry;

	using Position = VTX::Vec3f;

	const VTX::uint atomCount = 1000000;

	BENCHMARK( "Populate registry" )
	{
		registry_t registry = registry_t();

		for ( VTX::uint i = 0; i < atomCount; i++ )
		{
			const entity_t entity = registry.create();
			registry.emplace<VTX::Core::Old::Struct::Atom>( entity );
		}
	};

	registry_t		  registry	= registry_t();
	entt::basic_group atomGroup = registry.group<VTX::Core::Old::Struct::Atom>( entt::get<Position> );

	for ( VTX::uint i = 0; i < atomCount; i++ )
	{
		const entity_t entity = registry.create();
		registry.emplace<VTX::Core::Old::Struct::Atom>( entity );
	}

	BENCHMARK( "Browse all atoms with view" )
	{
		auto view = registry.view<VTX::Core::Old::Struct::Atom>();
		for ( entity_t entity : view )
		{
			VTX::Core::Old::Struct::Atom & atom		   = registry.get<VTX::Core::Old::Struct::Atom>( entity );
			const int				  randomSymbol = std::rand() % int( VTX::Core::ChemDB::Atom::SYMBOL::COUNT );
			atom.setSymbol( VTX::Core::ChemDB::Atom::SYMBOL( randomSymbol ) );

			// VTX::Core::Old::Struct::setVisible( atom, true );
		}
	};

	BENCHMARK( "Browse all atoms with group" )
	{
		auto tmpGroup = registry.group<VTX::Core::Old::Struct::Atom>( entt::get<Position> );
		for ( entity_t entity : tmpGroup )
		{
			VTX::Core::Old::Struct::Atom & atom		   = tmpGroup.get<VTX::Core::Old::Struct::Atom>( entity );
			const int				  randomSymbol = std::rand() % int( VTX::Core::ChemDB::Atom::SYMBOL::COUNT );
			atom.setSymbol( VTX::Core::ChemDB::Atom::SYMBOL( randomSymbol ) );

			// VTX::Core::Old::Struct::setVisible( atom, true );
		}
	};

	BENCHMARK( "Browse all atoms with prepared group" )
	{
		for ( entity_t entity : atomGroup )
		{
			VTX::Core::Old::Struct::Atom & atom		   = atomGroup.get<VTX::Core::Old::Struct::Atom>( entity );
			const int				  randomSymbol = std::rand() % int( VTX::Core::ChemDB::Atom::SYMBOL::COUNT );
			atom.setSymbol( VTX::Core::ChemDB::Atom::SYMBOL( randomSymbol ) );

			// VTX::Core::Old::Struct::setVisible( atom, true );
		}
	};
}

TEST_CASE( "VTX_APP - Instantiations perfs", "[.] [perfs]" )
{
	const VTX::uint atomCount = 1000000;

	BENCHMARK( "Populate vector" )
	{
		std::vector<VTX::Core::Old::Struct::Atom> atoms = std::vector<VTX::Core::Old::Struct::Atom>();
		atoms.resize( atomCount );

		for ( VTX::uint i = 0; i < atomCount; i++ )
		{
			atoms[ i ] = VTX::Core::Old::Struct::Atom();
		}
	};

	std::vector<VTX::Core::Old::Struct::Atom> atoms = std::vector<VTX::Core::Old::Struct::Atom>();
	atoms.resize( atomCount );

	for ( int i = 0; i < atomCount; i++ )
	{
		atoms[ i ] = VTX::Core::Old::Struct::Atom();
	}

	BENCHMARK( "Browse all atoms" )
	{
		for ( VTX::uint i = 0; i < atomCount; i++ )
		{
			const int randomSymbol = std::rand() % int( VTX::Core::ChemDB::Atom::SYMBOL::COUNT );
			atoms[ i ].setSymbol( VTX::Core::ChemDB::Atom::SYMBOL( randomSymbol ) );
		}
	};
}

TEST_CASE( "VTX_APP - Full sequence", "[.] [integration]" )
{
	using namespace VTX;
	using namespace VTX::App;

	const std::string moleculeName	   = "1AGA";
	const std::string moleculePathname = moleculeName + ".mmtf";

	// Create Scene
	VTXApp::get().start( {} );
	const Application::Scene & scene = VTXApp::get().getScene();

	// Create MoleculeEntity
	const FilePath moleculePath = Old::Internal::IO::Filesystem::getInternalDataDir() / moleculePathname;
	VTX_ACTION<ECS::Action::Open>( moleculePath );

	// Pick first Molecule
	REQUIRE( scene.getItemCount() == 1 );

	ECS::Core::BaseEntity moleculeEntity = scene.getItem( 0 );
	REQUIRE( ECS::MAIN_REGISTRY().isValid( moleculeEntity ) );

	moleculeEntity = scene.getItem( 1 );
	REQUIRE( !ECS::MAIN_REGISTRY().isValid( moleculeEntity ) );

	moleculeEntity = scene.getItem( moleculeName );
	REQUIRE( ECS::MAIN_REGISTRY().isValid( moleculeEntity ) );

	entt::basic_view view = scene.getAllSceneItemsOftype<Model::Chemistry::Molecule>();
	REQUIRE( view.size_hint() == 1 );

	Renderer::GL::StructProxyMolecule & gpuProxyComponent
		= ECS::MAIN_REGISTRY().getComponent<Renderer::GL::StructProxyMolecule>( moleculeEntity );

	REQUIRE( gpuProxyComponent.atomIds != nullptr );
	REQUIRE( ( ( *gpuProxyComponent.atomIds )[ 2 ] ) == size_t( 2 ) );
};

TEST_CASE( "VTX_APP - Benchamrk", "[perfs]" )
{
	using namespace VTX;
	using namespace VTX::App;

	const std::string moleculeName	   = "8OIT";
	const std::string moleculePathname = moleculeName + ".mmtf";

	// Create Scene
	VTXApp::get().start( {} );
	const Application::Scene & scene = VTXApp::get().getScene();

	// Create MoleculeEntity
	const FilePath moleculePath = Old::Internal::IO::Filesystem::getInternalDataDir() / moleculePathname;

	BENCHMARK( "Open molecules" ) { VTX_ACTION<ECS::Action::Open>( moleculePath ); };

	int i = 0;
	BENCHMARK( "Get" )
	{
		ECS::Core::BaseEntity moleculeEntity = scene.getItem( i );
		i++;
	};

	BENCHMARK( "View all" ) { entt::basic_view view = scene.getAllSceneItemsOftype<Model::Chemistry::Molecule>(); };
}
