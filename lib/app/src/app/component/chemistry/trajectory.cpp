#include "app/component/chemistry/trajectory.hpp"
#include "app/component/chemistry/system.hpp"
#include "app/component/scene/scene_item_component.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Component::Chemistry
{
	Trajectory::Trajectory()
	{
		_moleculePtr = &ECS_REGISTRY().getComponent<System>( *this );
		_referenceUpdateFunction();
	}

	Trajectory::Trajectory( System * const p_molecule ) : _moleculePtr( p_molecule ) { _referenceUpdateFunction(); }

	size_t Trajectory::getCurrentFrame() const { return _moleculePtr->getTrajectory().currentFrameIndex; }
	void   Trajectory::setCurrentFrame( const size_t p_frameIndex )
	{
		_moleculePtr->getTrajectory().currentFrameIndex = p_frameIndex;
	}

	size_t Trajectory::getFrameCount() const { return _moleculePtr->getTrajectory().frames.size(); }

	void Trajectory::setPlayer( App::Core::Player::BasePlayer * const p_player )
	{
		const bool resetPlayer = _player == nullptr;

		onFrameChange.clear();

		_player = p_player;
		_player->setCount( _moleculePtr->getTrajectory().getFrameCount() );

		_player->onFrameChange += [ this ]( const size_t p_frameIndex )
		{
			_moleculePtr->getTrajectory().currentFrameIndex = p_frameIndex;
			onFrameChange( p_frameIndex );
		};

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
			= ECS_REGISTRY().getComponent<Component::Scene::SceneItemComponent>( *this );
		sceneComponent.addUpdateFunction( "", [ this ]( const float p_deltaTime ) { _update( p_deltaTime ); } );
	}

} // namespace VTX::App::Component::Chemistry
