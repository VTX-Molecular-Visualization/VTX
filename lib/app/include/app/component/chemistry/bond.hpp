#ifndef __VTX_APP_COMPONENT_CHEMISTRY_BOND__
#define __VTX_APP_COMPONENT_CHEMISTRY_BOND__

#include "_fwd.hpp"
#include <util/constants.hpp>

namespace VTX::App::Component::Chemistry
{
	class Bond
	{
	  public:
		Bond() {}
		Bond( Molecule * const p_moleculePtr ) : _moleculePtr( p_moleculePtr ) {}
		Bond( Molecule * const p_moleculePtr, const size_t p_index ) : _moleculePtr( p_moleculePtr ), _index( p_index )
		{
		}

		const Molecule * const getMoleculePtr() const { return _moleculePtr; }
		Molecule * const	   getMoleculePtr() { return _moleculePtr; }

		size_t getIndexFirstAtom() const;
		void   setIndexFirstAtom( const size_t p_atomIndex );

		size_t getIndexSecondAtom() const;
		void   setIndexSecondAtom( const size_t p_atomIndex );

	  private:
		Molecule * _moleculePtr = nullptr;
		size_t	   _index		= INVALID_INDEX;
	};

} // namespace VTX::App::Component::Chemistry
#endif
