#ifndef __VTX_APP_COMPONENT_CHEMISTRY_ATOM__
#define __VTX_APP_COMPONENT_CHEMISTRY_ATOM__

#include "_fwd.hpp"
#include "app/core/model/base_model.hpp"
#include "app/internal/chemdb/atom.hpp"
#include "app/old_app/generic/base_colorable.hpp"
#include "app/old_app/generic/base_visible.hpp"
#include "app/id.hpp"
#include "app/component/object3d/helper/aabb.hpp"
#include <util/types.hpp>

namespace VTX::App::Component::Chemistry
{
	namespace ChemDB = App::Internal::ChemDB;

	class Atom : public App::Core::Model::BaseModel, public Generic::BaseVisible
	{
		VTX_MODEL

	  public:
		inline const uint	   getIndex() const { return _index; };
		inline void			   setIndex( const uint p_index ) { _index = p_index; };
		Molecule * const	   getMoleculePtr() const;
		Chain * const		   getChainPtr() const;
		inline Residue * const getResiduePtr() const { return _residuePtr; }
		inline void			   setResiduePtr( Residue * const p_residue ) { _residuePtr = p_residue; }

		inline const Util::Color::Rgba & getColor() const { return ChemDB::Atom::SYMBOL_COLOR[ int( _symbol ) ]; };

		inline const ChemDB::Atom::SYMBOL getSymbol() const { return _symbol; };
		inline const std::string &		  getSymbolStr() const { return ChemDB::Atom::SYMBOL_STR[ (int)_symbol ]; };
		inline void						  setSymbol( const ChemDB::Atom::SYMBOL p_symbol )
		{
			_symbol = p_symbol;
			BaseModel::setDefaultName( &getSymbolName() );
		};
		inline const std::string & getSymbolName() const { return ChemDB::Atom::SYMBOL_NAME[ (int)_symbol ]; }
		inline const uint		   getAtomicNumber() const { return (uint)_symbol; }
		inline const float		   getVdwRadius() const { return ChemDB::Atom::SYMBOL_VDW_RADIUS[ (int)_symbol ]; }
		inline ChemDB::Atom::TYPE  getType() const { return _type; }
		inline void				   setType( const ChemDB::Atom::TYPE p_type ) { _type = p_type; }
		inline const std::string & getName() const { return _name; };
		inline void				   setName( const std::string & p_name ) { _name = p_name; };

		// Mask BaseVisible::setVisible
		void setVisible( const bool p_visible );
		void setVisible( const bool p_visible, const bool p_notify );

		const Vec3f &				 getLocalPosition() const;
		const Vec3f					 getWorldPosition() const;
		const App::Component::Object3D::Helper::AABB getAABB() const;
		const App::Component::Object3D::Helper::AABB getWorldAABB() const;

	  private:
		uint				 _index		 = 0;
		Residue *			 _residuePtr = nullptr;
		ChemDB::Atom::TYPE	 _type		 = ChemDB::Atom::TYPE::NORMAL;
		ChemDB::Atom::SYMBOL _symbol	 = ChemDB::Atom::SYMBOL::UNKNOWN;
		// /!\ Names PDB != MMTF (CA and C1 for alpha carbon).
		std::string _name = "";

		Atom() : BaseModel( App::ID::Model::MODEL_ATOM ) {}
	};

} // namespace VTX::App::Component::Chemistry
#endif
