#include <array>
#include <string>
#include <thread>
//
#include <ui/qt/util.hpp>
//
#include "tool/mdprep/gateway/form_data.hpp"
#include "tool/mdprep/gateway/shared.hpp"
//
#include "tool/mdprep/gateway/backend_gromacs.hpp"
#include "tool/mdprep/ui/shared.hpp"
//
#include "tool/mdprep/ui/gromacs.hpp"
//
#include "tool/mdprep/ui/md_engine_field_placer.hpp"
#include "tool/mdprep/ui/md_engine_specific_field_placer.hpp"
//
#include "tool/mdprep/ui/md_engine.hpp"
//
#include "tool/mdprep/ui/md_engine_factory.hpp"

namespace VTX::Tool::Mdprep::ui
{

	constexpr const char * string( const E_MD_ENGINE & p_ ) noexcept
	{
		switch ( p_ )
		{
		case E_MD_ENGINE::gromacs: return { "Gromacs" };
		default: break;
		}
		return "DEBUUUG"; // For developers that add new MD Engine support
	}
	namespace
	{
		constexpr std::array<const char *, MD_ENGINE_NUMBER> createMdEngineStringList()
		{
			std::array<const char *, MD_ENGINE_NUMBER> out;
			for ( size_t i = 0; i < MD_ENGINE_NUMBER; i++ )
			{
				out[ i ] = string( static_cast<E_MD_ENGINE>( i ) );
			}
			return out;
		}
	} // namespace
	constexpr const std::array<const char *, MD_ENGINE_NUMBER> g_mdEngineStrings = createMdEngineStringList();

	const std::array<const char *, MD_ENGINE_NUMBER> & mdEngineStrings() { return g_mdEngineStrings; }
	void											   get( const E_MD_ENGINE & p_engine, MdEngine & p_out ) noexcept
	{
		switch ( p_engine )
		{
		case E_MD_ENGINE::gromacs: p_out = MdEngine( Gromacs() ); break;
		default: break;
		}
	}

} // namespace VTX::Tool::Mdprep::ui
