#include "viewpoint.hpp"
#include "../view/ui/viewpoint.hpp"

namespace VTX
{
	namespace Model
	{
		uint Viewpoint::COUNTER = 0;

		void Viewpoint::setSelected( const bool p_selected )
		{
			BaseModel::setSelected( p_selected );
			if ( isSelected() ) { addItem( (View::BaseView<BaseModel> *)( new View::UI::Viewpoint( this ) ) ); }
			else
			{
				_deleteView( ID::View::UI_VIEWPOINT );
			}
		}
	} // namespace Model
} // namespace VTX
