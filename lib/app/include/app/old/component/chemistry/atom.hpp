#ifndef __VTX_APP_OLD_COMPONENT_CHEMISTRY_ATOM__
#define __VTX_APP_OLD_COMPONENT_CHEMISTRY_ATOM__

#include "_fwd.hpp"
#include "app/old/component/generic/base_visible.hpp"
#include "app/old/component/object3d/helper/aabb.hpp"
#include "app/old/core/model/base_model.hpp"
#include "app/old/id.hpp"
#include <core/chemdb/atom.hpp>
#include <core/old/struct/atom.hpp>
#include <util/types.hpp>

namespace VTX::App::Old::Component::Chemistry
{
	namespace ChemDB = VTX::Core::ChemDB;

	class Atom : public Core::Model::BaseModel, public Component::Generic::BaseVisible
	{
		VTX_MODEL

	  public:
		inline VTX::Core::Old::Struct::Atom &	   getAtomStruct() { return *_atomStruct; }
		inline const VTX::Core::Old::Struct::Atom & getAtomStruct() const { return *_atomStruct; }

		inline const uint	   getIndex() const { return _atomStruct->getIndex(); };
		inline void			   setIndex( const uint p_index ) { _atomStruct->setIndex( p_index ); };
		Molecule * const	   getMoleculePtr() const;
		Chain * const		   getChainPtr() const;
		inline Residue * const getResiduePtr() const { return _residuePtr; }
		void				   setResiduePtr( Residue * const p_residue );

		inline const Util::Color::Rgba & getColor() const { return _atomStruct->getColor(); };

		inline const ChemDB::Atom::SYMBOL getSymbol() const { return _atomStruct->getSymbol(); };
		inline const std::string &		  getSymbolStr() const { return _atomStruct->getSymbolStr(); };
		void							  setSymbol( const ChemDB::Atom::SYMBOL p_symbol );
		inline const std::string &		  getSymbolName() const { return _atomStruct->getSymbolName(); }
		inline const uint				  getAtomicNumber() const { return _atomStruct->getAtomicNumber(); }
		inline const float				  getVdwRadius() const { return _atomStruct->getVdwRadius(); }
		inline ChemDB::Atom::TYPE		  getType() const { return _atomStruct->getType(); }
		inline void						  setType( const ChemDB::Atom::TYPE p_type ) { _atomStruct->setType( p_type ); }
		inline const std::string &		  getName() const { return _atomStruct->getName(); };
		inline void						  setName( const std::string & p_name ) { _atomStruct->setName( p_name ); };

		// Mask BaseVisible::setVisible
		void setVisible( const bool p_visible );
		void setVisible( const bool p_visible, const bool p_notify );

		const Vec3f &								 getLocalPosition() const;
		const Vec3f									 getWorldPosition() const;
		const App::Old::Component::Object3D::Helper::AABB getAABB() const;
		const App::Old::Component::Object3D::Helper::AABB getWorldAABB() const;

	  protected:
		Atom() : BaseModel( App::Old::ID::Model::MODEL_ATOM ) {}

	  private:
		VTX::Core::Old::Struct::Atom * _atomStruct = nullptr;
		Residue *				  _residuePtr = nullptr;
	};

} // namespace VTX::App::Old::Component::Chemistry
#endif
