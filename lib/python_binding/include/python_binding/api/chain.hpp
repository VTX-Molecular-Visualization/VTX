#ifndef __VTX_PYTHON_API_CHAIN__
#define __VTX_PYTHON_API_CHAIN__

#include <util/types.hpp>

namespace VTX::PythonBinding::API
{
	/**
	 * @brief Class responsible for defining an facade for Python for an atom.
	 */
	class Chain
	{
	  public:
		Chain() = default;

		inline const size_t getIndex() const
		{
			if ( _ptr )
				return _ptr->getIndex();
			return 0xffffffffffffffff;
		}
		inline void setIndex( const size_t p_index )
		{
			if ( _ptr )
				_ptr->setIndex( p_index );
		}

		inline const std::string & getName() const
		{
			if ( _ptr )
				return _ptr->getName();
			static const std::string _;
			return _;
		}
		inline void setName( const std::string & p_name )
		{
			if ( _ptr )
				_ptr->setName( p_name );
		}

		inline size_t getIndexFirstResidue() const
		{
			if ( _ptr )
				return _ptr->getIndexFirstResidue();
			return 0;
		}
		inline size_t getIndexLastResidue() const
		{
			if ( _ptr )
				return _ptr->getIndexLastResidue();
			return 0;
		}
		inline void setIndexFirstResidue( const size_t p_residueIndex )
		{
			if ( _ptr )
				_ptr->setIndexFirstResidue( p_residueIndex );
		}

		inline size_t getResidueCount() const
		{
			if ( _ptr )
				return _ptr->getResidueCount();
			return 0;
		}
		inline void setResidueCount( const size_t p_residueCount )
		{
			if ( _ptr )
				_ptr->setResidueCount( p_residueCount );
		}

		inline atom_index_t getIndexFirstAtom() const
		{
			if ( _ptr )
				return _ptr->getIndexFirstAtom();
			return 0xffffffff;
		}
		inline atom_index_t getIndexLastAtom() const
		{
			if ( _ptr )
				return _ptr->getIndexLastAtom();
			return 0xffffffff;
		}

		inline const std::string & getOriginalChainID() const
		{
			if ( _ptr )
				return _ptr->getOriginalChainID();
			static const std::string _;
			return _;
		}
		inline void setOriginalChainID( const std::string & p_chainId )
		{
			if ( _ptr )
				_ptr->setOriginalChainID( p_chainId );
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

			virtual const size_t getIndex() const				  = 0;
			virtual void		 setIndex( const size_t p_index ) = 0;

			virtual const std::string & getName() const						  = 0;
			virtual void				setName( const std::string & p_name ) = 0;

			virtual size_t getIndexFirstResidue() const						   = 0;
			virtual size_t getIndexLastResidue() const						   = 0;
			virtual void   setIndexFirstResidue( const size_t p_residueIndex ) = 0;

			virtual size_t getResidueCount() const						  = 0;
			virtual void   setResidueCount( const size_t p_residueCount ) = 0;

			virtual atom_index_t getIndexFirstAtom() const = 0;
			virtual atom_index_t getIndexLastAtom() const  = 0;

			virtual const std::string & getOriginalChainID() const							= 0;
			virtual void				setOriginalChainID( const std::string & p_chainId ) = 0;

			virtual bool isVisible() const		= 0;
			virtual bool isFullyVisible() const = 0;

			virtual void setVisible( const bool p_visible ) = 0;

			virtual void remove() = 0;
		};

		template<class T>
		class _wrapper final : public _interface
		{
			T & _obj;

		  public:
			_wrapper( T & p_ ) : _obj( p_ ) {}

			const size_t getIndex() const override { return _obj.getIndex(); }
			void		 setIndex( const size_t p_index )
			{
				if constexpr ( not std::is_const<T>::value )
					_obj.setIndex( p_index );
			}

			const std::string & getName() const override { return _obj.getName(); }
			void				setName( const std::string & p_name )
			{
				if constexpr ( not std::is_const<T>::value )
					_obj.setName( p_name );
			}

			size_t getIndexFirstResidue() const override { return _obj.getIndexFirstResidue(); }
			size_t getIndexLastResidue() const override { return _obj.getIndexLastResidue(); }
			void   setIndexFirstResidue( const size_t p_residueIndex )
			{
				if constexpr ( not std::is_const<T>::value )
					_obj.setIndexFirstResidue( p_residueIndex );
			}

			size_t getResidueCount() const override { return _obj.getResidueCount(); }
			void   setResidueCount( const size_t p_residueCount )
			{
				if constexpr ( not std::is_const<T>::value )
					_obj.setResidueCount( p_residueCount );
			}

			atom_index_t getIndexFirstAtom() const override { return _obj.getIndexFirstAtom(); }
			atom_index_t getIndexLastAtom() const override { return _obj.getIndexLastAtom(); }

			const std::string & getOriginalChainID() const override { return _obj.getOriginalChainID(); }
			void				setOriginalChainID( const std::string & p_chainId )
			{
				if constexpr ( not std::is_const<T>::value )
					_obj.setOriginalChainID( p_chainId );
			}

			bool isVisible() const override { return _obj.isVisible(); }
			bool isFullyVisible() const override { return _obj.isFullyVisible(); }

			void setVisible( const bool p_visible )
			{
				if constexpr ( not std::is_const<T>::value )
					_obj.setVisible( p_visible );
			}

			void remove() override
			{
				if constexpr ( not std::is_const<T>::value )
					_obj.remove();
			}

			/*
			// TODO
			System * const		 getSystemPtr() const { return _systemPtr; };
			const System * const getConstSystemPtr() const { return _systemPtr; };
			void				 setSystemPtr( System * const p_systemPtr ) { _systemPtr = p_systemPtr; };
			// const Util::Color::Rgba & getColor() const { return _defaultColor; };
			// void setColor( const Util::Color::Rgba & p_defaultColor ) { _defaultColor = p_defaultColor; };

			ResidueIndexRange getResidueRange() const;
			AtomIndexRange	  getAtomRange() const;

			Iterator::ResidueContainer residues() const;
			Iterator::AtomContainer	   atoms() const;

			*/
		};

		std::shared_ptr<_interface> _ptr = nullptr;

	  public:
		template<class T>
			requires( not std::same_as<std::remove_cv<T>, Chain> )
		Chain( T & p_ ) : _ptr( new _wrapper<T>( p_ ) )
		{
		}
	};
} // namespace VTX::PythonBinding::API

#endif
