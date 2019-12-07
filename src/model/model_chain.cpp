#include "model_chain.hpp"
#include "../util/type.hpp"

namespace VTX
{
	namespace Model
	{
		void ModelChain::setSelected( const bool p_selected )
		{
			_isSelected = p_selected;
			if ( isSelected() )
			{ _addView( Util::Type::componentToView<ModelChain>( UI::COMPONENT_NAME::VIEW_CHAIN ) ); }
			else
			{
				_clearViews();
			}
		}
	} // namespace Model
} // namespace VTX
