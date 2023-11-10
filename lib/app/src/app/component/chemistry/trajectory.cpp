#include "app/component/chemistry/trajectory.hpp"
#include "app/application/ecs/registry_manager.hpp"
#include "app/component/chemistry/molecule.hpp"
#include "app/component/scene/scene_item_component.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Component::Chemistry
{
	Trajectory::Trajectory() { _referenceUpdateFunction(); };
	Trajectory::Trajectory( Molecule * const p_molecule ) : _moleculePtr( p_molecule ) { _referenceUpdateFunction(); }

	size_t Trajectory::getCurrentFrame() const
	{
		return _moleculePtr->getMoleculeStruct().trajectory.currentFrameIndex;
	}

	size_t Trajectory::getFrameCount() const { return _moleculePtr->getMoleculeStruct().trajectory.frames.size(); }

	void Trajectory::setPlayer( std::unique_ptr<App::Core::TrajectoryPlayer::BasePlayer> & p_player )
	{
		const bool resetPlayer = _player == nullptr;

		_player = std::move( p_player );
		_player->setTrajectory( _moleculePtr->getTrajectory() );

		if ( resetPlayer )
			_player->reset();
	}

	void Trajectory::_update( const float p_deltaTime )
	{
		if ( _player != nullptr )
			_player->update( p_deltaTime );
	}

	void Trajectory::_referenceUpdateFunction()
	{
		Component::Scene::SceneItemComponent & sceneComponent
			= MAIN_REGISTRY().getComponent<Component::Scene::SceneItemComponent>( *this );
		sceneComponent.addUpdateFunction( "", [ this ]( const float p_deltaTime ) { _update( p_deltaTime ); } );
	}

} // namespace VTX::App::Component::Chemistry
