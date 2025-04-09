#ifndef __VTX_PYTHON_API_ATOM__

#include <concepts>
#include <core/chemdb/atom.hpp>
#include <memory>
#include <util/types.hpp>

namespace VTX::PythonBinding::API
{
	class Residue;
	class Chain;
	class System;

	/**
	 * @brief Class responsible for defining an atom's facade from Python's perspective.
	 */
	class Atom
	{
	  public:
		Atom() = default;

		atom_index_t getIndex() const
		{
			if ( _ptr )
				return _ptr->getIndex();
			return 0xffffffff;
		}
		void setIndex( const atom_index_t p_index )
		{
			if ( _ptr )
				_ptr->setIndex( p_index );
		}
		Residue * getResiduePtr() const
		{
			if ( _ptr )
				return _ptr->getResiduePtr();
			return nullptr;
		}
		const Residue * getConstResiduePtr() const
		{
			if ( _ptr )
				return _ptr->getConstResiduePtr();
			return nullptr;
		}
		void setResiduePtr( Residue * const p_residue )
		{
			if ( _ptr )
				_ptr->setResiduePtr( p_residue );
		}
		Chain * getChainPtr() const
		{
			if ( _ptr )
				return _ptr->getChainPtr();
			return nullptr;
		}
		const Chain * getConstChainPtr() const
		{
			if ( _ptr )
				return _ptr->getConstChainPtr();
			return nullptr;
		}
		inline System * getSystemPtr() const
		{
			if ( _ptr )
				return _ptr->getSystemPtr();
			return nullptr;
		}
		inline const System * getConstSystemPtr() const
		{
			if ( _ptr )
				return _ptr->getConstSystemPtr();
			return nullptr;
		}

		const std::string & getName() const
		{
			if ( _ptr )
				return _ptr->getName();
			return {};
		}
		void setName( const std::string & p_name )
		{
			if ( _ptr )
				_ptr->setName( p_name );
		}
		const Core::ChemDB::Atom::SYMBOL & getSymbol() const
		{
			if ( _ptr )
				return _ptr->getSymbol();
			return Core::ChemDB::Atom::SYMBOL::UNKNOWN;
		}
		void setSymbol( const Core::ChemDB::Atom::SYMBOL & p_symbol )
		{
			if ( _ptr )
				_ptr->setSymbol( p_symbol );
		}

		Core::ChemDB::Atom::TYPE getType() const
		{
			if ( _ptr )
				return _ptr->getType();
			return Core::ChemDB::Atom::TYPE::NORMAL; // Not a huge fan of returning NORMAL by default, but heh
		}
		void setType( const Core::ChemDB::Atom::TYPE p_type )
		{
			if ( _ptr )
				_ptr->setType( p_type );
		}

		float getVdwRadius() const
		{
			if ( _ptr )
				return _ptr->getVdwRadius();
			return 0.f;
		}

		const Vec3f & getLocalPosition() const
		{
			if ( _ptr )
				return _ptr->getLocalPosition();
			return {};
		}
		Vec3f getWorldPosition() const
		{
			if ( _ptr )
				return _ptr->getWorldPosition();
			return {};
		}

		bool isVisible() const
		{
			if ( _ptr )
				return _ptr->isVisible();
			return false;
		}
		void setVisible( const bool p_visible )
		{
			if ( _ptr )
				_ptr->setVisible( p_visible );
		}

		void remove()
		{
			if ( _ptr )
				_ptr->remove();
		}

	  private:
		struct _interface
		{
			virtual ~_interface() = default;

			virtual atom_index_t		  getIndex() const							 = 0;
			virtual void				  setIndex( const atom_index_t p_index )	 = 0;
			virtual Residue *			  getResiduePtr() const						 = 0;
			virtual const Residue *		  getConstResiduePtr() const				 = 0;
			virtual void				  setResiduePtr( Residue * const p_residue ) = 0;
			virtual Chain *				  getChainPtr() const						 = 0;
			virtual const Chain *		  getConstChainPtr() const					 = 0;
			virtual inline System *		  getSystemPtr() const						 = 0;
			virtual inline const System * getConstSystemPtr() const					 = 0;

