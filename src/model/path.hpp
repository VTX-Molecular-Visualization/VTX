#ifndef __VTX_MODEL_PATH__
#define __VTX_MODEL_PATH__

#include "base_model.hpp"
#include "define.hpp"
#include "setting.hpp"
#include <vector>

namespace VTX
{
	namespace Model
	{
		class Viewpoint;
		class Path : public BaseModel
		{
			VTX_MODEL

		  public:
			using ViewpointPtr		 = Viewpoint *;
			using VectorViewpointPtr = std::vector<ViewpointPtr>;

			enum class DURATION_MODE : int
			{
				CONSTANT_SPEED,
				PATH,
				VIEWPOINT
			};

			enum class INTERPOLATION_MODE : int
			{
				LINEAR,
				CATMULL_ROM,
				CUBIC
			};

			void addViewpoint( const ViewpointPtr p_viewpoint ) { _viewpoints.emplace_back( p_viewpoint ); }
			void removeViewpoint( const ViewpointPtr p_viewpoint )
			{
				_viewpoints.erase( std::find( _viewpoints.begin(), _viewpoints.end(), p_viewpoint ) );
			}

			inline const std::string &		  getName() const { return _name; }
			inline void						  setName( const std::string & p_name ) { _name = p_name; }
			inline VectorViewpointPtr &		  getViewpoints() { return _viewpoints; }
			inline const VectorViewpointPtr & getViewpoints() const { return _viewpoints; }

			inline float			  getDuration() const { return _duration; }
			inline void				  setDuration( const float p_duration ) { _duration = p_duration; }
			inline DURATION_MODE	  getDurationMode() const { return _modeDuration; }
			inline void				  setDurationMode( const DURATION_MODE p_mode ) { _modeDuration = p_mode; }
			inline INTERPOLATION_MODE getInterpolationMode() const { return _modeInterpolation; }
			inline void setInterpolationMode( const INTERPOLATION_MODE p_mode ) { _modeInterpolation = p_mode; }
			inline bool isLooping() const { return _isLooping; }
			inline void setIsLooping( const bool p_isLooping ) { _isLooping = p_isLooping; }
			void		refreshAllDurations();
			Viewpoint	getInterpolatedViewpoint( const float p_time ) const;
			const std::vector<std::string> * const getCurrentActions( const float p_time );

		  private:
			std::string		   _name			  = "";
			VectorViewpointPtr _viewpoints		  = VectorViewpointPtr();
			DURATION_MODE	   _modeDuration	  = DURATION_MODE::CONSTANT_SPEED;
			INTERPOLATION_MODE _modeInterpolation = INTERPOLATION_MODE::CATMULL_ROM;
			float			   _duration		  = Setting::PATH_DURATION_DEFAULT;
			bool			   _isLooping		  = false;

			Path();
			~Path();
		};
	} // namespace Model
} // namespace VTX
#endif
