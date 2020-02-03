#include "model_path.hpp"
#include "util/math.hpp"
#include "util/type.hpp"
#include "view/view_ui_path_list.hpp"
namespace VTX
{
	namespace Model
	{
		ModelPath::~ModelPath() { Util::Type::clearVector( _checkpoints ); }

		void ModelPath::_addItems() { addItem( (View::BaseView<BaseModel> *)( new View::ViewUIPathList( this ) ) ); }

		/*
		Object3D::Camera::CameraConfiguration ModelPath::getCurrentCameraConfiguration( float p_time ) const
		{
			float total	 = 0.f;
			uint  offset = 0;

			for ( ; offset < _checkpoints.size(); ++offset )
			{
				Model::ModelCheckpoint * checkpoint = _checkpoints[ offset ];
				if ( offset >= 1 ) { total += checkpoint->getDuration(); }
				if ( total >= p_time ) { break; }
			}

			float value = 1.f - ( ( -( p_time - total ) ) / _checkpoints[ offset ]->getDuration() );

			return Util::Math::lerpCameraConfiguration( (ModelPath * const)this, offset, value );
		}
		*/

		void ModelPath::setSelectedCheckpoint( const uint p_id )
		{
			if ( _selectedCheckpoint != nullptr ) { _selectedCheckpoint->setSelected( false ); }
			try
			{
				_selectedCheckpoint = _checkpoints.at( p_id );
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
			for ( Model::ModelCheckpoint * checkpoint : _checkpoints )
			{
				total += checkpoint->getDuration();
			}
			total -= _checkpoints[ 0 ]->getDuration();
			return total;
		} // namespace Model

	} // namespace Model
} // namespace VTX
