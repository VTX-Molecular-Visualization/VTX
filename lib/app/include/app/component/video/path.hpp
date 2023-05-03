#ifndef __VTX_APP_COMPONENT_VIDEO_PATH__
#define __VTX_APP_COMPONENT_VIDEO_PATH__

#include "app/application/setting.hpp"
#include "app/component/object3d/viewpoint.hpp" // viewpoint included because Object3D::Viewpoint getInterpolatedViewpoint( const float p_time ) const. Maybe change the function to remove this include
// #include "app/component/object3d/_fwd.hpp"
#include "app/component/video/enum_path.hpp"
#include "app/core/model/base_model.hpp"
#include "app/core/scene/base_scene_item.hpp"
#include <string>
#include <util/types.hpp>
#include <vector>

namespace VTX::App::Component::Video
{
	class Path : public Core::Model::BaseModel, public Core::Scene::BaseSceneItem
	{
		VTX_MODEL

	  public:
		using VectorViewpointPtr = std::vector<Component::Object3D::Viewpoint *>;

		// BaseSceneItem
		const Core::Model::ID & getModelID() const override { return getId(); }

		void addViewpoint( Object3D::Viewpoint * const p_viewpoint );
		void removeViewpoint( Object3D::Viewpoint * const p_viewpoint );

		inline const std::string &		  getName() const { return _name; }
		inline void						  setName( const std::string & p_name ) { _name = p_name; }
		inline VectorViewpointPtr &		  getViewpoints() { return _viewpoints; }
		inline const VectorViewpointPtr & getViewpoints() const { return _viewpoints; }

		inline float				   getDuration() const { return _duration; }
		inline void					   setDuration( const float p_duration ) { _duration = p_duration; }
		inline PATH_DURATION_MODE	   getDurationMode() const { return _modeDuration; }
		inline void					   setDurationMode( const PATH_DURATION_MODE p_mode ) { _modeDuration = p_mode; }
		inline PATH_INTERPOLATION_MODE getInterpolationMode() const { return _modeInterpolation; }
		inline void setInterpolationMode( const PATH_INTERPOLATION_MODE p_mode ) { _modeInterpolation = p_mode; }
		inline bool isLooping() const { return _isLooping; }
		inline void setIsLooping( const bool p_isLooping ) { _isLooping = p_isLooping; }
		void		refreshAllDurations();
		Object3D::Viewpoint getInterpolatedViewpoint( const float p_time ) const;

	  private:
		std::string				_name			   = "";
		VectorViewpointPtr		_viewpoints		   = VectorViewpointPtr();
		PATH_DURATION_MODE		_modeDuration	   = Application::Setting::DEFAULT_PATH_DURATION_MODE;
		PATH_INTERPOLATION_MODE _modeInterpolation = Application::Setting::DEFAULT_PATH_INTERPOLATION_MODE;
		float					_duration		   = Application::Setting::PATH_DURATION_DEFAULT;
		bool					_isLooping		   = false;

		Path();
		~Path();
	};
} // namespace VTX::App::Component::Video
#endif
