#ifndef __VTX_UTIL_COLLECTION__
#define __VTX_UTIL_COLLECTION__

#include "hashing.hpp"
#include "singleton.hpp"
#include <concepts>
#include <functional>
#include <map>
#include <memory>

namespace VTX::Util
{
	using CollectionKey = std::string;

	// template<typename T>
	//  concept CollectionableConcept
	//	= requires( const T & _collectionable, std::unique_ptr<T> p_clone ) { p_clone = _collectionable.clone(); };

	template<typename T>
	// template<CollectionableConcept T>
	class Collection
	{
	  public:
		template<typename T2>
		class Registration
		{
		  public:
			Registration( const CollectionKey & p_key )
			{
				Singleton<Collection<T>>::get().template addItem<T2>( p_key );
			}
			Registration( const Hash & p_hash ) { Singleton<Collection<T>>::get().template addItem<T2>( p_hash ); }
		};

	  public:
		Collection() {};

		template<typename T2>
			requires std::derived_from<T2, T> && std::default_initializable<T2>
		void addItem( const Hash & p_hash )
		{
			_collection[ p_hash ] = std::make_unique<T2>();
		}
		template<typename T2>
			requires std::derived_from<T2, T> && std::default_initializable<T2>
		void addItem( const CollectionKey & p_key )
		{
			addItem<T2>( Util::hash( p_key ) );
		}

		std::unique_ptr<T> instantiateItem( const Hash & p_hash )
		{
			if ( !_collection.contains( p_hash ) )
				return nullptr;

			std::unique_ptr<T> itemPtr = _collection[ p_hash ]->clone();
			return std::move( itemPtr );
		}
		std::unique_ptr<T> instantiateItem( const CollectionKey & p_key )
		{
			return instantiateItem( Util::hash( p_key ) );
		}

		template<typename T2>
			requires std::derived_from<T2, T> && std::copy_constructible<T2>
		std::unique_ptr<T2> instantiateItem( const Hash & p_hash )
		{
			const std::unique_ptr<T> source = instantiateItem( p_hash );

			if ( source == nullptr )
				return nullptr;

			const T2 * const castedSourcePtr = dynamic_cast<const T2 *>( source.get() );
			return castedSourcePtr == nullptr ? nullptr : std::make_unique<T2>( *castedSourcePtr );
		}
		template<typename T2>
			requires std::derived_from<T2, T> && std::copy_constructible<T2>
		std::unique_ptr<T2> instantiateItem( const CollectionKey & p_name )
		{
			return instantiateItem<T2>( Util::hash( p_name ) );
		}

	  private:
		std::map<Hash, std::unique_ptr<T>> _collection;
	};

} // namespace VTX::Util
#endif
