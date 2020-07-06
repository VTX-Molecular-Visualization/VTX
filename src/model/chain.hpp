#ifndef __VTX_MODEL_CHAIN__
#define __VTX_MODEL_CHAIN__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_model.hpp"
#include "define.hpp"
#include "generic/base_colorable.hpp"
#include "generic/base_representable.hpp"
#include "generic/base_visible.hpp"
#include <iostream>

namespace VTX
{
	namespace Model
	{
		class Molecule;
		class Chain :
			public BaseModel,
			public Generic::BaseColorable,
			public Generic::BaseVisible,
			public Generic::BaseRepresentable
		{
		  public:
			enum class TYPE : int
			{
				STANDARD,
				NON_STANDARD
			};

			// one color per chain id + 1 unknown
			static const uint		NB_COLORS = 26;
			static const Color::Rgb CHAIN_ID_COLOR_ATOM[ NB_COLORS ];
			static const Color::Rgb CHAIN_ID_COLOR_HETATM[ NB_COLORS ];
			static const Color::Rgb CHAIN_ID_UNKNOWN_COLOR;

			// inline TYPE				getType() const { return _type; }
			// inline void				setType( const TYPE p_type ) { _type = p_type; }
			inline uint				getIndex() const { return _index; };
			inline void				setIndex( const uint p_index ) { _index = p_index; };
			inline Molecule * const getMoleculePtr() const { return _moleculePtr; }
			inline void				setMoleculePtr( Molecule * const p_molecule ) { _moleculePtr = p_molecule; }

			static Color::Rgb getChainIdColor( const std::string & p_chainId, const bool p_isHetAtm = false );

			inline const std::string & getName() const { return _name; };
			inline void				   setName( const std::string & p_name ) { _name = p_name; };
			inline const std::string & getSequence() const { return _sequence; };
			inline uint				   getIndexFirstResidue() const { return _indexFirstResidue; };
			inline void				   setIndexFirstResidue( const uint p_id ) { _indexFirstResidue = p_id; };
			inline uint				   getResidueCount() const { return _residueCount; };
			inline void				   setResidueCount( const uint p_count ) { _residueCount = p_count; };

			virtual void setSelected( const bool ) override;
			virtual void addRepresentation( const Generic::REPRESENTATION ) override;
			virtual void removeRepresentation( const Generic::REPRESENTATION ) override;

			void computeSequence();

		  private:
			// TYPE	   _type		= TYPE::STANDARD;
			uint	   _index		= 0;
			Molecule * _moleculePtr = nullptr;

			std::string _name			   = "unknown";
			std::string _sequence		   = "";
			uint		_indexFirstResidue = 0;
			uint		_residueCount	   = 0;
		}; // namespace Model

	} // namespace Model
} // namespace VTX
#endif
