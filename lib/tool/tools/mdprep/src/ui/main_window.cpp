#include "tools/mdprep/ui/main_window.hpp"

namespace VTX::Tool::Mdprep::ui
{
	constexpr const char * string( const E_MD_ENGINE & p_ ) noexcept
	{
		switch ( p_ )
		{
		case E_MD_ENGINE::gromacs: return { "Gromacs" };
		default: break;
		}
		return "Please provide a user string for the MD engine"; // For developers that add new MD Engine support
	}
	namespace
	{
		constexpr std::array<const char *, MD_ENGINE_NUMBER> createMdEngineStringList()
		{
			std::array<const char *, MD_ENGINE_NUMBER> out;
			for ( int i = 0; i < MD_ENGINE_NUMBER; i++ )
			{
				out[ i ] = string( static_cast<E_MD_ENGINE>( i ) );
			}
			return out;
		}
	} // namespace
	constexpr const std::array<const char *, MD_ENGINE_NUMBER> g_mdEngineStrings = createMdEngineStringList();

	const std::array<const char *, MD_ENGINE_NUMBER> & mdEngineStrings() { return g_mdEngineStrings; }
} // namespace VTX::Tool::Mdprep::ui
