#ifndef __VTX_PYTHON_API_ATOM__
#define __VTX_PYTHON_API_ATOM__

#include <core/chemdb/atom.hpp>
#include <memory>
#include <util/concepts.hpp>
#include <util/types.hpp>

namespace VTX::PythonBinding::API
{

	template<class T>
	struct is_unique_ptr : std::false_type
	{
	};
	template<class T, class D>
	struct is_unique_ptr<std::unique_ptr<T, D>> : std::true_type
	{
	};
	template<class T>
	struct is_unique_ptr<std::unique_ptr<T>> : std::true_type
	{
	};

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

		inline atom_index_t getIndex() const
		{
			if ( _ptr )
				return _ptr->getIndex();
			return 0xffffffff;
		}
		inline void setIndex( const atom_index_t p_index )
		{
			if ( _ptr )
				_ptr->setIndex( p_index );
		}
		Residue		  getResidue();
		const Residue getResidue() const;
		Chain		  getChain();
		const Chain	  getChain() const;
		System		  getSystem();
		const System  getSystem() const;

		inline const std::string & getName() const
		{
			if ( _ptr )
				return _ptr->getName();
			static const std::string _; // Ugly af but does the job
			return _;
		}
		inline void setName( const std::string & p_name )
		{
			if ( _ptr )
				_ptr->setName( p_name );
		}
		inline const Core::ChemDB::Atom::SYMBOL & getSymbol() const
		{
			if ( _ptr )
				return _ptr->getSymbol();
			static const Core::ChemDB::Atom::SYMBOL _ = Core::ChemDB::Atom::SYMBOL::UNKNOWN;
			return _;
		}
		inline void setSymbol( const Core::ChemDB::Atom::SYMBOL & p_symbol )
		{
			if ( _ptr )
				_ptr->setSymbol( p_symbol );
		}

		inline Core::ChemDB::Atom::TYPE getType() const
		{
			if ( _ptr )
				return _ptr->getType();
			return Core::ChemDB::Atom::TYPE::NORMAL; // Not a huge fan of returning NORMAL by default, but heh
		}
		inline void setType( const Core::ChemDB::Atom::TYPE p_type )
		{
			if ( _ptr )
				_ptr->setType( p_type );
		}

		inline float getVdwRadius() const
		{
			if ( _ptr )
				return _ptr->getVdwRadius();
			return 0.f;
		}

		inline const Vec3f & getLocalPosition() const
		{
			if ( _ptr )
				return _ptr->getLocalPosition();
			static const Vec3f _ { 0.f, 0.f, 0.f };
			return _;
		}
		inline Vec3f getWorldPosition() const
		{
			if ( _ptr )
				return _ptr->getWorldPosition();
			return {};
		}

		inline bool isVisible() const
		{
			if ( _ptr )
				return _ptr->isVisible();
			return false;
		}
		inline void setVisible( const bool p_visible )
		{
			if ( _ptr )
				_ptr->setVisible( p_visible );
		}

		inline void remove()
		{
			if ( _ptr )
				_ptr->remove();
		}

	  private:
		struct _interface
		{
			virtual ~_interface() = default;

			virtual atom_index_t  getIndex() const						 = 0;
			virtual void		  setIndex( const atom_index_t p_index ) = 0;
			virtual Residue		  getResidue()							 = 0;
			virtual const Residue getResidue() const					 = 0;
			virtual Chain		  getChain()							 = 0;
			virtual const Chain	  getChain() const						 = 0;
			virtual System		  getSystem()							 = 0;
			virtual const System  getSystem() const						 = 0;

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

		template<class T, typename Residue = Residue, typename Chain = Chain, typename System = System>
		class _wrapper final : public _interface
		{
			T & _obj;
			using Obj = std::remove_pointer<T>::type;
			inline Obj & obj()
			{
				if constexpr ( std::is_pointer<T>::value )
					return *_obj;
				else
					return _obj;
			}
			inline const Obj & obj() const
			{
				if constexpr ( std::is_pointer<T>::value )
					return *_obj;
				else
					return _obj;
			}

