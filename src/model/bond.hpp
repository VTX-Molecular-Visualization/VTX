#ifndef __VTX_MODEL_BOND__
#define __VTX_MODEL_BOND__

#include "base_model.hpp"
#include "id.hpp"

namespace VTX
{
	namespace Model
	{
		class Molecule;
		class Bond : public BaseModel
		{
			VTX_MODEL

		  public:
			inline uint				getIndexFirstAtom() const { return _indexFirstAtom; };
			inline void				setIndexFirstAtom( const uint p_index ) { _indexFirstAtom = p_index; };
			inline uint				getIndexSecondAtom() const { return _indexSecondAtom; };
			inline void				setIndexSecondAtom( const uint p_index ) { _indexSecondAtom = p_index; };
			inline Molecule * const getMoleculePtr() const { return _moleculePtr; }
			inline void				setMoleculePtr( Molecule * const p_molecule ) { _moleculePtr = p_molecule; }

		  private:
			uint	   _indexFirstAtom	= 0;
			uint	   _indexSecondAtom = 0;
			Molecule * _moleculePtr		= nullptr;

			Bond() : BaseModel( ID::Model::MODEL_BOND ) {};
		};

	} // namespace Model
} // namespace VTX
#endif
