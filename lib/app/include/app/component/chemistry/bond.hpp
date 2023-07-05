#ifndef __VTX_APP_COMPONENT_CHEMISTRY_BOND__
#define __VTX_APP_COMPONENT_CHEMISTRY_BOND__

#include "_fwd.hpp"
#include <core/struct/bond.hpp>

namespace VTX::App::Component::Chemistry
{
	class Bond : public Core::Struct::Bond
	{
	  public:
		Bond() : Core::Struct::Bond() {}
		Bond( Molecule * const p_moleculePtr );
		Bond( Molecule * const p_moleculePtr, const size_t p_index );

		const Molecule * const getMoleculePtr() const;
		Molecule * const	   getMoleculePtr();

		size_t getIndexFirstAtom() const;
		void   setIndexFirstAtom( const size_t p_atomIndex );

		size_t getIndexSecondAtom() const;
		void   setIndexSecondAtom( const size_t p_atomIndex );
	};

} // namespace VTX::App::Component::Chemistry
#endif
