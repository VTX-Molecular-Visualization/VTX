#ifndef __VTX_CORE_STRUCT_CHAIN__
#define __VTX_CORE_STRUCT_CHAIN__

#include "_fwd.hpp"
#include <string>
#include <util/color/rgba.hpp>
#include <util/types.hpp>

namespace VTX::Core::Old::Struct
{
	class Chain
	{
	  public:
		static Util::Color::Rgba getChainIdColor( const std::string & p_chainId, const bool p_isHetAtm = false );

	  public:
		Chain() {}

		// inline CHAIN_TYPE	getType() const { return _type; }
		// inline void			setType( const CHAIN_TYPE p_type ) { _type = p_type; }
		inline uint				getIndex() const { return _index; };
		inline void				setIndex( const uint p_index ) { _index = p_index; };
		inline Molecule * const getMoleculePtr() const { return _moleculePtr; }
		void					setMoleculePtr( Molecule * const p_molecule );

		const std::string & getOriginalChainID() const { return _originalChainID; };
		void				setOriginalChainID( const std::string & p_chainId ) { _originalChainID = p_chainId; };

		inline const std::string & getName() const { return _name; };
		inline void				   setName( const std::string & p_name ) { _name = p_name; };
		inline uint				   getIndexFirstResidue() const { return _indexFirstResidue; };
		inline void				   setIndexFirstResidue( const uint p_id ) { _indexFirstResidue = p_id; };
		inline uint				   getResidueCount() const { return _residueCount; };
		void					   setResidueCount( const uint p_count );
		uint					   getRealResidueCount() const { return _realResidueCount; };
		void					   removeToResidue( const uint p_residueIndex );

		uint computeRealAtomCount() const;

		inline uint getIndexLastResidue() const { return _indexFirstResidue + _residueCount - 1; };

	  private:
		// CHAIN_TYPE	   _type		= CHAIN_TYPE::STANDARD;

		uint		_index			 = 0;
		Molecule *	_moleculePtr	 = nullptr;
		std::string _originalChainID = "";

		std::string _name			   = "unknown";
		uint		_indexFirstResidue = 0;
		uint		_residueCount	   = 0;
		uint		_realResidueCount  = 0;
	};

} // namespace VTX::Core::Old::Struct
#endif
