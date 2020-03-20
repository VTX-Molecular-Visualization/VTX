#ifndef __VTX_MODEL_PATH__
#define __VTX_MODEL_PATH__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_model.hpp"
#include "define.hpp"
#include "generic/base_serializable.hpp"
#include "object3d/camera.hpp"
#include "viewpoint.hpp"
#include <vector>

namespace VTX
{
	namespace Model
	{
		class Path : public BaseModel, public Generic::BaseSerializable
		{
		  public:
			using ViewpointPtr		 = Model::Viewpoint *;
			using VectorViewpointPtr = std::vector<ViewpointPtr>;

			enum class DURATION_MODE : int
			{
				CONSTANT_SPEED,
				PATH,
				VIEWPOINT
			};

			enum class INNTERPOLATION_MODE : int
			{
				LINEAR,
				CATMULL_ROM,
				CUBIC
			};

			~Path();

			void addViewpoint( const ViewpointPtr p_viewpoint ) { _viewpoints.emplace_back( p_viewpoint ); }
			void removeViewpoint( const ViewpointPtr p_viewpoint )
			{
				_viewpoints.erase( std::find( _viewpoints.begin(), _viewpoints.end(), p_viewpoint ) );
			}

			inline VectorViewpointPtr & getViewpoints() { return _viewpoints; }

			inline float			   getDuration() const { return _duration; }
			inline void				   setDuration( const float p_duration ) { _duration = p_duration; }
			inline DURATION_MODE	   getDurationMode() const { return _modeDuration; }
			inline void				   setDurationMode( const DURATION_MODE p_mode ) { _modeDuration = p_mode; }
			inline INNTERPOLATION_MODE getInterpolationMode() const { return _modeInterpolation; }
			inline void		 setInterpolationMode( const INNTERPOLATION_MODE p_mode ) { _modeInterpolation = p_mode; }
			inline bool		 isLooping() const { return _isLooping; }
			inline void		 setIsLooping( const bool p_isLooping ) { _isLooping = p_isLooping; }
			void			 refreshAllDurations();
			Model::Viewpoint getInterpolatedViewpoint( float p_time ) const;

			virtual void setSelected( const bool ) override;

			// TODO: redo implementation.
			virtual void load( const IO::Path & ) override;
			virtual void save( const IO::Path & ) const override;

		  protected:
			virtual void _addItems() override final;

		  private:
			VectorViewpointPtr	_viewpoints		   = VectorViewpointPtr();
			DURATION_MODE		_modeDuration	   = DURATION_MODE::CONSTANT_SPEED;
			INNTERPOLATION_MODE _modeInterpolation = INNTERPOLATION_MODE::LINEAR;
			float				_duration		   = PATH_DEFAULT_DURATION;
			bool				_isLooping		   = false;
		};
	} // namespace Model
} // namespace VTX
#endif
