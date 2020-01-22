#include "model_chain.hpp"
#include "../util/type.hpp"

namespace VTX
{
	namespace Model
	{
		void ModelChain::setSelected( const bool p_selected )
		{
			BaseModel::setSelected( p_selected );
			if ( isSelected() )
			{
				//_addView( Util::Type::componentToView<ModelChain>( UI::COMPONENT_NAME::VIEW_CHAIN ) );
			}
			else
			{
				//_removeView( std::string( magic_enum::enum_name( UI::COMPONENT_NAME::VIEW_CHAIN ) ) );
			}
		}
	} // namespace Model
} // namespace VTX
