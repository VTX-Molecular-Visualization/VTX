#ifndef __VTX_APP_CORE_COLLECTION__
#define __VTX_APP_CORE_COLLECTION__

#include <concepts>
#include <functional>
#include <map>
#include <memory>
#include <util/generic/base_static_singleton.hpp>
#include <util/hashing.hpp>

namespace VTX::App::Core
{
	using CollectionKey = std::string;

	template<typename T>
	concept CollectionableConcept = requires() { true; };

	template<CollectionableConcept T>
	class Collection : public Util::Generic::BaseStaticSingleton<Collection<T>>
	{
	  public:
		template<typename T2>
		class Registration
		{
		  public:
			Registration( const CollectionKey & p_key ) { Collection<T>::get().template addItem<T2>( p_key ); }
			Registration( const Util::Hashing::Hash & p_hash ) { Collection<T>::get().template addItem<T2>( p_hash ); }
		};

	  public:
		Collection( typename Util::Generic::BaseStaticSingleton<Collection<T>>::StructPrivacyToken ) {};

		template<typename T2>
			requires std::derived_from<T2, T> && std::default_initializable<T2>
		void addItem( const Util::Hashing::Hash & p_hash )
		{
			_collection[ p_hash ] = []() { return std::make_unique<T2>(); };
		}
		template<typename T2>
			requires std::derived_from<T2, T> && std::default_initializable<T2>
		void addItem( const CollectionKey & p_key )
		{
			addItem<T2>( Util::Hashing::hash( p_key ) );
		}

		std::unique_ptr<T> instantiateItem( const Util::Hashing::Hash & p_hash )
		{
			if ( !_collection.contains( p_hash ) )
				return nullptr;

			return _collection[ p_hash ]();
		}
		std::unique_ptr<T> instantiateItem( const CollectionKey & p_key )
		{
			return instantiateItem( Util::Hashing::hash( p_key ) );
		}

		template<typename T2>
			requires std::derived_from<T2, T> && std::copy_constructible<T2>
		std::unique_ptr<T2> instantiateItem( const Util::Hashing::Hash & p_hash )
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
			return instantiateItem<T2>( Util::Hashing::hash( p_name ) );
		}

	  private:
		std::map<Util::Hashing::Hash, std::function<std::unique_ptr<T>()>> _collection;
	};

} // namespace VTX::App::Core
#endif