			virtual const std::string &				   getName() const											= 0;
			virtual void							   setName( const std::string & p_name )					= 0;
			virtual const Core::ChemDB::Atom::SYMBOL & getSymbol() const										= 0;
			virtual void							   setSymbol( const Core::ChemDB::Atom::SYMBOL & p_symbol ) = 0;

			virtual Core::ChemDB::Atom::TYPE getType() const								  = 0;
			virtual void					 setType( const Core::ChemDB::Atom::TYPE p_type ) = 0;

			virtual float getVdwRadius() const = 0;

			virtual const Vec3f & getLocalPosition() const = 0;
			virtual Vec3f		  getWorldPosition() const = 0;

			virtual bool isVisible() const					= 0;
			virtual void setVisible( const bool p_visible ) = 0;

			virtual void remove() = 0;
		};

		template<class T>
		class _wrapper final
		{
			T & _obj;

		  public:
			_wrapper( T & p_ ) : _obj( p_ ) {}

			atom_index_t		  getIndex() const { return _obj.getIndex(); }
			void				  setIndex( const atom_index_t p_index ) { _obj.setIndex( p_index ); }
			Residue *			  getResiduePtr() const { return _obj.getResiduePtr(); }
			const Residue *		  getConstResiduePtr() const { return _obj.getConstResiduePtr(); }
			void				  setResiduePtr( Residue * const p_residue ) { _obj.setResiduePtr( p_residue ); }
			Chain *				  getChainPtr() const { return _obj.getChainPtr(); }
			const Chain *		  getConstChainPtr() const { return _obj.getConstChainPtr(); }
			inline System *		  getSystemPtr() const { return _obj.getSystemPtr(); }
			inline const System * getConstSystemPtr() const { return _obj.getConstSystemPtr(); }

			const std::string &				   getName() const { return _obj.getName(); }
			void							   setName( const std::string & p_name ) { _obj.setName( p_name ); }
			const Core::ChemDB::Atom::SYMBOL & getSymbol() const { return _obj.getSymbol(); }
			void setSymbol( const Core::ChemDB::Atom::SYMBOL & p_symbol ) { _obj.setSymbol( p_symbol ); }

			Core::ChemDB::Atom::TYPE getType() const { return _obj.getType(); }
			void					 setType( const Core::ChemDB::Atom::TYPE p_type ) { _obj.setType( p_type ); }

			float getVdwRadius() const { return _obj.getVdwRadius(); }

			const Vec3f & getLocalPosition() const { return _obj.getLocalPosition(); }
			Vec3f		  getWorldPosition() const { return _obj.getWorldPosition(); }

			bool isVisible() const { return _obj.isVisible(); }
			void setVisible( const bool p_visible ) { _obj.setVisible( p_visible ); }

			void remove() { _obj.remove(); }
		};

		std::shared_ptr<_interface> _ptr
			= nullptr; // We want the atom to be copyable at it will point toward the same entity eventually
	  public:
		template<class T>
			requires( not std::same_as<std::remove_cvref<T>, Atom> )
		Atom( T & p_ ) : _ptr( new _wrapper<T>( p_ ) )
		{
			static_assert(
				requires( const T t ) {
					{ t.getIndex() } -> std::same_as<atom_index_t>;
				}, "Missing |atom_index_t getIndex() const| class method."
			);
			static_assert(
				requires( const T t, const atom_index_t & idx ) {
					{ t.setIndex( idx ) };
				}, "Missing |void setIndex( const atom_index_t p_index )| class method."
			);
			// TODO : The rest of the static assertion to help maintainance, when we're sure this design works
		}
	};
} // namespace VTX::PythonBinding::API

#endif
