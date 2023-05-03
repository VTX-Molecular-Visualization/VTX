#include "app/component/video/path.hpp"
#include "app/component/object3d/viewpoint.hpp"
#include "app/core/event/vtx_event.hpp"
#include "app/event.hpp"
#include "app/event/global.hpp"
#include "app/mvc.hpp"
#include "app/old_app/id.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <util/exceptions.hpp>

namespace VTX::App::Component::Video
{
	Path::Path() : BaseModel( VTX::ID::Model::MODEL_PATH ) {};
	Path::~Path() { VTX::MVC_MANAGER().deleteAllModels( _viewpoints ); }

	void Path::addViewpoint( Object3D::Viewpoint * const p_viewpoint )
	{
		_viewpoints.emplace_back( p_viewpoint );
		VTX_EVENT<Object3D::Viewpoint *>( VTX::App::Event::Global::VIEWPOINT_ADDED, p_viewpoint );
	}
	void Path::removeViewpoint( Object3D::Viewpoint * const p_viewpoint )
	{
		_viewpoints.erase( std::find( _viewpoints.begin(), _viewpoints.end(), p_viewpoint ) );
		VTX_EVENT<Object3D::Viewpoint *>( VTX::App::Event::Global::VIEWPOINT_REMOVED, p_viewpoint );
	}

	/*
	const std::vector<std::string> * const Path::getCurrentActions( const float p_time )
	{
		uint  size	 = (uint)_viewpoints.size();
		float total	 = 0.f;
		uint  offset = 0;

		// Get the last actions.
		if ( p_time >= _duration )
		{
			return &_viewpoints[ size - 1 ]->getActions();
		}

		// Find the next and previous points.
		while ( total <= p_time && offset < size )
		{
			total += _viewpoints[ offset++ ]->getDuration();
		}
		offset--;
		return &_viewpoints[ offset - 1 ]->getActions();
	}
	*/

	Object3D::Viewpoint Path::getInterpolatedViewpoint( const float p_time ) const
	{
		Object3D::Viewpoint viewpoint = Object3D::Viewpoint( (Path * const)this );

		uint  size	 = (uint)_viewpoints.size();
		float total	 = 0.f;
		uint  offset = 0;

		// Find the next and previous points.
		while ( total <= p_time && offset < size )
		{
			total += _viewpoints[ offset++ ]->getDuration();
		}
		offset--;

		if ( _modeInterpolation == App::Component::Video::PATH_INTERPOLATION_MODE::LINEAR )
		{
			// Computes value.
			Object3D::Viewpoint * const p0	  = _viewpoints[ offset - 1 ];
			Object3D::Viewpoint * const p1	  = _viewpoints[ Util::Math::min<int>( (int)size - 1, offset ) ];
			float						value = 1.f - ( ( total - p_time ) / p1->getDuration() );

			VTX::ID::VTX_ID controller = p0->getController() == VTX::ID::Controller::TRACKBALL
												 && p1->getController() == VTX::ID::Controller::TRACKBALL
											 ? VTX::ID::Controller::TRACKBALL
											 : VTX::ID::Controller::FREEFLY;

			// Lerp.
			viewpoint.setController( controller );
			viewpoint.setRotation( Util::Math::linearInterpolation( p0->getRotation(), p1->getRotation(), value ) );
			viewpoint.setPosition( Util::Math::linearInterpolation( p0->getPosition(), p1->getPosition(), value ) );

			if ( controller == VTX::ID::Controller::TRACKBALL )
			{
				viewpoint.setTarget( Util::Math::linearInterpolation( p0->getTarget(), p1->getTarget(), value ) );
				viewpoint.setDistance( Util::Math::linearInterpolation( p0->getDistance(), p1->getDistance(), value ) );
			}
		}
		else if ( _modeInterpolation == App::Component::Video::PATH_INTERPOLATION_MODE::CATMULL_ROM )
		{
			Object3D::Viewpoint * const p0	  = _viewpoints[ Util::Math::max<int>( 0, (int)offset - 2 ) ];
			Object3D::Viewpoint * const p1	  = _viewpoints[ offset - 1 ];
			Object3D::Viewpoint * const p2	  = _viewpoints[ Util::Math::min<int>( (int)size - 1, offset ) ];
			Object3D::Viewpoint * const p3	  = _viewpoints[ Util::Math::min<int>( (int)size - 1, offset + 1 ) ];
			float						value = 1.f - ( ( total - p_time ) / p2->getDuration() );

			VTX::ID::VTX_ID controller = p1->getController() == VTX::ID::Controller::TRACKBALL
												 && p2->getController() == VTX::ID::Controller::TRACKBALL
											 ? VTX::ID::Controller::TRACKBALL
											 : VTX::ID::Controller::FREEFLY;

			viewpoint.setController( controller );
			viewpoint.setRotation( Util::Math::catmullRomInterpolation(
				p0->getRotation(), p1->getRotation(), p2->getRotation(), p3->getRotation(), value ) );

			viewpoint.setPosition( Util::Math::catmullRomInterpolation(
				p0->getPosition(), p1->getPosition(), p2->getPosition(), p3->getPosition(), value ) );

			if ( controller == VTX::ID::Controller::TRACKBALL )
			{
				viewpoint.setTarget( Util::Math::catmullRomInterpolation(
					p0->getTarget(), p1->getTarget(), p2->getTarget(), p3->getTarget(), value ) );
				viewpoint.setDistance( Util::Math::linearInterpolation( p1->getDistance(), p2->getDistance(), value ) );
			}
		}
		else if ( _modeInterpolation == App::Component::Video::PATH_INTERPOLATION_MODE::CUBIC )
		{
			Object3D::Viewpoint * const p0	  = _viewpoints[ Util::Math::max<int>( 0, (int)offset - 2 ) ];
			Object3D::Viewpoint * const p1	  = _viewpoints[ offset - 1 ];
			Object3D::Viewpoint * const p2	  = _viewpoints[ Util::Math::min<int>( (int)size - 1, offset ) ];
			Object3D::Viewpoint * const p3	  = _viewpoints[ Util::Math::min<int>( (int)size - 1, offset + 1 ) ];
			float						value = 1.f - ( ( total - p_time ) / p2->getDuration() );

			VTX::ID::VTX_ID controller = p1->getController() == VTX::ID::Controller::TRACKBALL
												 && p2->getController() == VTX::ID::Controller::TRACKBALL
											 ? VTX::ID::Controller::TRACKBALL
											 : VTX::ID::Controller::FREEFLY;

			viewpoint.setController( controller );
			viewpoint.setRotation( Util::Math::cubicInterpolation(
				p0->getRotation(), p1->getRotation(), p2->getRotation(), p3->getRotation(), value ) );
			viewpoint.setPosition( Util::Math::cubicInterpolation(
				p0->getPosition(), p1->getPosition(), p2->getPosition(), p3->getPosition(), value ) );

			if ( controller == VTX::ID::Controller::TRACKBALL )
			{
				viewpoint.setTarget( Util::Math::cubicInterpolation(
					p0->getTarget(), p1->getTarget(), p2->getTarget(), p3->getTarget(), value ) );
				viewpoint.setDistance( Util::Math::linearInterpolation( p1->getDistance(), p2->getDistance(), value ) );
			}
		}

		return viewpoint;
	}

