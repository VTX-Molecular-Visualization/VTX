#include "chain.hpp"
#include "util/type.hpp"
#include "view/ui/chain.hpp"

namespace VTX
{
	namespace Model
	{
		void Chain::setSelected( const bool p_selected )
		{
			BaseModel::setSelected( p_selected );
			if ( isSelected() ) { addItem( (View::BaseView<BaseModel> *)( new View::UI::Chain( this ) ) ); }
			else
			{
				_deleteView( ID::View::UI_CHAIN );
			}
		}
	} // namespace Model
} // namespace VTX
