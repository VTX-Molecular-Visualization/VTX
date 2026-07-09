#include "app/action/representation.hpp"
#include "app/helper/preset.hpp"
#include "app/system/representation.hpp"
#include "app/system/uid.hpp"
#include <core/struct/topology.hpp>
#include <util/exceptions.hpp>

namespace VTX::App::Action::Representation
{
	void AddItem::execute(
		const Entity						 p_ent,
		const Core::Struct::E_SYSTEM_ITEM	 p_item,
		const std::string_view				 p_presetName,
		const Core::Struct::IndexRangeList & p_ranges
	)
	{
		const std::optional<Entity> preset = Helper::Preset::getByName<Renderer::Representation>( p_presetName );
		if ( not preset )
		{
			throw VTXException( "Representation preset not found: " + std::string( p_presetName ) );
		}
		execute( p_ent, p_item, *preset, p_ranges );
	}

	void AddSelected::execute( const std::string_view p_presetName )
	{
		const std::optional<Entity> preset = Helper::Preset::getByName<Renderer::Representation>( p_presetName );
		if ( not preset )
		{
			throw VTXException( "Representation preset not found: " + std::string( p_presetName ) );
		}
		execute( *preset );
	}
} // namespace VTX::App::Action::Representation
