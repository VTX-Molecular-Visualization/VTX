#ifndef __VTX_CHAIN__
#define __VTX_CHAIN__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"
#include "base_model.hpp"
#include <iostream>

namespace VTX
{
	namespace Model
	{
		class ModelMolecule;
		class ModelChain : public BaseModel
		{
		  public:
			inline ModelMolecule * const getMoleculePtr() const { return _moleculePtr; }
			inline void setMoleculePtr( ModelMolecule * const p_molecule ) { _moleculePtr = p_molecule; }

			inline const std::string & getName() const { return _name; };
			inline void				   setName( const std::string & p_name ) { _name = p_name; };
			inline float * const	   getColor() { return _color; }
			inline void				   setColor( const Vec3f & p_color )
			{
				_color[ 0 ] = p_color.x;
				_color[ 1 ] = p_color.y;
				_color[ 2 ] = p_color.z;
			}
			inline uint getIdFirstResidue() const { return _idFirstResidue; };
			inline void setIdFirstResidue( const uint p_id ) { _idFirstResidue = p_id; };
			inline uint getResidueCount() const { return _residueCount; };
			inline void setResidueCount( const uint p_count ) { _residueCount = p_count; };

			virtual void setSelected( const bool ) override;

		  private:
			ModelMolecule * _moleculePtr = nullptr;

			std::string _name			= "";
			float		_color[ 3 ]		= { 0.f, 0.f, 0.f };
			uint		_idFirstResidue = 0;
			uint		_residueCount	= 0;
		}; // namespace Model

	} // namespace Model
} // namespace VTX
#endif
