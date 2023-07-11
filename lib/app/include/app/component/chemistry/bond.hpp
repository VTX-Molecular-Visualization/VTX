#ifndef __VTX_APP_COMPONENT_CHEMISTRY_BOND__
#define __VTX_APP_COMPONENT_CHEMISTRY_BOND__

#include "_fwd.hpp"
#include "define.hpp"

namespace VTX::App::Component::Chemistry
{
	class Bond : public BondCore
	{
	  public:
		Bond() : BondCore() {}
		Bond( Molecule * const p_moleculePtr ) : BondCore( p_moleculePtr ) {}
		Bond( Molecule * const p_moleculePtr, const size_t p_index ) : BondCore( p_moleculePtr, p_index ) {}

		size_t getIndexFirstAtom() const;
		void   setIndexFirstAtom( const size_t p_atomIndex );

		size_t getIndexSecondAtom() const;
		void   setIndexSecondAtom( const size_t p_atomIndex );
	};

} // namespace VTX::App::Component::Chemistry
#endif
