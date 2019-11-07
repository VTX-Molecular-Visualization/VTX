#ifndef __VTX_RESIDUE__
#define __VTX_RESIDUE__

#include "../defines.hpp"
#include <map>

namespace VTX
{
	namespace Model
	{
		enum class RESIDUE_TYPE
		{
			ALA = 0,
			ARG,
			ASN,
			ASP,
			CYS,
			GLN,
			GLU,
			GLY,
			HIS,
			ILE,
			LEU,
			LYS,
			MET,
			PHE,
			PRO,
			SER,
			THR,
			TRP,
			TYR,
			VAL,
			HOH, /// TODO: keep water as a residue ?
			// keep at last
			INVALID_TYPE,
			RESIDUE_TYPE_COUNT
		};

		class Residue
		{
		  public:
			uint		 getId() const { return _id; };
			void		 setId( const uint p_id ) { _id = p_id; };
			RESIDUE_TYPE getType() const { return _type; };
			void setType( const RESIDUE_TYPE p_type ) { _type = p_type; };

		  private:
			uint		 _id   = INVALID_ID;
			RESIDUE_TYPE _type = RESIDUE_TYPE::INVALID_TYPE;
		};

	} // namespace Model
} // namespace VTX
#endif
