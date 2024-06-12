#include "tool/core/serialization/ui_core_serializers.hpp"
#include <app/application/system/serializer.hpp>
#include <string>
#include <util/exceptions.hpp>

namespace VTX::App::Internal::Serialization
{
	Util::JSon::Object serialize( const UI::Core::LayoutDescriptor & p_component ) { throw NotImplementedException(); }

	void deserialize( const Util::JSon::Object & p_json, UI::Core::LayoutDescriptor & p_layoutDescriptor )
	{
		const Util::JSon::Array & tools = p_json[ "TOOLS" ];

		for ( const Util::JSon::Object & jsonItem : tools )
		{
			UI::Core::ToolDescriptor descriptor;
			deserialize( jsonItem, descriptor );
			p_layoutDescriptor.addTool( descriptor );
		}
	}

	Util::JSon::Object serialize( const UI::Core::ToolDescriptor & p_component ) { throw NotImplementedException(); }

	void deserialize( const Util::JSon::Object & p_json, UI::Core::ToolDescriptor & p_toolDescriptor )
	{
		const std::string toolIdentifier = App::SERIALIZER().deserializeField<std::string>( p_json, "IDENTIFIER" );
		p_toolDescriptor.setIdentifier( toolIdentifier );
	}

} // namespace VTX::App::Internal::Serialization
