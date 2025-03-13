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

	Trajectory::Trajectory( System * const p_system, const FilePath & p_path ) : _systemPtr( p_system ), _path( p_path )
	{
		_referenceUpdateFunction();
	}

	size_t Trajectory::getCurrentFrame() const { return _systemPtr->getTrajectory().getCurrentFrameIndex(); }
	void   Trajectory::setCurrentFrame( const size_t p_frameIndex )
	{
		_systemPtr->getTrajectory().setCurrentFrameIndex( p_frameIndex );
	}

	size_t Trajectory::getFrameCount() const { return _systemPtr->getTrajectory().getFrameCount(); }

	void Trajectory::_referenceUpdateFunction()
	{
		auto & updatable = ECS_REGISTRY().getComponent<Component::Scene::Updatable>( *this );
	}

} // namespace VTX::App::Component::Chemistry
