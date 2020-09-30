#include "path.hpp"
#include "exception.hpp"
#include "generic/factory.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>

namespace VTX
{
	namespace Model
	{
		Path::Path() {}

		Path::~Path() { Generic::clearVector( _viewpoints ); }

		const std::vector<std::string> * const Path::getCurrentActions( const double p_time )
		{
			uint  size	 = (uint)_viewpoints.size();
			float total	 = 0.0;
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

		Model::Viewpoint Path::getInterpolatedViewpoint( const double p_time ) const
		{
			Viewpoint viewpoint = Viewpoint( (Path * const)this );

			uint   size	  = (uint)_viewpoints.size();
			double total  = 0.0;
			uint   offset = 0;

			// Find the next and previous points.
			while ( total <= p_time && offset < size )
			{
				total += _viewpoints[ offset++ ]->getDuration();
			}
			offset--;

			if ( _modeInterpolation == INTERPOLATION_MODE::LINEAR )
			{
				// Computes value.
				Viewpoint * const p0	= _viewpoints[ offset - 1 ];
				Viewpoint * const p1	= _viewpoints[ Util::Math::min<int>( (int)size - 1, offset ) ];
				double			  value = 1.0 - ( ( total - p_time ) / p1->getDuration() );

				// Lerp.
				viewpoint.setPosition( Util::Math::linearInterpolation( p0->getPosition(), p1->getPosition(), value ) );
				viewpoint.setRotation( Util::Math::linearInterpolation( p0->getRotation(), p1->getRotation(), value ) );
				viewpoint.setTarget( Util::Math::linearInterpolation( p0->getTarget(), p1->getTarget(), value ) );
				viewpoint.setDistance( Util::Math::linearInterpolation( p0->getDistance(), p1->getDistance(), value ) );
				viewpoint.setController( p0->getController() == ID::Controller::TRACKBALL && p1->getController() == ID::Controller::TRACKBALL ? ID::Controller::TRACKBALL
																																			  : ID::Controller::FREEFLY );
			}
			else if ( _modeInterpolation == INTERPOLATION_MODE::CATMULL_ROM )
			{
				Viewpoint * const p0	= _viewpoints[ Util::Math::max<int>( 0, (int)offset - 2 ) ];
				Viewpoint * const p1	= _viewpoints[ offset - 1 ];
				Viewpoint * const p2	= _viewpoints[ Util::Math::min<int>( (int)size - 1, offset ) ];
				Viewpoint * const p3	= _viewpoints[ Util::Math::min<int>( (int)size - 1, offset + 1 ) ];
				double			  value = 1.0 - ( ( total - p_time ) / p2->getDuration() );

				viewpoint.setPosition( Util::Math::catmullRomInterpolation( p0->getPosition(), p1->getPosition(), p2->getPosition(), p3->getPosition(), value ) );
				viewpoint.setRotation( Util::Math::catmullRomInterpolation( p0->getRotation(), p1->getRotation(), p2->getRotation(), p3->getRotation(), value ) );
				viewpoint.setTarget( Util::Math::catmullRomInterpolation( p0->getTarget(), p1->getTarget(), p2->getTarget(), p3->getTarget(), value ) );
				viewpoint.setDistance( Util::Math::linearInterpolation( p1->getDistance(), p2->getDistance(), value ) );
				viewpoint.setController( p1->getController() == ID::Controller::TRACKBALL && p2->getController() == ID::Controller::TRACKBALL

											 ? ID::Controller::TRACKBALL
											 : ID::Controller::FREEFLY );
			}
			else if ( _modeInterpolation == INTERPOLATION_MODE::CUBIC )
			{
				Viewpoint * const p0	= _viewpoints[ Util::Math::max<int>( 0, (int)offset - 2 ) ];
				Viewpoint * const p1	= _viewpoints[ offset - 1 ];
				Viewpoint * const p2	= _viewpoints[ Util::Math::min<int>( (int)size - 1, offset ) ];
				Viewpoint * const p3	= _viewpoints[ Util::Math::min<int>( (int)size - 1, offset + 1 ) ];
				double			  value = 1.0 - ( ( total - p_time ) / p2->getDuration() );

				viewpoint.setPosition( Util::Math::cubicInterpolation( p0->getPosition(), p1->getPosition(), p2->getPosition(), p3->getPosition(), value ) );
				viewpoint.setRotation( Util::Math::cubicInterpolation( p0->getRotation(), p1->getRotation(), p2->getRotation(), p3->getRotation(), value ) );
				viewpoint.setTarget( Util::Math::cubicInterpolation( p0->getTarget(), p1->getTarget(), p2->getTarget(), p3->getTarget(), value ) );
				viewpoint.setDistance( Util::Math::linearInterpolation( p1->getDistance(), p2->getDistance(), value ) );
				viewpoint.setController( p1->getController() == ID::Controller::TRACKBALL && p2->getController() == ID::Controller::TRACKBALL

											 ? ID::Controller::TRACKBALL
											 : ID::Controller::FREEFLY );
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
			if ( _modeDuration == DURATION_MODE::PATH )
			{
				float duration = 0.f;
				if ( _viewpoints.size() >= 2 )
				{
					duration = _duration / (float)( _viewpoints.size() - 1 );
				}

				for ( uint i = 1; i < _viewpoints.size(); ++i )
				{
					Viewpoint * const viewpoint = _viewpoints[ i ];
					viewpoint->setDuration( duration );
				}
			}
			// Set the path duration from viewpoint durations.
			else if ( _modeDuration == DURATION_MODE::VIEWPOINT )
			{
				_duration = 0.f;
				for ( uint i = 1; i < _viewpoints.size(); ++i )
				{
					Viewpoint * const viewpoint = _viewpoints[ i ];
					_duration += viewpoint->getDuration();
				}
			}
			// Set viewport duration from path duration/distance.
			else if ( _modeDuration == DURATION_MODE::CONSTANT_SPEED )
			{
				// Compute total distance.
				float totalDistance = 0.f;
				uint  size			= (uint)Util::Math::max<int>( (int)_viewpoints.size() - 1, 0 );
				for ( uint i = 0; i < size; ++i )
				{
					totalDistance += (float)Util::Math::distance( _viewpoints[ i ]->getPosition(), _viewpoints[ i + 1u ]->getPosition() );
				}

				// Compute viewpoint durations.
				for ( uint i = 1; i < _viewpoints.size(); ++i )
				{
					Viewpoint * const viewpoint = _viewpoints[ i ];
					if ( totalDistance == 0.f )
					{
						viewpoint->setDuration( 0.f );
						break;
					}
					float distance = (float)Util::Math::distance( _viewpoints[ i - 1u ]->getPosition(), viewpoint->getPosition() );
					viewpoint->setDuration( _duration * distance / totalDistance );
				}
			}
		} // namespace Model

		void Path::setSelected( const bool p_selected )
		{
			BaseSelectable::setSelected( p_selected );
			if ( isSelected() )
			{
				// addItem( (View::BaseView<BaseModel> *)new View::UI::Path( this ) );
			}
			else
			{
				// delete removeItem( ID::View::UI_PATH );
			}
		}

	} // namespace Model
} // namespace VTX
