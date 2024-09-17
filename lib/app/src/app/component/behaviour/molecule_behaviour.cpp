#include "app/component/behaviour/molecule_behaviour.hpp"
#include "app/application/selection/molecule_data.hpp"
#include "app/application/selection/molecule_granularity.hpp"
#include "app/application/system/renderer.hpp"
#include "app/application/system/settings_system.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/component/chemistry/trajectory.hpp"
#include "app/component/render/proxy_molecule.hpp"
#include "app/component/scene/aabb_component.hpp"
#include "app/component/scene/pickable.hpp"
#include "app/component/scene/selectable.hpp"
#include "app/component/scene/uid_component.hpp"
#include "app/core/player/base_player.hpp"
#include "app/core/player/loop.hpp"
#include "app/core/player/players.hpp"
#include "app/internal/application/settings.hpp"
#include "app/internal/io/reader/molecule_loader.hpp"

namespace VTX::App::Component::Behaviour
{
	Molecule::Molecule() :
		_entity( MAIN_REGISTRY().getEntity( *this ) ),
		_moleculeComponent( MAIN_REGISTRY().getComponent<Component::Chemistry::Molecule>( _entity ) )
	{
	}

	void Molecule::init( const Util::VariantMap & p_extraData )
	{
		_loadMolecule( p_extraData );

		_initSelectableComponent();
		_initUIDComponent();
		_initAABBComponent();
		_initGpuProxyComponent();
		_initTrajectoryComponent();
		_initPickableComponent();
	}

	void Molecule::_initSelectableComponent() const
	{
		Component::Scene::Selectable & selectableComponent
			= MAIN_REGISTRY().getComponent<Component::Scene::Selectable>( _entity );
		selectableComponent.setSelectionDataGenerator<Application::Selection::MoleculeData>();
	}
	void Molecule::_initUIDComponent() const
	{
		Component::Scene::UIDComponent & uidComponent
			= MAIN_REGISTRY().getComponent<Component::Scene::UIDComponent>( _entity );

		uidComponent.referenceUID( _moleculeComponent.getAtomUIDs() );
	}
	void Molecule::_initAABBComponent() const
	{
		Component::Scene::AABB & aabbComponent = MAIN_REGISTRY().getComponent<Component::Scene::AABB>( _entity );
		aabbComponent.init();
		aabbComponent.setAABBComputationFunction( [ this ]() { return _computeMoleculeAABB(); } );

		_moleculeComponent.onStruct +=
			[ this ]() { MAIN_REGISTRY().getComponent<Component::Scene::AABB>( _entity ).invalidateAABB(); };
	}
	void Molecule::_initGpuProxyComponent() const
	{
		if ( !MAIN_REGISTRY().hasComponent<Component::Render::ProxyMolecule>( _entity ) )
			return;

		Component::Render::ProxyMolecule & gpuProxyComponent
			= MAIN_REGISTRY().getComponent<Component::Render::ProxyMolecule>( _entity );

		gpuProxyComponent.setup( App::RENDERER().facade() );
	}
	void Molecule::_initTrajectoryComponent() const
	{
		if ( _moleculeComponent.hasTrajectory() )
		{
			Component::Chemistry::Trajectory & trajectoryComponent
				= MAIN_REGISTRY().addComponent<Component::Chemistry::Trajectory>( _entity, &_moleculeComponent );

			App::Core::Player::BasePlayer * defaultPlayMode
				= Util::Singleton<App::Core::Player::Players>::get().create<App::Core::Player::Loop>();

			trajectoryComponent.setPlayer( defaultPlayMode );
		}
	}

	void Molecule::_initPickableComponent() const
	{
		Component::Scene::Pickable & pickableComponent
			= MAIN_REGISTRY().getComponent<Component::Scene::Pickable>( _entity );

		pickableComponent.setPickingFunction( [ this ]( const Application::Selection::PickingInfo & p_pickingInfo )
											  { return _moleculePickingFunction( p_pickingInfo ); } );
	}

