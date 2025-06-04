#ifndef __VTX_PYTHON_API_RESIDUE__
#define __VTX_PYTHON_API_RESIDUE__

#include <concepts>
#include <util/types.hpp>

namespace VTX::PythonBinding::API
{
	class System;
	class Chain;
	class Atom;
	/**
	 * @brief Class responsible for defining an facade for Python for a residue.
	 */
	class Residue
	{
	  public:
		Residue() = default;

		inline size_t getIndex() const
		{
			if ( _ptr )
				return _ptr->getIndex();
			return 0;
		}
		inline void setIndex( const size_t p_index )
		{
			if ( _ptr )
				_ptr->setIndex( p_index );
		}

		inline atom_index_t getIndexFirstAtom() const
		{
			if ( _ptr )
				return _ptr->getIndexFirstAtom();
			return 0;
		}
		inline void setIndexFirstAtom( const atom_index_t p_indexFirstAtom )
		{
			if ( _ptr )
				_ptr->setIndexFirstAtom( p_indexFirstAtom );
		}
		inline atom_index_t getIndexLastAtom() const
		{
			if ( _ptr )
				return _ptr->getIndexLastAtom();
			return 0;
		}

		inline uint64_t getAtomCount() const
		{
			if ( _ptr )
				return _ptr->getAtomCount();
			return 0;
		}
		inline void setAtomCount( const atom_index_t p_atomCount )
		{
			if ( _ptr )
				_ptr->setAtomCount( p_atomCount );
		}

		inline size_t getIndexFirstBond() const
		{
			if ( _ptr )
				return _ptr->getIndexFirstBond();
			return 0;
		}
		inline void setIndexFirstBond( const size_t p_indexFirstBond )
		{
			if ( _ptr )
				_ptr->setIndexFirstBond( p_indexFirstBond );
		}

		inline size_t getBondCount() const
		{
			if ( _ptr )
				return _ptr->getBondCount();
			return 0;
		}
		inline void setBondCount( const size_t p_bondCount )
		{
			if ( _ptr )
				_ptr->setBondCount( p_bondCount );
		}

		inline size_t getIndexInOriginalChain() const
		{
			if ( _ptr )
				return _ptr->getIndexInOriginalChain();
			return 0;
		}

		inline const std::string_view getShortName() const
		{
			if ( _ptr )
				return _ptr->getShortName();
			return {};
		}
		inline const std::string_view getName() const
		{
			if ( _ptr )
				return _ptr->getName();
			return {};
		}
		inline const std::string_view getLongName() const
		{
			if ( _ptr )
				return _ptr->getLongName();
			return {};
		}

		inline void setIndexInOriginalChain( const size_t p_index )
		{
			if ( _ptr )
				_ptr->setIndexInOriginalChain( p_index );
		}

		inline void setVisible( const bool p_visible )
		{
			if ( _ptr )
				_ptr->setVisible( p_visible );
		}
		inline bool isVisible() const
		{
			if ( _ptr )
				return _ptr->isVisible();
			return 0;
		}
		inline bool isFullyVisible() const
		{
			if ( _ptr )
				return _ptr->isFullyVisible();
			return 0;
		}

		const Chain	 getChain() const;
		Chain		 getChain();
		const System getSystem() const;
		System		 getSystem();

	  private:
		struct _interface
		{
			virtual ~_interface() = default;

			virtual size_t getIndex() const					= 0;
			virtual void   setIndex( const size_t p_index ) = 0;

			virtual atom_index_t getIndexFirstAtom() const								  = 0;
			virtual void		 setIndexFirstAtom( const atom_index_t p_indexFirstAtom ) = 0;
			virtual atom_index_t getIndexLastAtom() const								  = 0;

			virtual uint64_t getAtomCount() const							= 0;
			virtual void	 setAtomCount( const atom_index_t p_atomCount ) = 0;

			virtual size_t getIndexFirstBond() const						  = 0;
			virtual void   setIndexFirstBond( const size_t p_indexFirstBond ) = 0;

			virtual size_t getBondCount() const						= 0;
			virtual void   setBondCount( const size_t p_bondCount ) = 0;

			virtual size_t getIndexInOriginalChain() const = 0;

			virtual const std::string_view getShortName() const = 0;
			virtual const std::string_view getName() const		= 0;
			virtual const std::string_view getLongName() const	= 0;

			virtual void setIndexInOriginalChain( const size_t p_index ) = 0;

			virtual void setVisible( const bool p_visible ) = 0;
			virtual bool isVisible() const					= 0;
			virtual bool isFullyVisible() const				= 0;

