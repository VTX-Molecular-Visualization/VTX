#include "app/helper/scene.hpp"
#include "app/system/uid.hpp"
#include <io/metadata.hpp>
#include <util/string.hpp>

namespace VTX::App::Helper::Scene
{
	std::vector<Entity> getAllSystems()
	{
		const auto view = REG().view<System::UID>();
		return { view.begin(), view.end() };
	}

	Entity getSystemByName( const std::string_view p_name )
	{
		const std::string name = Util::String::toLower( std::string( p_name ) );

		for ( auto e : REG().view<IO::Metadata>() )
		{
			auto & metadata = REG().get<IO::Metadata>( e );

			if ( Util::String::toLower( metadata.name ) == name )
			{
				return e;
			}
		}

		return InvalidEntity;
	}

	Entity getSystemByPdb( const std::string_view p_pdb )
	{
		const std::string pdb = Util::String::toLower( std::string( p_pdb ) );

		for ( auto e : REG().view<IO::Metadata>() )
		{
			auto & metadata = REG().get<IO::Metadata>( e );

			if ( Util::String::toLower( metadata.pdbIDCode ) == pdb )
			{
				return e;
			}
		}

		return InvalidEntity;
	}

	Entity getSystemByFileName( const std::string_view p_fileName )
	{
		const std::string fileName = Util::String::toLower( std::string( p_fileName ) );

		for ( auto e : REG().view<IO::Metadata>() )
		{
			const auto &   metadata = REG().get<IO::Metadata>( e );
			const FilePath filename = metadata.path.filename();
			if ( Util::String::toLower( filename.string() ) == fileName
				 || Util::String::toLower( filename.stem().string() ) == fileName )
			{
				return e;
			}
		}
		return InvalidEntity;
	}

	std::optional<Entity> findSystemByAtomUID( const UID32 p_uid )
	{
		const auto view = REG().view<System::UID>();
		for ( const auto & [ entity, uid ] : view.each() )
		{
			if ( uid.containsAtomUID( p_uid ) )
			{
				return entity;
			}
		}

		return std::nullopt;
	}

	std::optional<Entity> findSystemByResidueUID( const UID32 p_uid )
	{
		const auto view = REG().view<System::UID>();
		for ( const auto & [ entity, uid ] : view.each() )
		{
			if ( uid.containsResidueUID( p_uid ) )
			{
				return entity;
			}
		}

		return std::nullopt;
	}

} // namespace VTX::App::Helper::Scene
