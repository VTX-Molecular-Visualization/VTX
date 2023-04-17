#ifndef __VTX_MODEL_BOND__
#define __VTX_MODEL_BOND__

#include "base_model.hpp"
#include "app/old_app/id.hpp"
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
			enum class ORDER : char
			{
				UNKNOWN	   = 0, ///< Bond order is unknown or unspecified
				SINGLE	   = 1, ///< Single bond
				DOUBLE	   = 2, ///< Double bond
				TRIPLE	   = 3, ///< Triple bond
				QUADRUPLE  = 4, ///< Quadruplet bond
				QUINTUPLET = 5, ///< Quintuplet bond
				AROMATIC   = 6, ///< Aromatic bond
				COUNT,

				// space for more bond types if needed
				// DOWN = 250, ///< Single bond direction from first atom to second is 'down'. Used for cis-trans
				// isomers UP	 = 251, ///< Single bond direction from first atom to second is 'up'. Used for cis-trans
				// isomers DATIVE_R = 252, ///< Dative bond where the electrons are localized to the first atom DATIVE_L
				// = 253, ///< Dative bond where the electrons are localized to the second atom
				// AMIDE	 = 254, ///< Amide bond (C(=O)-NH)
				// AROMATIC = 255, ///< Aromatic bond (for example the ring bonds in benzene)
			};

			inline static const std::string TYPE_STR[ int( ORDER::COUNT ) ] { "Unknown", "Simple",	  "Double",
																			  "Triple",	 "Quadruple", "Quintuple" };

			static bool comparer( const Bond & p_lhs, const Bond & p_rhs );

			inline uint getIndexFirstAtom() const { return _indexFirstAtom; }
			inline void setIndexFirstAtom( const uint p_index ) { _indexFirstAtom = p_index; }
			inline uint getIndexSecondAtom() const { return _indexSecondAtom; }
			inline void setIndexSecondAtom( const uint p_index ) { _indexSecondAtom = p_index; }

			inline ORDER getOrder() const { return _order; }
			inline void	 setOrder( const ORDER p_order ) { _order = p_order; }

			inline Molecule * const getMoleculePtr() const { return _moleculePtr; }
			inline void				setMoleculePtr( Molecule * const p_molecule ) { _moleculePtr = p_molecule; }

		  private:
			uint	   _indexFirstAtom	= 0;
			uint	   _indexSecondAtom = 0;
			Molecule * _moleculePtr		= nullptr;
			ORDER	   _order			= ORDER::UNKNOWN;

			Bond() : BaseModel( VTX::ID::Model::MODEL_BOND ) {};
		};

	} // namespace Model
} // namespace VTX
#endif
