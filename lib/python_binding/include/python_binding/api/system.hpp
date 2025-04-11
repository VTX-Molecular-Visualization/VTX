#ifndef __VTX_PYTHON_API_SYSTEM__

#include <util/types.hpp>

namespace VTX::PythonBinding::API
{

	class Residue;
	class Chain;
	class Atom;

	/**
	 * @brief Class responsible for defining an facade for Python for a chemical system.
	 */
	class System
	{
	  public:
		System() = default;

		inline void initChains( const size_t p_chainCount )
		{
			if ( _ptr )
				_ptr->initChains( p_chainCount );
		}
		/*
		inline Chain getChain( const size_t p_index )
		{
			if ( _ptr )
				return _ptr->getChain( p_index );
			return 0;
		}
		inline const Chain getChain( const size_t p_index ) const
		{
			if ( _ptr )
				return _ptr->getChain( p_index );
			return 0;
		}
		inline Residue getResidue( const size_t p_index )
		{
			if ( _ptr )
				return _ptr->getResidue( p_index );
			return 0;
		}
		inline const Residue getResidue( const size_t p_index ) const
		{
			if ( _ptr )
				return _ptr->getResidue( p_index );
			return 0;
		}
		inline Atom getAtom( const atom_index_t p_index )
		{
			if ( _ptr )
				return _ptr->getAtom( p_index );
			return 0;
		}
		inline const Atom getAtom( const atom_index_t p_index ) const
		{
			if ( _ptr )
				return _ptr->getAtom( p_index );
			return 0;
		}
		*/

		inline void initResidues( const size_t p_residueCount )
		{
			if ( _ptr )
				_ptr->initResidues( p_residueCount );
		}

		inline void initAtoms( const size_t p_atomCount )
		{
			if ( _ptr )
				_ptr->initAtoms( p_atomCount );
		}

		inline void initBonds( const size_t p_bondCount )
		{
			if ( _ptr )
				_ptr->initBonds( p_bondCount );
		}

		inline const std::string & getName() const
		{
			if ( _ptr )
				return _ptr->getName();
			return 0;
		}
		inline void setName( const std::string & p_name )
		{
			if ( _ptr )
				_ptr->setName( p_name );
		}

