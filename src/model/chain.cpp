#include "chain.hpp"
#include "view/ui/chain.hpp"

namespace VTX
{
	namespace Model
	{
		void Chain::setSelected( const bool p_selected )
		{
			BaseModel::setSelected( p_selected );
			if ( isSelected() )
			{ addItem( (View::BaseView<BaseModel> *)Generic::create<Chain, View::UI::Chain>( this ) ); }
			else
			{
				Generic::destroy( removeItem( ID::View::UI_CHAIN ) );
			}
		}
	} // namespace Model
} // namespace VTX
