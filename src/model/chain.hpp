#ifndef __VTX_MODEL_CHAIN__
#define __VTX_MODEL_CHAIN__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_model.hpp"
#include "define.hpp"
#include "generic/base_colorable.hpp"
#include <iostream>

namespace VTX
{
	namespace Model
	{
		class Molecule;
		class Chain : public BaseModel, public Generic::BaseColorable
		{
		  public:
			inline uint				getIndex() const { return _index; };
			inline void				setIndex( const uint p_index ) { _index = p_index; };
			inline Molecule * const getMoleculePtr() const { return _moleculePtr; }
			inline void				setMoleculePtr( Molecule * const p_molecule ) { _moleculePtr = p_molecule; }

			inline const std::string & getName() const { return _name; };
			inline void				   setName( const std::string & p_name ) { _name = p_name; };
			inline uint				   getIdFirstResidue() const { return _idFirstResidue; };
			inline void				   setIdFirstResidue( const uint p_id ) { _idFirstResidue = p_id; };
			inline uint				   getResidueCount() const { return _residueCount; };
			inline void				   setResidueCount( const uint p_count ) { _residueCount = p_count; };

			virtual void setSelected( const bool ) override;

		  private:
			uint	   _index		= 0;
			Molecule * _moleculePtr = nullptr;

			std::string _name			= "unknown";
			uint		_idFirstResidue = 0;
			uint		_residueCount	= 0;
		}; // namespace Model

	} // namespace Model
} // namespace VTX
#endif
