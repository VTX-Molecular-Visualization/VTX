#ifndef __VTX_APP_CORE_COLLECTION__
#define __VTX_APP_CORE_COLLECTION__

#include <algorithm>
#include <concepts>
#include <memory>
#include <string>
#include <util/generic/base_static_singleton.hpp>
#include <vector>

namespace VTX::App::Core
{
	template<typename T>
	concept DisplayableName = requires( T p_t, std::string p_name ) { p_name = p_t.getName(); };

	template<typename T>
	concept Collectionable = DisplayableName<T>;

	template<Collectionable T>
	class Collection : public Util::Generic::BaseStaticSingleton<Collection<T>>
	{
	  public:
		Collection( typename Util::Generic::BaseStaticSingleton<Collection<T>>::StructPrivacyToken ) {};

		template<typename T2>
		void addItem()
		{
			addItem( std::make_unique<T2>() );
		}
		void addItem( std::unique_ptr<T> p_item ) { _collection.emplace_back( std::move( p_item ) ); }

		std::unique_ptr<T> instantiateItem( const size_t p_index )
		{
			return std::make_unique( _collection[ p_index ] );
		}
		template<typename T2>
		std::unique_ptr<T> instantiateItem( const size_t p_index )
		{
			const T & source = *( _collection[ p_index ] );
			return std::make_unique<T2>( dynamic_cast<const T2 &>( source ) );
		}

		std::unique_ptr<T> instantiateItem( const std::string & p_name )
		{
			typename std::vector<std::unique_ptr<T>>::const_iterator it = std::find_if(
				_collection.begin(),
				_collection.end(),
				[ p_name ]( const std::unique_ptr<T> & p_item ) { return p_item->getName() == p_name; } );

			if ( it == _collection.end() )
				return nullptr;
			else
				return std::make_unique( *it );
		}

		template<typename T2>
		std::unique_ptr<T> instantiateItem( const std::string & p_name )
		{
			typename std::vector<std::unique_ptr<T>>::const_iterator it = std::find_if(
				_collection.begin(),
				_collection.end(),
				[ p_name ]( const std::unique_ptr<T> & p_item ) { return p_item->getName() == p_name; } );

			if ( it == _collection.end() )
			{
				return nullptr;
			}
			else
			{
				const T & source = *( *it );
				return std::make_unique<T2>( dynamic_cast<const T2 &>( source ) );
			}
		}

	  private:
		std::vector<std::unique_ptr<T>> _collection;
	};

	template<typename T>
	class CollectionItem
	{
	  protected:
		template<typename T2>
		class Registration
		{
		  public:
			Registration() { App::Core::Collection<T>::get().template addItem<T2>(); }
		};
	};

} // namespace VTX::App::Core
#endif
