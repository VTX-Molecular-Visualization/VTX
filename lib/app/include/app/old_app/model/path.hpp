#ifndef __VTX_MODEL_PATH__
#define __VTX_MODEL_PATH__

#include "base_model.hpp"
#include "app/old_app/generic/base_scene_item.hpp"
#include "app/old_app/path/path_enum.hpp"
#include "app/old_app/setting.hpp"
#include <util/types.hpp>
#include <vector>

namespace VTX
{
	namespace Model
	{
		class Viewpoint;
		class Path : public BaseModel, public Generic::BaseSceneItem
		{
			VTX_MODEL

		  public:
			using ViewpointPtr		 = Viewpoint *;
			using VectorViewpointPtr = std::vector<ViewpointPtr>;

			// BaseSceneItem
			const Model::ID & getModelID() const override { return getId(); }

			void addViewpoint( const ViewpointPtr p_viewpoint );
			void removeViewpoint( const ViewpointPtr p_viewpoint );

			inline const std::string &		  getName() const { return _name; }
			inline void						  setName( const std::string & p_name ) { _name = p_name; }
			inline VectorViewpointPtr &		  getViewpoints() { return _viewpoints; }
			inline const VectorViewpointPtr & getViewpoints() const { return _viewpoints; }

			inline float					getDuration() const { return _duration; }
			inline void						setDuration( const float p_duration ) { _duration = p_duration; }
			inline VTX::Path::DURATION_MODE getDurationMode() const { return _modeDuration; }
			inline void setDurationMode( const VTX::Path::DURATION_MODE p_mode ) { _modeDuration = p_mode; }
			inline VTX::Path::INTERPOLATION_MODE getInterpolationMode() const { return _modeInterpolation; }
			inline void							 setInterpolationMode( const VTX::Path::INTERPOLATION_MODE p_mode )
			{
				_modeInterpolation = p_mode;
			}
			inline bool isLooping() const { return _isLooping; }
			inline void setIsLooping( const bool p_isLooping ) { _isLooping = p_isLooping; }
			void		refreshAllDurations();
			Viewpoint	getInterpolatedViewpoint( const float p_time ) const;

			std::string generateNewViewpointName() const;

		  private:
			std::string					  _name				 = "";
			VectorViewpointPtr			  _viewpoints		 = VectorViewpointPtr();
			VTX::Path::DURATION_MODE	  _modeDuration		 = Setting::DEFAULT_PATH_DURATION_MODE;
			VTX::Path::INTERPOLATION_MODE _modeInterpolation = Setting::DEFAULT_PATH_INTERPOLATION_MODE;
			float						  _duration			 = Setting::PATH_DURATION_DEFAULT;
			bool						  _isLooping		 = false;

			Path();
			~Path();
		};
	} // namespace Model
} // namespace VTX
#endif
