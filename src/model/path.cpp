#include "path.hpp"
#include "exception.hpp"
#include "generic/factory.hpp"
#include "view/ui/path.hpp"
#include "view/ui/path_list.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>

namespace VTX
{
	namespace Model
	{
		Path::~Path() { Generic::clearVector( _viewpoints ); }

		void Path::_addItems()
		{
			addItem( (View::BaseView<BaseModel> *)Generic::create<Path, View::UI::PathList>( this ) );
		}

		const std::vector<std::string> * const Path::getCurrentActions( const float p_time )
		{
			Viewpoint viewpoint( (Path * const)this );

			uint  size	 = (uint)_viewpoints.size();
			float total	 = 0.f;
			uint  offset = 0;

			// Find the next and previous points.
			while ( total <= p_time && offset < size )
			{
				total += _viewpoints[ offset++ ]->getDuration();
			}
			offset--;
			return &_viewpoints[ offset - 1 ]->getActions();
		}

		Model::Viewpoint Path::getInterpolatedViewpoint( const float p_time ) const
		{
			Viewpoint viewpoint( (Path * const)this );

			uint  size	 = (uint)_viewpoints.size();
			float total	 = 0.f;
			uint  offset = 0;

			// Find the next and previous points.
			while ( total <= p_time && offset < size )
			{
				total += _viewpoints[ offset++ ]->getDuration();
			}
			offset--;

			if ( _modeInterpolation == INNTERPOLATION_MODE::LINEAR )
			{
				// Computes value.
				Viewpoint * const p0	= _viewpoints[ offset - 1 ];
				Viewpoint * const p1	= _viewpoints[ Util::Math::min<int>( (int)size - 1, offset ) ];
				float			  value = 1.f - ( ( total - p_time ) / p1->getDuration() );

				// Lerp.
				viewpoint.setPosition( Util::Math::linearInterpolation( p0->getPosition(), p1->getPosition(), value ) );
				viewpoint.setRotation( Util::Math::linearInterpolation( p0->getRotation(), p1->getRotation(), value ) );
			}
			else if ( _modeInterpolation == INNTERPOLATION_MODE::CATMULL_ROM )
			{
				Viewpoint * const p0	= _viewpoints[ Util::Math::max<int>( 0, (int)offset - 2 ) ];
				Viewpoint * const p1	= _viewpoints[ offset - 1 ];
				Viewpoint * const p2	= _viewpoints[ Util::Math::min<int>( (int)size - 1, offset ) ];
				Viewpoint * const p3	= _viewpoints[ Util::Math::min<int>( (int)size - 1, offset + 1 ) ];
				float			  value = 1.f - ( ( total - p_time ) / p2->getDuration() );

				viewpoint.setPosition( Util::Math::catmullRomInterpolation(
					p0->getPosition(), p1->getPosition(), p2->getPosition(), p3->getPosition(), value ) );
				viewpoint.setRotation( Util::Math::catmullRomInterpolation(
					p0->getRotation(), p1->getRotation(), p2->getRotation(), p3->getRotation(), value ) );
			}
			else if ( _modeInterpolation == INNTERPOLATION_MODE::CUBIC )
			{
				Viewpoint * const p0	= _viewpoints[ Util::Math::max<int>( 0, (int)offset - 2 ) ];
				Viewpoint * const p1	= _viewpoints[ offset - 1 ];
				Viewpoint * const p2	= _viewpoints[ Util::Math::min<int>( (int)size - 1, offset ) ];
				Viewpoint * const p3	= _viewpoints[ Util::Math::min<int>( (int)size - 1, offset + 1 ) ];
				float			  value = 1.f - ( ( total - p_time ) / p2->getDuration() );

				viewpoint.setPosition( Util::Math::cubicInterpolation(
					p0->getPosition(), p1->getPosition(), p2->getPosition(), p3->getPosition(), value ) );
				viewpoint.setRotation( Util::Math::cubicInterpolation(
					p0->getRotation(), p1->getRotation(), p2->getRotation(), p3->getRotation(), value ) );
			}

			return viewpoint;
		}