		  public:
			_wrapper( T & p_ ) : _obj( p_ ) {}

			virtual atom_index_t getIndex() const override { return obj().getIndex(); }
			virtual void		 setIndex( const atom_index_t p_index ) override
			{
				if constexpr ( not std::is_const<T>::value )
					obj().setIndex( p_index );
			}
			virtual Residue getResidue() override
			{
				if constexpr ( not std::is_const<T>::value )
					return { *obj().getResiduePtr() };
				else
					return {};
			}
			virtual const Residue getResidue() const override { return { *obj().getConstResiduePtr() }; }
			virtual Chain		  getChain() override
			{
				if constexpr ( not std::is_const<T>::value )
					return { *obj().getChainPtr() };
				else
					return {};
			}
			virtual const Chain getChain() const override { return { *obj().getConstChainPtr() }; }
			virtual System		getSystem() override
			{
				if constexpr ( not std::is_const<T>::value )
					return { *obj().getSystemPtr() };
				else
					return {};
			}
			virtual const System getSystem() const override { return { *obj().getConstSystemPtr() }; }

			virtual const std::string & getName() const override { return obj().getName(); }
			virtual void				setName( const std::string & p_name ) override
			{
				if constexpr ( not std::is_const<T>::value )
					obj().setName( p_name );
			}
			virtual const Core::ChemDB::Atom::SYMBOL & getSymbol() const override { return obj().getSymbol(); }
			virtual void							   setSymbol( const Core::ChemDB::Atom::SYMBOL & p_symbol ) override
			{
				if constexpr ( not std::is_const<T>::value )
					obj().setSymbol( p_symbol );
			}

			virtual Core::ChemDB::Atom::TYPE getType() const override { return obj().getType(); }
			virtual void					 setType( const Core::ChemDB::Atom::TYPE p_type ) override
			{
				if constexpr ( not std::is_const<T>::value )
					obj().setType( p_type );
			}

			virtual float getVdwRadius() const override { return obj().getVdwRadius(); }

			virtual const Vec3f & getLocalPosition() const override { return obj().getLocalPosition(); }
			virtual Vec3f		  getWorldPosition() const override { return obj().getWorldPosition(); }

			virtual bool isVisible() const override { return obj().isVisible(); }
			virtual void setVisible( const bool p_visible ) override
			{
				if constexpr ( not std::is_const<T>::value )
					obj().setVisible( p_visible );
			}

			virtual void remove() override
			{
				if constexpr ( not std::is_const<T>::value )
					obj().remove();
			}
		};

		std::shared_ptr<_interface> _ptr
			= nullptr; // We want the atom to be copyable at it will point toward the same entity eventually
	  public:
		template<class T, typename D>
			requires( not std::same_as<std::remove_cvref_t<T>, Atom> ) 
		Atom( std::unique_ptr<T, D> & p_ ) : _ptr( new _wrapper<T>( *p_ ) )
		{
		}
		template<class T, typename D>
			requires( not std::same_as<std::remove_cvref_t<T>, Atom> ) 
		Atom( const std::unique_ptr<T, D> & p_ ) : _ptr( new _wrapper<T>( *p_ ) )
		{
		}
		template<class T>
			requires( not std::same_as<std::remove_cvref_t<T>, Atom> ) 
		Atom( T & p_ ) : _ptr( new _wrapper<T>( p_ ) )
		{
			// static_assert(
			//	requires( const T t ) {
			//		{ t.getIndex() } -> std::same_as<atom_index_t>;
			//	}, "Missing |atom_index_t getIndex() const| class method."
			//);
			// static_assert(
			//	requires( T t, const atom_index_t idx ) {
			//		{ t.setIndex( idx ) };
			//	}, "Missing |void setIndex( const atom_index_t p_index )| class method."
			//);
			//  TODO : The rest of the static assertion to help maintainance, when we're sure this design works
		}
	};
} // namespace VTX::PythonBinding::API

#endif