	void Path::refreshAllDurations()
	{
		// Force the first to 0.
		if ( _viewpoints.size() > 0 )
		{
			_viewpoints[ 0 ]->setDuration( 0.f );
		}

		// Set same duration for each viewpoint.
		if ( _modeDuration == App::Component::Video::PATH_DURATION_MODE::PATH )
		{
			float duration = 0.f;
			if ( _viewpoints.size() >= 2 )
			{
				duration = _duration / ( _viewpoints.size() - 1 );
			}

			for ( uint i = 1; i < _viewpoints.size(); ++i )
			{
				Object3D::Viewpoint * const viewpoint = _viewpoints[ i ];
				viewpoint->setDuration( duration );
			}
		}
		// Set the path duration from viewpoint durations.
		else if ( _modeDuration == App::Component::Video::PATH_DURATION_MODE::VIEWPOINT )
		{
			_duration = 0.f;
			for ( uint i = 1; i < _viewpoints.size(); ++i )
			{
				Object3D::Viewpoint * const viewpoint = _viewpoints[ i ];
				_duration += viewpoint->getDuration();
			}
		}
		// Set viewport duration from path duration/distance.
		else if ( _modeDuration == App::Component::Video::PATH_DURATION_MODE::CONSTANT_SPEED )
		{
			// Compute total distance.
			float totalDistance = 0.f;
			uint  size			= (uint)Util::Math::max<int>( (int)_viewpoints.size() - 1, 0 );
			for ( uint i = 0; i < size; ++i )
			{
				totalDistance
					+= Util::Math::distance( _viewpoints[ i ]->getPosition(), _viewpoints[ i + 1u ]->getPosition() );
			}

			// Compute viewpoint durations.
			for ( uint i = 1; i < _viewpoints.size(); ++i )
			{
				Object3D::Viewpoint * const viewpoint = _viewpoints[ i ];
				if ( totalDistance == 0.f )
				{
					viewpoint->setDuration( 0.f );
					break;
				}

				const float distance
					= Util::Math::distance( _viewpoints[ i - 1u ]->getPosition(), viewpoint->getPosition() );
				viewpoint->setDuration( _duration * distance / totalDistance );
			}
		}
	}
} // namespace VTX::App::Component::Video
