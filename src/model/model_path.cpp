#include "model_path.hpp"
#include "exception.hpp"
#include "util/math.hpp"
#include "util/type.hpp"
#include "view/view_ui_path.hpp"
#include "view/view_ui_path_list.hpp"
#include <fstream>
#include <sstream>

namespace VTX
{
	namespace Model
	{
		ModelPath::~ModelPath() { Util::Type::clearVector( _checkpoints ); }

		void ModelPath::_addItems() { addItem( (View::BaseView<BaseModel> *)( new View::ViewUIPathList( this ) ) ); }

		ModelCheckpoint::CheckpointInterpolationData ModelPath::getCurrentCheckpointInterpolationData(
			float p_time ) const
		{
			float total	 = 0.f;
			uint  offset = 0;

			for ( ; offset < _checkpoints.size(); ++offset )
			{
				Model::ModelCheckpoint * checkpoint = _checkpoints[ offset ];
				if ( offset >= 1 ) { total += checkpoint->getDuration(); }
				if ( total >= p_time ) { break; }
			}

			ModelCheckpoint::CheckpointInterpolationData data = ModelCheckpoint::CheckpointInterpolationData(
				_checkpoints[ offset > 0 ? offset - 1 : 0 ]->getPosition(),
				_checkpoints[ offset > 0 ? offset - 1 : 0 ]->getRotation(),
				_checkpoints[ offset ]->getPosition(),
				_checkpoints[ offset ]->getRotation(),
				1.f - ( ( -( p_time - total ) ) / _checkpoints[ offset ]->getDuration() ) );

			return data;
		}

		void ModelPath::load( const IO::Path & p_file )
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
			Util::Type::clearVector( _checkpoints );

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

				addCheckpoint( new ModelCheckpoint( this, position, rotation, duration ) );
			}

			chrono.stop();
			VTX_INFO( "Import finished in " + std::to_string( chrono.elapsedTime() ) + " seconds" );
		}

		void ModelPath::save( const IO::Path & p_file ) const
		{
			VTX_INFO( "Exporting " + std::to_string( _checkpoints.size() ) + " view points in " + p_file.c_str() );
			std::ofstream file;
			file.open( p_file, std::ios::out | std::ios::trunc );

			if ( !file.is_open() )
				throw Exception::VTXException( "ModelPath::exportPath: cannot open file " + p_file.str() );

			Tool::Chrono chrono = Tool::Chrono();
			chrono.start();
			file << _checkpoints.size() << std::endl;
			for ( Model::ModelCheckpoint * checkpoint : _checkpoints )
			{
				const Vec3f & p = checkpoint->getPosition();
				const Quatf & r = checkpoint->getRotation();
				const float & d = checkpoint->getDuration();
				file << p.x << " " << p.y << " " << p.z << " " << r.x << " " << r.y << " " << r.z << " " << r.w << " "
					 << d << std::endl;
			}
			chrono.stop();
			VTX_INFO( "Export finished in " + std::to_string( chrono.elapsedTime() ) + " seconds" );

			file.close();
		}

		void ModelPath::setSelected( const bool p_selected )
		{
			BaseModel::setSelected( p_selected );
			if ( isSelected() ) { addItem( (View::BaseView<BaseModel> *)( new View::ViewUIPath( this ) ) ); }
			else
			{
				_deleteView( ID::View::UI_PATH );
			}
		}

		void ModelPath::setSelectedCheckpoint( ModelCheckpoint * const p_checkpoint )
		{
			if ( _selectedCheckpoint != nullptr ) { _selectedCheckpoint->setSelected( false ); }
			try
			{
				_selectedCheckpoint = p_checkpoint;
				_selectedCheckpoint->setSelected( true );
			}
			catch ( const std::exception )
			{
				VTX_WARNING( "Failed to select checkpoint" );
				_selectedCheckpoint = nullptr;
			}
		}

		float ModelPath::computeTotalTime() const
		{
			float total = 0.f;
			if ( _checkpoints.size() == 0 ) { return total; }
			for ( Model::ModelCheckpoint * checkpoint : _checkpoints )
			{
				total += checkpoint->getDuration();
			}
			total -= _checkpoints[ 0 ]->getDuration();
			return total;
		} // namespace Model

	} // namespace Model
} // namespace VTX
