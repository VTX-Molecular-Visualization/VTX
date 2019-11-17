#ifndef __CHAIN_HPP__
#define __CHAIN_HPP__

#include "../defines.hpp"
#include <iostream>

	namespace Model
	{
		class ModelMolecule;
		class ModelChain
		{
		  public:
			inline ModelMolecule * const getMoleculePtr() const { return _moleculePtr; }
			inline void setMoleculePtr( ModelMolecule * const p_molecule ) { _moleculePtr = p_molecule; }

			inline uint				   getId() const { return _id; };
			inline void				   setId( const uint p_id ) { _id = p_id; };
			inline const std::string & getName() const { return _name; };
			inline void				   setName( const std::string & p_name ) { _name = p_name; };
			inline uint				   getIdFirstResidue() const { return _idFirstResidue; };
			inline void				   setIdFirstResidue( const uint p_id ) { _idFirstResidue = p_id; };
			inline uint				   getResidueCount() const { return _residueCount; };
			inline void				   setResidueCount( const uint p_count ) { _residueCount = p_count; };

		  private:
			ModelMolecule * _moleculePtr = nullptr;

			uint		_id				= INVALID_ID;
			std::string _name			= "";
			uint		_idFirstResidue = 0;
			uint		_residueCount	= 0;
		};

	} // namespace Model
#endif
