#include "checkpoint.hpp"
#include "../view/ui/checkpoint.hpp"

namespace VTX
{
	namespace Model
	{
		uint Checkpoint::COUNTER = 0;

		void Checkpoint::setSelected( const bool p_selected )
		{
			BaseModel::setSelected( p_selected );
			if ( isSelected() ) { addItem( (View::BaseView<BaseModel> *)( new View::UI::Checkpoint( this ) ) ); }
			else
			{
				_deleteView( ID::View::UI_CHECKPOINT );
			}
		}
	} // namespace Model
} // namespace VTX
