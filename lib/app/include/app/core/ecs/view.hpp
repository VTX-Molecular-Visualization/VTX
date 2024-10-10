#ifndef __VTX_APP_CORE_ECS_VIEW__
#define __VTX_APP_CORE_ECS_VIEW__

#include "app/core/ecs/base_entity.hpp"
#include "app/core/ecs/concepts.hpp"
#include <entt/entity/mixin.hpp>
#include <entt/entity/registry.hpp>
#include <entt/entity/storage.hpp>
#include <entt/entity/view.hpp>
#include <limits>
#include <memory>

namespace VTX::App::Core::ECS
{
	template<typename It>
	struct ViewIterator;

	template<ConceptComponent Type, ConceptComponent... Other>
	class View
	{
	  private:
		template<ConceptComponent T>
		using single_storage = const entt::basic_sigh_mixin<entt::basic_storage<T>, entt::registry>;

		template<ConceptComponent... Types>
		using internal_view = entt::basic_view<entt::get_t<single_storage<Types>...>, entt::exclude_t<>>;

		using view_type = internal_view<Type, Other...>;

		using entt_iterator_t = typename view_type::iterator;
		using iterator_t	  = ViewIterator<entt_iterator_t>;

	  public:
		View( const entt::registry & p_registry ) : _view( p_registry.view<Type, Other...>() ) {}

		const view_type & getView() const { return _view; };

		size_t size() const { return _view.handle()->size(); }
		size_t size_hint() const { return _view.size_hint(); }

		template<ConceptComponent T>
		const T & getComponent( const BaseEntity & p_entity ) const
		{
			// Template disambiguator necessary in this case because calling a template function
			return _view.template get<T>( p_entity );
		}

		iterator_t begin() const { return iterator_t( _view.begin() ); }
		iterator_t end() const { return iterator_t( _view.end() ); }

		BaseEntity front() const { return BaseEntity( _view.front() ); }
		BaseEntity back() const { return BaseEntity( _view.back() ); }

		iterator_t find( const BaseEntity & p_entity ) const { return iterator_t( _view.find( p_entity ) ); }

	  private:
		view_type _view;
	};

	template<typename It>
	struct ViewIterator
	{
	  public:
		ViewIterator( It p_it ) : _iterator( p_it ) {};

		BaseEntity		   operator*() const { return BaseEntity( *_iterator ); }
		const BaseEntity * operator->() { return static_cast<BaseEntity *>( _iterator ); }

		// Prefix increment
		ViewIterator & operator++()
		{
			_iterator++;
			return *this;
		}

		// Postfix increment
		ViewIterator operator++( int )
		{
			ViewIterator tmp = *this;
			++( *this );
			return tmp;
		}

		friend bool operator==( const ViewIterator & a, const ViewIterator & b ) { return a._iterator == b._iterator; };
		friend bool operator!=( const ViewIterator & a, const ViewIterator & b ) { return a._iterator != b._iterator; };

	  private:
		It _iterator;
	};

} // namespace VTX::App::Core::ECS

#endif