			virtual const Chain	 getChain() const  = 0;
			virtual Chain		 getChain()		   = 0;
			virtual const System getSystem() const = 0;
			virtual System		 getSystem()	   = 0;
		};

		template<class T, typename System = System, typename Chain = Chain>
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

			virtual size_t getIndex() const override { return obj().getIndex(); }
			virtual void   setIndex( const size_t p_index ) override
			{
				if constexpr ( not std::is_const<T>::value )
					obj().setIndex( p_index );
			}

			virtual atom_index_t getIndexFirstAtom() const override { return obj().getIndexFirstAtom(); }
			virtual void		 setIndexFirstAtom( const atom_index_t p_indexFirstAtom )
			{
				if constexpr ( not std::is_const<T>::value )
					obj().setIndexFirstAtom( p_indexFirstAtom );
			}
			virtual atom_index_t getIndexLastAtom() const override { return obj().getIndexLastAtom(); }

			virtual uint64_t getAtomCount() const override { return obj().getAtomCount(); }
			virtual void	 setAtomCount( const atom_index_t p_atomCount ) override
			{
				if constexpr ( not std::is_const<T>::value )
					obj().setAtomCount( p_atomCount );
			}

			virtual size_t getIndexFirstBond() const override { return obj().getIndexFirstBond(); }
			virtual void   setIndexFirstBond( const size_t p_indexFirstBond ) override
			{
				if constexpr ( not std::is_const<T>::value )
					obj().setIndexFirstBond( p_indexFirstBond );
			}

			virtual size_t getBondCount() const override { return obj().getBondCount(); }
			virtual void   setBondCount( const size_t p_bondCount ) override
			{
				if constexpr ( not std::is_const<T>::value )
					obj().setBondCount( p_bondCount );
			}

			virtual size_t getIndexInOriginalChain() const override { return obj().getIndexInOriginalChain(); }

			virtual const std::string_view getShortName() const override { return obj().getShortName(); }
			virtual const std::string_view getName() const override { return obj().getName(); }
			virtual const std::string_view getLongName() const override { return obj().getLongName(); }

			virtual void setIndexInOriginalChain( const size_t p_index ) override
			{
				if constexpr ( not std::is_const<T>::value )
					obj().setIndexInOriginalChain( p_index );
			}

			virtual void setVisible( const bool p_visible ) override
			{
				if constexpr ( not std::is_const<T>::value )
					obj().setVisible( p_visible );
			}
			virtual bool isVisible() const override { return obj().isVisible(); }
			virtual bool isFullyVisible() const override { return obj().isFullyVisible(); }

			virtual const Chain getChain() const override { return { *obj().getConstChainPtr() }; }
			virtual Chain		getChain() override
			{
				if constexpr ( not std::is_const<T>::value )
					return { *obj().getChainPtr() };
				else
					return {};
			}
			virtual const System getSystem() const override { return { *obj().getConstSystemPtr() }; }
			virtual System		 getSystem() override
			{
				if constexpr ( not std::is_const<T>::value )
					return { *obj().getSystemPtr() };
				else
					return {};
			}
			/*
			//  TODO
			AtomIndexRange			getAtomRange() const { return _obj.getAtomRange(); }
			Iterator::AtomContainer atoms() const { return _obj.atoms(); }
			ChemDB::Residue::SYMBOL getSymbol() const;
			void					setSymbol( const ChemDB::Residue::SYMBOL p_symbol );

			ChemDB::Residue::TYPE getType() { return _type; };
			void				  setType( const ChemDB::Residue::TYPE p_type ) { _type = p_type; };

			ChemDB::Atom::TYPE getAtomType() const;
			void			   setAtomType( const ChemDB::Atom::TYPE p_type );

			void				setChainPtr( Chain * const p_chainPtr );

			*/
		};

	  public:
		template<class T>
			requires( not std::same_as<std::remove_cvref_t<T>, Residue> )
		Residue( std::unique_ptr<T> & p_ ) : _ptr( new _wrapper<T>( *p_ ) )
		{
		}
		template<class T>
			requires( not std::same_as<std::remove_cvref_t<T>, Residue> )
		Residue( const std::unique_ptr<T> & p_ ) : _ptr( new _wrapper<T>( *p_ ) )
		{
		}
		template<class T>
			requires( not std::same_as<std::remove_cvref_t<T>, Residue> )
		Residue( T & p_ ) : _ptr( new _wrapper<T>( p_ ) )
		{
		}

	  private:
		std::shared_ptr<_interface> _ptr = nullptr;
	};
} // namespace VTX::PythonBinding::API

#endif
