#include "viewpoint.hpp"
#include "view/ui/viewpoint.hpp"

namespace VTX
{
	namespace Model
	{
		void Viewpoint::setSelected( const bool p_selected )
		{
			BaseSelectable::setSelected( p_selected );
			if ( isSelected() )
			{
				addItem( (View::BaseView<BaseModel> *)new View::UI::Viewpoint( this ) );
			}
			else
			{
				delete removeItem( ID::View::UI_VIEWPOINT );
			}
		}

	} // namespace Model
} // namespace VTX
