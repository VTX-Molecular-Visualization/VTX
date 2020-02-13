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
			if ( isSelected() )
			{ addItem( (View::BaseView<BaseModel> *)Generic::FactoryView<Chain, View::UI::Chain>::create( this ) ); }
			else
			{
				Generic::FactoryView<Chain, View::UI::Chain>::destroy(
					(View::UI::Chain *)removeItem( ID::View::UI_CHAIN ) );
			}
		}
	} // namespace Model
} // namespace VTX
