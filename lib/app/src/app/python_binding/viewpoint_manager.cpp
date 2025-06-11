
#include "app/python_binding/viewpoint_manager.hpp"
#include "app/application/scene.hpp"
#include "app/component/render/camera.hpp"

namespace VTX::App::PythonBinding
{
	void TravelViewpointManager::registerCurrent() { registerCurrent( _defaultTravelTime ); }
	void TravelViewpointManager::registerCurrent( float p_travelTime )
	{
		_viewpoints.push_back(
			{ SCENE().getCamera().getTransform().getPosition(),
			  SCENE().getCamera().getTransform().getRotation(),
			  p_travelTime }
		);
	}

	TravelViewpointManager::Iterator TravelViewpointManager::begin() { return Iterator( _viewpoints.begin() ); }
	TravelViewpointManager::Iterator TravelViewpointManager::end() { return Iterator( _viewpoints.end() ); }
	TravelViewpoint & TravelViewpointManager::operator[]( const size_t & p_idx ) { return _viewpoints[ p_idx ]; }
	size_t			  TravelViewpointManager::size() const { return _viewpoints.size(); }
} // namespace VTX::App::PythonBinding