		void Path::refreshAllDurations()
		{
			// Force the first to 0.
			if ( _viewpoints.size() > 0 ) { _viewpoints[ 0 ]->setDuration( 0.f ); }

			// Set same duration for each viewpoint.
			if ( _modeDuration == DURATION_MODE::PATH )
			{
				float duration = 0.f;
				if ( _viewpoints.size() >= 2 ) { duration = _duration / (float)( _viewpoints.size() - 1 ); }

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
					totalDistance += Util::Math::distance( _viewpoints[ i ]->getPosition(),
														   _viewpoints[ i + 1u ]->getPosition() );
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
					float distance
						= Util::Math::distance( _viewpoints[ i - 1u ]->getPosition(), viewpoint->getPosition() );
					viewpoint->setDuration( _duration * distance / totalDistance );
				}
			}
		} // namespace Model

		void Path::setSelected( const bool p_selected )
		{
			BaseModel::setSelected( p_selected );
			if ( isSelected() )
			{ addItem( (View::BaseView<BaseModel> *)Generic::create<Path, View::UI::Path>( this ) ); }
			else
			{
				Generic::destroy( removeItem( ID::View::UI_PATH ) );
			}
		}

		void Path::load( const IO::Path & p_file )
		{
			VTX_INFO( "Importing view points from " + p_file.str() );
			std::ifstream file;
			file.open( p_file );

			if ( !file.is_open() )
				throw Exception::VTXException( "ModelPath::importPath: cannot open file " + p_file.str() );

			Tool::Chrono chrono = Tool::Chrono();
			chrono.start();
			std::string		   line;
			std::istringstream iss;

			int	  nbViewPoints = 0;
			float x = 1, y = 1, z = 1, w = 1;
			Vec3f position;
			Quatf rotation;
			float duration;
			// read first line to get the number of view points
			std::getline( file, line );
			iss.str( line );
			iss >> nbViewPoints;
			Generic::clearVector( _viewpoints );

			for ( int i = 0; i < nbViewPoints; ++i )
			{
				std::getline( file, line );
				iss.clear();
				iss.str( line );

				iss >> position.x;
				iss >> position.y;
				iss >> position.z;
				// get rotation
				iss >> rotation.x;
				iss >> rotation.y;
				iss >> rotation.z;
				iss >> rotation.w;
				// get duration
				iss >> duration;

				addViewpoint( new Viewpoint( this, position, rotation, duration ) );

				std::string action;
				while ( iss.eof() == false )
				{
					iss >> action;
					std::replace( action.begin(), action.end(), '-', ' ' );
					getViewpoints()[ i ]->addAction( action );
				}
			}
			refreshAllDurations();
			chrono.stop();
			VTX_INFO( "Import finished in " + std::to_string( chrono.elapsedTime() ) + " seconds" );
		}

		void Path::save( const IO::Path & p_file ) const
		{
			VTX_INFO( "Exporting " + std::to_string( _viewpoints.size() ) + " view points in " + p_file.c_str() );
			std::ofstream file;
			file.open( p_file, std::ios::out | std::ios::trunc );

			if ( !file.is_open() )
				throw Exception::VTXException( "ModelPath::exportPath: cannot open file " + p_file.str() );

			Tool::Chrono chrono = Tool::Chrono();
			chrono.start();
			file << _viewpoints.size() << std::endl;
			for ( Model::Viewpoint * viewpoint : _viewpoints )
			{
				const Vec3f & p = viewpoint->getPosition();
				const Quatf & r = viewpoint->getRotation();
				const float & d = viewpoint->getDuration();
				file << p.x << " " << p.y << " " << p.z << " " << r.x << " " << r.y << " " << r.z << " " << r.w << " "
					 << d;

				for ( std::string action : viewpoint->getActions() )
				{
					std::replace( action.begin(), action.end(), ' ', '-' );
					file << " " << action;
				}

				file << std::endl;
			}
			chrono.stop();
			VTX_INFO( "Export finished in " + std::to_string( chrono.elapsedTime() ) + " seconds" );

			file.close();
		}

	} // namespace Model
} // namespace VTX
