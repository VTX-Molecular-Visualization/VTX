#include "app/component/chemistry/trajectory.hpp"
#include "app/component/chemistry/system.hpp"
#include "app/component/scene/updatable.hpp"
#include "app/vtx_app.hpp"

namespace VTX::App::Component::Chemistry
{
	Trajectory::Trajectory()
	{
		_systemPtr = &ECS_REGISTRY().getComponent<System>( *this );
		_referenceUpdateFunction();
	}

	Trajectory::Trajectory( System * const p_system, const FilePath & p_path ) : _systemPtr( p_system ), _path(p_path)
	{
		_referenceUpdateFunction();
	}

	size_t Trajectory::getCurrentFrame() const { return _systemPtr->getTrajectory().currentFrameIndex; }
	void   Trajectory::setCurrentFrame( const size_t p_frameIndex )
	{
		_systemPtr->getTrajectory().currentFrameIndex = p_frameIndex;
	}

	// devjla
	// FIXME obviously wrong
	// size_t Trajectory::getFrameCount() const { return _systemPtr->getTrajectory().frames.size(); }
	size_t Trajectory::getFrameCount() const
	{
		//return _moleculePtr->getTrajectory().frames.GetTotalElements();
		if ( _systemPtr->getTrajectory()._isOptimized )
			return _systemPtr->getTrajectory()._framesCircBuff.GetTotalElements();
		else
			return _systemPtr->getTrajectory()._framesVector.size();
	}

	void Trajectory::setPlayer( App::Core::Player::BasePlayer * const p_player )
	{
		const bool resetPlayer = _player == nullptr;

		onFrameChange.clear();

		_player = p_player;
		_player->setCount( _systemPtr->getTrajectory().getFrameCount() );

		/* _player->onFrameChange += [ this ]( const size_t p_frameIndex )
		{
			_systemPtr->getTrajectory()._currentFrameIndex = p_frameIndex;
			onFrameChange( p_frameIndex );
		};*/
		_player->onFrameChange += [ this ]( const VTX::Core::Struct::Frame & p_frame )
		{
			//_systemPtr->getTrajectory()._currentFrameIndex = p_frameIndex; currently done in player
			onFrameChange( p_frame );
		};

		if ( resetPlayer )
		{
			_player->reset();
		}
	}

	void Trajectory::_update( const float p_deltaTime )
	{
		if ( _player != nullptr )
		{
			_player->update( p_deltaTime );
		}
	}

	void Trajectory::_referenceUpdateFunction()
	{
		auto & updatable = ECS_REGISTRY().getComponent<Component::Scene::Updatable>( *this );
		updatable.addUpdateFunction( [ this ]( const float p_deltaTime, const float ) { _update( p_deltaTime ); } );
	}

} // namespace VTX::App::Component::Chemistry
