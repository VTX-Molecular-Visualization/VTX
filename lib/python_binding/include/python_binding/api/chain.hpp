#ifndef __VTX_PYTHON_API_CHAIN__
#define __VTX_PYTHON_API_CHAIN__

#include <concepts>
#include <memory>
#include <util/constants.hpp>

namespace VTX::PythonBinding::API
{
	/**
	 * @brief Class responsible for defining an facade for Python for an atom.
	 */
	class Chain
	{
	  public:
		Chain() = default;

		inline const Index getIndex() const
		{
			if ( _ptr )
				return _ptr->getIndex();
			return INVALID_INDEX;
		}
		inline void setIndex( const Index p_index )
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

		inline Index getIndexFirstResidue() const
		{
			if ( _ptr )
				return _ptr->getIndexFirstResidue();
			return 0;
		}
		inline Index getIndexLastResidue() const
		{
			if ( _ptr )
				return _ptr->getIndexLastResidue();
			return 0;
		}
		inline void setIndexFirstResidue( const Index p_residueIndex )
		{
			if ( _ptr )
				_ptr->setIndexFirstResidue( p_residueIndex );
		}

		inline Index getResidueCount() const
		{
			if ( _ptr )
				return _ptr->getResidueCount();
			return 0;
		}
		inline void setResidueCount( const Index p_residueCount )
		{
			if ( _ptr )
				_ptr->setResidueCount( p_residueCount );
		}

		inline Index getIndexFirstAtom() const
		{
			if ( _ptr )
				return _ptr->getIndexFirstAtom();
			return INVALID_INDEX;
		}
		inline Index getIndexLastAtom() const
		{
			if ( _ptr )
				return _ptr->getIndexLastAtom();
			return INVALID_INDEX;
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

			virtual const Index getIndex() const				= 0;
			virtual void		setIndex( const Index p_index ) = 0;

			virtual const std::string & getName() const						  = 0;
			virtual void				setName( const std::string & p_name ) = 0;

			virtual Index getIndexFirstResidue() const						 = 0;
			virtual Index getIndexLastResidue() const						 = 0;
			virtual void  setIndexFirstResidue( const Index p_residueIndex ) = 0;

			virtual Index getResidueCount() const						= 0;
			virtual void  setResidueCount( const Index p_residueCount ) = 0;

			virtual Index getIndexFirstAtom() const = 0;
			virtual Index getIndexLastAtom() const	= 0;

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

			const Index getIndex() const override { return obj().getIndex(); }
			void		setIndex( const Index p_index )
			{
				if constexpr ( not std::is_const<T>::value )
					obj().setIndex( p_index );
			}

			const std::string & getName() const override { return obj().getName(); }
			void				setName( const std::string & p_name )
			{
				if constexpr ( not std::is_const<T>::value )
					obj().setName( p_name );
			}

			Index getIndexFirstResidue() const override { return obj().getIndexFirstResidue(); }
			Index getIndexLastResidue() const override { return obj().getIndexLastResidue(); }
			void  setIndexFirstResidue( const Index p_residueIndex )
			{
				if constexpr ( not std::is_const<T>::value )
					obj().setIndexFirstResidue( p_residueIndex );
			}

			Index getResidueCount() const override { return obj().getResidueCount(); }
			void  setResidueCount( const Index p_residueCount )
			{
				if constexpr ( not std::is_const<T>::value )
					obj().setResidueCount( p_residueCount );
			}

			Index getIndexFirstAtom() const override { return obj().getIndexFirstAtom(); }
			Index getIndexLastAtom() const override { return obj().getIndexLastAtom(); }

			const std::string & getOriginalChainID() const override { return obj().getOriginalChainID(); }
			void				setOriginalChainID( const std::string & p_chainId )
			{
				if constexpr ( not std::is_const<T>::value )
					obj().setOriginalChainID( p_chainId );
			}

			bool isVisible() const override { return obj().isVisible(); }
			bool isFullyVisible() const override { return obj().isFullyVisible(); }

			void setVisible( const bool p_visible )
			{
				if constexpr ( not std::is_const<T>::value )
					obj().setVisible( p_visible );
			}

			void remove() override
			{
				if constexpr ( not std::is_const<T>::value )
					obj().remove();
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
			requires( not std::same_as<std::remove_cvref_t<T>, Chain> )
		Chain( std::unique_ptr<T> & p_ ) : _ptr( new _wrapper<T>( *p_ ) )
		{
		}
		template<class T>
			requires( not std::same_as<std::remove_cvref_t<T>, Chain> )
		Chain( const std::unique_ptr<T> & p_ ) : _ptr( new _wrapper<T>( *p_ ) )
		{
		}
		template<class T>
			requires( not std::same_as<std::remove_cvref_t<T>, Chain> )
		Chain( T & p_ ) : _ptr( new _wrapper<T>( p_ ) )
		{
		}
	};
} // namespace VTX::PythonBinding::API

#endif