	void Molecule::_loadMolecule( const Util::VariantMap & p_extraData ) const
	{
		const Util::VariantMap::const_iterator filepathProperty = p_extraData.find( "filepath" );
		if ( filepathProperty == p_extraData.end() || !filepathProperty->second.is<std::string>() )
		{
			VTX_ERROR( "Missing property \"filepath\" in loading data." );
			return;
		}

		Internal::IO::Reader::MoleculeLoader loader = Internal::IO::Reader::MoleculeLoader();
		const FilePath						 path	= FilePath( filepathProperty->second.get<std::string>() );

		const Core::ECS::BaseEntity &	  entity = MAIN_REGISTRY().getEntity( _moleculeComponent );
		Component::IO::MoleculeMetadata & metaData
			= MAIN_REGISTRY().addComponent<Component::IO::MoleculeMetadata>( entity );

		if ( p_extraData.find( "buffer" ) != p_extraData.end() )
		{
			const Util::VTXVariant buffer = p_extraData.at( "buffer" ).get<std::string>();

			if ( !buffer.is<std::string>() )
			{
				VTX_ERROR( "Invalid buffer." );
				return;
			}

			loader.readBuffer( buffer.get<std::string>(), path, _moleculeComponent );
		}
		else // Filepath
		{
			loader.readFile( path, _moleculeComponent );
			metaData.path = path;
		}

		const VTX::IO::Reader::Chemfiles & chemfilesReader = loader.getChemfilesReader();
		const std::string &				   pdbId		   = chemfilesReader.getPdbIdCode();
		metaData.pdbIDCode								   = pdbId;

		_moleculeComponent.setPdbIdCode( pdbId );

		const std::string moleculeName = pdbId == "" ? Util::Filesystem::getFileName( path ) : pdbId;
		_moleculeComponent.setName( moleculeName );
	}

	Util::Math::AABB Molecule::_computeMoleculeAABB() const
	{
		Util::Math::AABB res = Util::Math::AABB();
		res.invalidate();

		for ( const auto & atomPtr : _moleculeComponent.getAtoms() )
		{
			if ( atomPtr != nullptr )
				res.extend( atomPtr->getLocalPosition() );
		}

		return res;
	}

	std::unique_ptr<Application::Selection::SelectionData> Molecule::_moleculePickingFunction(
		const Application::Selection::PickingInfo & p_pickingInfo
	) const
	{
		const Component::Scene::Selectable & selectableComponent
			= MAIN_REGISTRY().getComponent<Component::Scene::Selectable>( _entity );

		const Application::Selection::Granularity granularity = SETTINGS().get<Application::Selection::Granularity>(
			Internal::Application::Settings::Selection::MOLECULE_GRANULARITY_KEY
		);

		std::unique_ptr<Application::Selection::SelectionData> res
			= std::make_unique<Application::Selection::MoleculeData>( selectableComponent );

		Application::Selection::MoleculeData & molData = dynamic_cast<Application::Selection::MoleculeData &>( *res );
		molData.clear();

		if ( p_pickingInfo.hasOneValue() )
		{
			// First UID is Atom and not the other one => Pick Atom
			if ( _moleculeComponent.getAtomUIDs().contains( p_pickingInfo.getFirst() ) )
			{
				const Component::Chemistry::Atom * const atomPtr
					= _moleculeComponent.getAtomFromUID( p_pickingInfo.getFirst() );

				if ( atomPtr != nullptr )
				{
					molData.set( Application::Selection::MoleculeGranularity::getSelectionData( *atomPtr, granularity )
					);
				}
			}
			else if ( _moleculeComponent.getResidueUIDs().contains( p_pickingInfo.getFirst() ) )
			{
				// First UID is Residue => Pick Residue
				const Component::Chemistry::Residue * const residuePtr
					= _moleculeComponent.getResidueFromUID( p_pickingInfo.getFirst() );

				if ( residuePtr != nullptr )
				{
					molData.set(
						Application::Selection::MoleculeGranularity::getSelectionData( *residuePtr, granularity )
					);
				}
			}
		}
		else if ( p_pickingInfo.hasTwoValues() )
		{
			// Two atoms picked => Pick Bond
			if ( ( _moleculeComponent.getAtomUIDs().contains( p_pickingInfo.getFirst() )
				   && _moleculeComponent.getAtomUIDs().contains( p_pickingInfo.getSecond() ) ) )
			{
				const Component::Chemistry::Atom * const firstAtomPtr
					= _moleculeComponent.getAtomFromUID( p_pickingInfo.getFirst() );
				const Component::Chemistry::Atom * const secondAtomPtr
					= _moleculeComponent.getAtomFromUID( p_pickingInfo.getSecond() );

				if ( firstAtomPtr != nullptr && secondAtomPtr != nullptr )
				{
					molData.set(
						Application::Selection::MoleculeGranularity::getSelectionData( *firstAtomPtr, granularity )
					);

					molData.add(
						Application::Selection::MoleculeGranularity::getSelectionData( *secondAtomPtr, granularity )
					);
				}
			}
		}

		return res;
	}

} // namespace VTX::App::Component::Behaviour