		inline const std::string & getPdbIdCode() const
		{
			if ( _ptr )
				return _ptr->getPdbIdCode();
			return 0;
		}
		inline void setPdbIdCode( const std::string & p_pdbIdCode )
		{
			if ( _ptr )
				_ptr->setPdbIdCode( p_pdbIdCode );
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
		inline void setVisible( const atom_index_t & p_atomId, bool p_visible )
		{
			if ( _ptr )
				_ptr->setVisible( p_visible );
		}

		inline void remove( const atom_index_t & p_atomIndex )
		{
			if ( _ptr )
				_ptr->remove( p_atomIndex );
		}

		inline size_t getRealChainCount() const
		{
			if ( _ptr )
				return _ptr->getRealChainCount();
			return 0;
		}
		inline size_t getRealResidueCount() const
		{
			if ( _ptr )
				return _ptr->getRealResidueCount();
			return 0;
		}
		inline size_t getRealAtomCount() const
		{
			if ( _ptr )
				return _ptr->getRealAtomCount();
			return 0;
		}

	  private:
		struct _interface
		{
			virtual ~_interface() = default;

			virtual void		initChains( const size_t p_chainCount ) = 0;
			virtual Chain		getChain( const size_t p_index )		= 0;
			virtual const Chain getChain( const size_t p_index ) const	= 0;

			virtual void		  initResidues( const size_t p_residueCount ) = 0;
			virtual Residue		  getResidue( const size_t p_index )		  = 0;
			virtual const Residue getResidue( const size_t p_index ) const	  = 0;

			virtual void	   initAtoms( const size_t p_atomCount )	   = 0;
			virtual Atom	   getAtom( const atom_index_t p_index )	   = 0;
			virtual const Atom getAtom( const atom_index_t p_index ) const = 0;

			virtual void initBonds( const size_t p_bondCount ) = 0;

			virtual const std::string & getName() const						  = 0;
			virtual void				setName( const std::string & p_name ) = 0;

			virtual const std::string & getPdbIdCode() const							= 0;
			virtual void				setPdbIdCode( const std::string & p_pdbIdCode ) = 0;

			virtual bool isVisible() const		= 0;
			virtual bool isFullyVisible() const = 0;

			virtual void setVisible( const bool p_visible )							 = 0;
			virtual void setVisible( const atom_index_t & p_atomId, bool p_visible ) = 0;

			virtual void remove( const atom_index_t & p_atomIndex ) = 0;

			virtual size_t getRealChainCount() const   = 0;
			virtual size_t getRealResidueCount() const = 0;
			virtual size_t getRealAtomCount() const	   = 0;

			/*
			// TODO : Implement it
			std::vector<std::unique_ptr<Chain>> &		getChains() ;
			const std::vector<std::unique_ptr<Chain>> & getChains() const ;
			std::vector<std::unique_ptr<Residue>> &		  getResidues() ;
			const std::vector<std::unique_ptr<Residue>> & getResidues() const ;
			std::vector<std::unique_ptr<Atom>> &	   getAtoms() ;
			const std::vector<std::unique_ptr<Atom>> & getAtoms() const ;
			std::vector<std::unique_ptr<Bond>> &	   getBonds() ;
			const std::vector<std::unique_ptr<Bond>> & getBonds() const ;

			// Is it necessary ?
			bool							hasTrajectory();
			VTX::Core::Struct::Trajectory & getTrajectory();
			const Atom * getAtomFromUID( Core::UID::uid p_uid ) const;
			void setVisible( const AtomIndexRange & p_atomRange, bool p_visible );
			void setVisible( const AtomIndexRangeList & p_atomRangeList, bool p_visible );
			void remove( const AtomIndexRange & p_atomRange );
			void remove( const AtomIndexRangeList & p_atomRangeList );
			const AtomIndexRangeList & getAtomVisibilities() const ;
			void					   setAtomVisibilities( const AtomIndexRangeList & p_visibility );
			const AtomIndexRangeList & getActiveAtoms() const ;

			const Core::UID::UIDRange & getAtomUIDs() const ;
			Bond *		 getBond( const size_t p_index ) ;
			const Bond * getBond( const size_t p_index ) const ;
			const Util::Math::Transform & getTransform() const ;
			Util::Math::Transform &		  getTransform() ;

			Atom * getAtomFromUID( Core::UID::uid p_uid );

			const Core::UID::UIDRange & getResidueUIDs() const ;
			const Residue *				getResidueFromUID( Core::UID::uid p_uid ) const;
			Residue *					getResidueFromUID( Core::UID::uid p_uid );
			*/
		};

		template<class T>
		class _wrapper final : public _interface
		{
			T & _obj;

		  public:
			_wrapper( T & p_ ) : _obj( p_ ) {}

			virtual void		initChains( const size_t p_chainCount ) override { _obj.initChains( p_chainCount ); }
			virtual Chain		getChain( const size_t p_index ) override { return _obj.getChain( p_index ); }
			virtual const Chain getChain( const size_t p_index ) override { return _obj.getChain( p_index ); }

			virtual void initResidues( const size_t p_residueCount ) override { _obj.initResidues( p_residueCount ); }
			virtual Residue		  getResidue( const size_t p_index ) override { return _obj.getResidue( p_index ); }
			virtual const Residue getResidue( const size_t p_index ) override { return _obj.getResidue( p_index ); }

			virtual void	   initAtoms( const size_t p_atomCount ) override { _obj.initAtoms( p_atomCount ); }
			virtual Atom	   getAtom( const atom_index_t p_index ) override { return _obj.getAtom( p_index ); }
			virtual const Atom getAtom( const atom_index_t p_index ) override { return _obj.getAtom( p_index ); }

			virtual void initBonds( const size_t p_bondCount ) override { _obj.initBonds( p_bondCount ); }

			virtual const std::string & getName() const override { _obj.getName(); }
			virtual void				setName( const std::string & p_name ) override { _obj.setName( p_name ); }

			virtual const std::string & getPdbIdCode() const override { _obj.getPdbIdCode(); }
			virtual void setPdbIdCode( const std::string & p_pdbIdCode ) override { _obj.setPdbIdCode( p_pdbIdCode ); }

			virtual bool isVisible() const override { _obj.isVisible(); }
			virtual bool isFullyVisible() const override { _obj.isFullyVisible(); }

			virtual void setVisible( const bool p_visible ) override { _obj.setVisible( p_visible ); }
			virtual void setVisible( const atom_index_t & p_atomId, bool p_visible ) override
			{
				_obj.setVisible( p_visible );
			}

			virtual void remove( const atom_index_t & p_atomIndex ) override { _obj.remove( p_atomIndex ); }

			virtual size_t getRealChainCount() const override { _obj.getRealChainCount(); }
			virtual size_t getRealResidueCount() const override { _obj.getRealResidueCount(); }
			virtual size_t getRealAtomCount() const override { _obj.getRealAtomCount(); }
		};

		std::shared_ptr<_interface> _ptr = nullptr;

	  public:
	  public:
		template<class T>
			requires( not std::same_as<std::remove_cv<T>, System> )
		System( T & p_ ) : _ptr( new _wrapper<T>( p_ ) )
		{
		}
	};
} // namespace VTX::PythonBinding::API

#endif
