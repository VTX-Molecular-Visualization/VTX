#ifndef VTX_TOOL_MDPREP_UI_MD_ENGINE_FACTORY_HPP
#define VTX_TOOL_MDPREP_UI_MD_ENGINE_FACTORY_HPP

#include <array>

namespace VTX::Tool::Mdprep::ui
{

	enum class E_MD_ENGINE : uint8_t
	{
		gromacs,
		COUNT
	};
	constexpr const size_t MD_ENGINE_NUMBER = static_cast<size_t>( E_MD_ENGINE::COUNT );

	constexpr const char * string( const E_MD_ENGINE & p_ ) noexcept
	{
		switch ( p_ )
		{
		case E_MD_ENGINE::gromacs: return "Gromacs";
		default: break;
		}
		return "DEBUUUG"; // For developers that add new MD Engine support
	}

	const std::array<const char *, MD_ENGINE_NUMBER> & mdEngineStrings();

	class MdEngine;
	void get( const E_MD_ENGINE &, MdEngine & );
} // namespace VTX::Tool::Mdprep::ui

#endif
