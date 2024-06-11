#ifndef __VTX_UI_CORE_SERIALIZATION_UI_CORE_SERIALIZERS__
#define __VTX_UI_CORE_SERIALIZATION_UI_CORE_SERIALIZERS__

#include "ui/core/layout_descriptor.hpp"
#include <util/json/json.hpp>
#include <vector>

// Use VTX::App::Internal::Serialization namespace to activate all serializers directly in Serialization
namespace VTX::App::Internal::Serialization
{
	Util::JSon::Object serialize( const UI::Core::LayoutDescriptor & p_component );
	void			   deserialize( const Util::JSon::Object & p_json, UI::Core::LayoutDescriptor & p_component );

	Util::JSon::Object serialize( const UI::Core::ToolDescriptor & p_component );
	void			   deserialize( const Util::JSon::Object & p_json, UI::Core::ToolDescriptor & p_toolDescriptor );

} // namespace VTX::App::Internal::Serialization

#endif
