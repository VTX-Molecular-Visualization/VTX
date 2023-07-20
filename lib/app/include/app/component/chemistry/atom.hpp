#ifndef __VTX_APP_COMPONENT_CHEMISTRY_ATOM__
#define __VTX_APP_COMPONENT_CHEMISTRY_ATOM__

#include "_fwd.hpp"
#include <core/chemdb/atom.hpp>
#include <string>
#include <util/constants.hpp>
#include <util/types.hpp>

namespace VTX::App::Component::Chemistry
{
	namespace ChemDB = VTX::Core::ChemDB;

	class Atom
	{
	  public:
		Atom() = default;
		Atom( Residue * const p_residue );
		Atom( Molecule * const p_molecule ) : _moleculePtr( p_molecule ) {};
		Atom( Molecule * const p_molecule, const size_t p_index ) : _moleculePtr( p_molecule ), _index( p_index ) {};

		size_t			getIndex() const { return _index; }
		void			setIndex( const size_t p_index ) { _index = p_index; }
		Residue *		getResiduePtr() const { return _residuePtr; }
		const Residue * getConstResiduePtr() const { return _residuePtr; }
		void			setResiduePtr( Residue * p_residue ) { _residuePtr = p_residue; }

		const std::string &			 getName() const;
		void						 setName( const std::string & p_name );
		const ChemDB::Atom::SYMBOL & getSymbol() const;
		void						 setSymbol( const ChemDB::Atom::SYMBOL & p_symbol );

		ChemDB::Atom::TYPE getType() const;
		void			   setType( const ChemDB::Atom::TYPE p_type );

		float getVdwRadius() const;

		const Vec3f & getLocalPosition() const;
		const Vec3f & getLocalPosition( const size_t & p_frameIndex ) const;
		Vec3f		  getWorldPosition() const;
		Vec3f		  getWorldPosition( const size_t & p_frameIndex ) const;
		bool		  isVisible();
		void		  setVisible( const bool p_visible );

	  private:
		// Store moleculePtr or use residuePtr->getChainPtr()->getMoleculePtr() ?
		size_t	   _index		= INVALID_INDEX;
		Molecule * _moleculePtr = nullptr;
		Residue *  _residuePtr	= nullptr;
	};

} // namespace VTX::App::Component::Chemistry
#endif
