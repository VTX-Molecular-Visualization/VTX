#ifndef __VTX_APP_COMPONENT_CHEMISTRY_CHAIN__
#define __VTX_APP_COMPONENT_CHEMISTRY_CHAIN__

#include "_fwd.hpp"
#include <core/struct/chain.hpp>
#include <util/color/rgba.hpp>

namespace VTX::App::Component::Chemistry
{
	class Chain : public Core::Struct::Chain
	{
	  public:
		Chain() : Core::Struct::Chain() {};
		Chain( Molecule * const p_molecule, const size_t p_internalIndex );

		void setOriginalChainID( const std::string & p_chainId ) { _originalChainID = p_chainId; }
		void setColor( const Util::Color::Rgba & p_defaultColor ) { _defaultColor = p_defaultColor; };

	  private:
		std::string		  _originalChainID = "";
		Util::Color::Rgba _defaultColor;
	};
} // namespace VTX::App::Component::Chemistry
#endif
