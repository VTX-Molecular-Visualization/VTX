#ifndef __VTX_APP_COMPONENT_CHEMISTRY_CHAIN__
#define __VTX_APP_COMPONENT_CHEMISTRY_CHAIN__

#include "_fwd.hpp"
#include "define.hpp"
#include <util/color/rgba.hpp>

namespace VTX::App::Component::Chemistry
{
	class Chain : public ChainCore
	{
	  public:
		Chain() : ChainCore() {};
		Chain( Molecule * const p_molecule, const size_t p_internalIndex ) :
			ChainCore( p_molecule, p_internalIndex ) {};

		void setOriginalChainID( const std::string & p_chainId ) { _originalChainID = p_chainId; }
		void setColor( const Util::Color::Rgba & p_defaultColor ) { _defaultColor = p_defaultColor; };

	  protected:
	  private:
		std::string		  _originalChainID = "";
		Util::Color::Rgba _defaultColor;
	};
} // namespace VTX::App::Component::Chemistry
#endif
