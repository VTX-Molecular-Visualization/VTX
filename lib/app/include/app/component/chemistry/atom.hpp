#ifndef __VTX_APP_COMPONENT_CHEMISTRY_ATOM__
#define __VTX_APP_COMPONENT_CHEMISTRY_ATOM__

#include "_fwd.hpp"
#include "app/core/ecs/base_component.hpp"
#include <core/chemdb/atom.hpp>
#include <string>
#include <util/constants.hpp>
#include <util/types.hpp>
#include <core/struct/frames_data_circbuff_prodcons.hpp>

namespace VTX::App::Component::Chemistry
{
	namespace ChemDB = VTX::Core::ChemDB;

	class Atom : public Core::ECS::BaseComponent
	{
	  public:
		Atom() = default;
		Atom( Residue * const p_residue );
		Atom( System * const p_system ) : _systemPtr( p_system ) {};
		Atom( System * const p_system, const atom_index_t p_index ) :
			_systemPtr( p_system ), _index( p_index ) {};

		atom_index_t			getIndex() const { return _index; }
		void					setIndex( const atom_index_t p_index ) { _index = p_index; }
		Residue *				getResiduePtr() const;
		const Residue *			getConstResiduePtr() const;
		void					setResiduePtr( Residue * const p_residue );
		Chain *					getChainPtr() const;
		const Chain *			getConstChainPtr() const;
		inline System *		getSystemPtr() const { return _systemPtr; }
		inline const System * getConstSystemPtr() const { return _systemPtr; }

		const std::string &			 getName() const;
		void						 setName( const std::string & p_name );
		const ChemDB::Atom::SYMBOL & getSymbol() const;
		void						 setSymbol( const ChemDB::Atom::SYMBOL & p_symbol );

		ChemDB::Atom::TYPE getType() const;
		void			   setType( const ChemDB::Atom::TYPE p_type );

		float getVdwRadius() const;

		// TODO devjla
		const Vec3f & getLocalPosition() const;
		Vec3f		  getWorldPosition() const;

		bool isVisible() const;
		void setVisible( const bool p_visible );

		void remove();

	  private:
		// Store systemPtr or use residuePtr->getChainPtr()->getSystemPtr() ?
		System *	 _systemPtr = nullptr;
		atom_index_t _index		  = INVALID_ATOM_INDEX;
	};

} // namespace VTX::App::Component::Chemistry
#endif
