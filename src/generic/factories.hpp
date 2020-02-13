#ifndef __VTX_FACTORIES__
#define __VTX_FACTORIES__

#ifdef _MSC_VER
#pragma once
#endif

#include "base_cleanable.hpp"
#include "base_initializable.hpp"
#include <type_traits>
#include <utility>

namespace VTX
{
	namespace Generic
	{
		template<typename T, typename = std::enable_if<std::is_base_of<Generic::BaseInitializable, T>::value>>
		class FactoryInitializable
		{
		  public:
			static T * const create()
			{
				T * const instance = new T();
				instance->init();
				return instance;
			}
		};

		template<typename T, typename = std::enable_if<std::is_base_of<Generic::BaseCleanable, T>::value>>
		class FactoryCleanable
		{
		  public:
			static void destroy( T * p_instance )
			{
				p_instance->clean();
				delete p_instance;
			}
		};

		template<typename T, typename = std::enable_if<std::is_base_of<Generic::BaseCollectionable, T>::value>>
		class FactoryCollectionable
		{
		  public:
			static T * const create() { return FactoryInitializable<T>::create(); }
			static void		 destroy( T * p_instance ) { return FactoryCleanable<T>::destroy( p_instance ); }
		};

		template<typename T, typename = std::enable_if<std::is_base_of<State::BaseState, T>::value>>
		class FactoryState
		{
		  public:
			static T * const create() { return FactoryCollectionable<T>::create(); }
			static void		 destroy( T * p_instance ) { return FactoryCollectionable<T>::destroy( p_instance ); }
		};

		template<typename T, typename = std::enable_if<std::is_base_of<Controller::BaseController, T>::value>>
		class FactoryController
		{
		  public:
			static T * const create() { return FactoryCollectionable<T>::create(); }
			static void		 destroy( T * p_instance ) { return FactoryCollectionable<T>::destroy( p_instance ); }
		};

		template<typename T, typename = std::enable_if<std::is_base_of<UI::BaseComponent, T>::value>>
		class FactoryComponent
		{
		  public:
			static T * const create() { return FactoryCollectionable<T>::create(); }
			static void		 destroy( T * p_instance ) { return FactoryCollectionable<T>::destroy( p_instance ); }
		};

		template<typename M,
				 typename V,
				 typename = std::enable_if<std::is_base_of<Model::BaseModel, M>::value>,
				 typename = std::enable_if<std::is_base_of<View::BaseView<M>, V>::value>>
		class FactoryView
		{
		  public:
			static V * const create( M * p_model )
			{
				V * const instance = new V( p_model );
				instance->init();
				return instance;
			}
			static void destroy( V * p_instance ) { return FactoryCleanable<V>::destroy( p_instance ); }
		};
	} // namespace Generic
} // namespace VTX
#endif
