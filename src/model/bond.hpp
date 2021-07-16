#ifndef __VTX_MODEL_BOND__
#define __VTX_MODEL_BOND__

#include "base_model.hpp"
#include "id.hpp"
#include <string>

namespace VTX
{
	namespace Model
	{
		class Molecule;
		class Bond : public BaseModel
		{
			VTX_MODEL

		  public:
			enum class TYPE
			{
				UNKNOWN	   = 0, ///< Bond order is unknown or unspecified
				SINGLE	   = 1, ///< Single bond
				DOUBLE	   = 2, ///< Double bond
				TRIPLE	   = 3, ///< Triple bond
				QUADRUPLE  = 4, ///< Quadruplet bond
				QUINTUPLET = 5, ///< Quintuplet bond
				COUNT,

				// space for more bond types if needed
				DOWN = 250, ///< Single bond direction from first atom to second is 'down'. Used for cis-trans isomers
				UP	 = 251, ///< Single bond direction from first atom to second is 'up'. Used for cis-trans isomers
				DATIVE_R = 252, ///< Dative bond where the electrons are localized to the first atom
				DATIVE_L = 253, ///< Dative bond where the electrons are localized to the second atom
				AMIDE	 = 254, ///< Amide bond (C(=O)-NH)
				AROMATIC = 255, ///< Aromatic bond (for example the ring bonds in benzene)
			};

			inline static const std::string TYPE_STR[ int( TYPE::COUNT ) ] { "Unknown", "Simple",	 "Double",
																			 "Triple",	"Quadruple", "Quintuple" };

			inline uint getIndexFirstAtom() const { return _indexFirstAtom; };
			inline void setIndexFirstAtom( const uint p_index ) { _indexFirstAtom = p_index; };
			inline uint getIndexSecondAtom() const { return _indexSecondAtom; };
			inline void setIndexSecondAtom( const uint p_index ) { _indexSecondAtom = p_index; };

			inline TYPE getBondType() const { return _type; };
			inline void setBondType( const TYPE p_type ) { _type = p_type; };

			inline Molecule * const getMoleculePtr() const { return _moleculePtr; }
			inline void				setMoleculePtr( Molecule * const p_molecule ) { _moleculePtr = p_molecule; }

		  private:
			uint	   _indexFirstAtom	= 0;
			uint	   _indexSecondAtom = 0;
			TYPE	   _type			= TYPE::UNKNOWN;
			Molecule * _moleculePtr		= nullptr;

			Bond() : BaseModel( ID::Model::MODEL_BOND ) {};
		};

	} // namespace Model
} // namespace VTX
#endif
