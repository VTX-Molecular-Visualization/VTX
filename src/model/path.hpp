#ifndef __VTX_MODEL_PATH__
#define __VTX_MODEL_PATH__

#ifdef _MSC_VER
#pragma once
#endif

#include "../generic/base_serializable.hpp"
#include "../object3d/camera.hpp"
#include "base_model.hpp"
#include "define.hpp"
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

			enum class COMPUTATION_MODE : int
			{
				DURATION,
				SPEED
			};

			Path() { setId( 0 ); };
			~Path();

			void addViewpoint( const ViewpointPtr p_viewpoint ) { _viewpoints.emplace_back( p_viewpoint ); }
			void removeViewpoint( const ViewpointPtr p_viewpoint )
			{
				_viewpoints.erase( std::find( _viewpoints.begin(), _viewpoints.end(), p_viewpoint ) );
			}
			inline VectorViewpointPtr & getViewpoints() { return _viewpoints; }

			virtual void						  setSelected( const bool ) override;
			void								  setSelectedViewpoint( Viewpoint * const );
			void								  resetSelectedViewpoint();
			float								  computeTotalTime() const;
			Viewpoint::ViewpointInterpolationData getCurrentViewpointInterpolationData( float p_time ) const;

			// TODO: redo implementation.
			virtual void load( const IO::Path & ) override;
			virtual void save( const IO::Path & ) const override;

		  protected:
			virtual void _addItems() override final;

		  private:
			VectorViewpointPtr _viewpoints		  = VectorViewpointPtr();
			COMPUTATION_MODE   _mode			  = COMPUTATION_MODE::DURATION;
			ViewpointPtr	   _selectedViewpoint = nullptr;

		}; // namespace Camera
	}	   // namespace Model
} // namespace VTX
#endif
