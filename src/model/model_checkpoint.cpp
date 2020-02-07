#include "model_checkpoint.hpp"
#include "../view/view_ui_checkpoint.hpp"

namespace VTX
{
	namespace Model
	{
		uint ModelCheckpoint::COUNTER = 0;

		void ModelCheckpoint::setSelected( const bool p_selected )
		{
			BaseModel::setSelected( p_selected );
			if ( isSelected() ) { addItem( (View::BaseView<BaseModel> *)( new View::ViewUICheckpoint( this ) ) ); }
			else
			{
				_deleteView( ID::View::UI_CHECKPOINT );
			}
		}
	} // namespace Model
} // namespace VTX
