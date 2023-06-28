#ifndef __VTX_APP_OLD_MANAGER_MVC_MANAGER__
#define __VTX_APP_OLD_MANAGER_MVC_MANAGER__

#include "app/old/application/generic/base_lockable.hpp"
#include "app/old/core/model/base_model.hpp"
#include "app/old/core/view/base_view.hpp"
#include "app/old/id.hpp"
#include "details/mvc/mvc_data.hpp"
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace VTX::App::Old::Manager
{
	using namespace ::VTX::App::Old::Manager::Details::MVC;

	class MvcManager : public Application::Generic::BaseLockable
	{
	  public:
		inline static MvcManager & get()
		{
			static MvcManager instance;
			return instance;
		}

	  public:
		template<typename M, typename = std::enable_if<std::is_base_of<App::Old::Core::Model::BaseModel, M>::value>>
		M * const instantiateModel()
		{
			M * const		model = new M();
			MvcData * const mvc	  = new MvcData( model );

			_lock();
			_container.emplace( model->getId(), mvc );
			model->_instantiateDefaultViews();
			_unlock();

			return model;
		}

		template<typename M,
				 typename P1,
				 typename = std::enable_if<std::is_base_of<App::Old::Core::Model::BaseModel, M>::value>>
		M * const instantiateModel( P1 & p_param1 )
		{
			M * const		model = new M( p_param1 );
			MvcData * const mvc	  = new MvcData( model );

			_lock();
			_container.emplace( model->getId(), mvc );
			model->_instantiateDefaultViews();
			_unlock();

			return model;
		}

		template<typename M,
				 typename P1,
				 typename = std::enable_if<std::is_base_of<App::Old::Core::Model::BaseModel, M>::value>>
		M * const instantiateModel( P1 * p_param1 )
		{
			M * const		model = new M( p_param1 );
			MvcData * const mvc	  = new MvcData( model );

			_lock();
			_container.emplace( model->getId(), mvc );
			model->_instantiateDefaultViews();
			_unlock();

			return model;
		}

		template<typename M, typename = std::enable_if<std::is_base_of<App::Old::Core::Model::BaseModel, M>::value>>
		void deleteModel( M * const & p_model )
		{
			_lock();
			const MvcData * const mvc = _container[ p_model->getId() ];
			_container.erase( p_model->getId() );
			_unlock();

			for ( const std::pair<const VTX::App::Old::VTX_ID, App::Old::Core::View::BaseView<App::Old::Core::Model::BaseModel> *> &
					  pair : mvc->getViews() )
			{
				delete pair.second;
			}

			delete mvc;
			delete p_model;
		}

		template<typename M, typename = std::enable_if<std::is_base_of<App::Old::Core::Model::BaseModel, M>::value>>
		void deleteAllModels( std::vector<M *> & p_models )
		{
			for ( auto it = p_models.begin(); it != p_models.end(); it++ )
			{
				if ( *it != nullptr )
					deleteModel( *it );
			}
		}

		template<typename M, typename = std::enable_if<std::is_base_of<App::Old::Core::Model::BaseModel, M>::value>>
		const M & getModel( const App::Old::Core::Model::ID & p_id ) const
		{
#ifdef _MSC_VER
			const App::Old::Core::Model::BaseModel & model = _container[ p_id ]->getModel();
#else
			const App::Old::Core::Model::BaseModel & model = _container.at( p_id )->getModel();
#endif
			const M & modelPtr = static_cast<const M &>( model );
			return modelPtr;
		}

		template<typename M, typename = std::enable_if<std::is_base_of<App::Old::Core::Model::BaseModel, M>::value>>
		M & getModel( const App::Old::Core::Model::ID & p_id )
		{
#ifdef _MSC_VER
			App::Old::Core::Model::BaseModel & model = _container[ p_id ]->getModel();
#else
			App::Old::Core::Model::BaseModel &		model = _container.at( p_id )->getModel();
#endif
			M & modelPtr = static_cast<M &>( model );
			return modelPtr;
		}

		bool doesModelExists( const App::Old::Core::Model::ID & p_id )
		{
			return _container.find( p_id ) != _container.end();
		}

		const App::Old::VTX_ID & getModelTypeID( const App::Old::Core::Model::ID & p_id )
		{
			return _container[ p_id ]->getModel().getTypeId();
		};

		template<typename V,
				 typename M,
				 typename = std::enable_if<std::is_base_of<App::Old::Core::Model::BaseModel, M>::value>,
				 typename = std::enable_if<std::is_base_of<App::Old::Core::View::BaseView<M>, V>::value>>
		inline V * const instantiateView( M * const p_model, const App::Old::VTX_ID & p_id )
		{
			_lock();
			V * const view = new V( p_model );
			static_cast<MvcData *>( _container[ p_model->getId() ] )->addView<M, V>( p_id, view );
			_unlock();

			return view;
		}

		// TODO Move this out of this class
		template<typename V,
				 typename M,
				 typename W,
				 typename = std::enable_if<std::is_base_of<App::Old::Core::Model::BaseModel, M>::value>,
				 typename = std::enable_if<std::is_base_of<App::Old::Core::View::BaseView<M>, V>::value>>
		inline V * const instantiateViewWidget( M * const			p_model,
												const App::Old::VTX_ID & p_id,
												W * const			p_parentWidget = nullptr )
		{
			_lock();
			V * const view = new V( p_model, p_parentWidget );
			static_cast<MvcData *>( _container[ p_model->getId() ] )->addView<M, V>( p_id, view );
			_unlock();

			return view;
		}

		template<typename V,
				 typename M,
				 typename = std::enable_if<std::is_base_of<App::Old::Core::Model::BaseModel, M>::value>,
				 typename = std::enable_if<std::is_base_of<App::Old::Core::View::BaseView<M>, V>::value>>
		inline V * const getView( const M * const p_model, const App::Old::VTX_ID & p_id )
		{
			return static_cast<MvcData *>( _container[ p_model->getId() ] )->getView<M, V>( p_id );
		}

		template<typename V,
				 typename M,
				 typename = std::enable_if<std::is_base_of<App::Old::Core::Model::BaseModel, M>::value>,
				 typename = std::enable_if<std::is_base_of<App::Old::Core::View::BaseView<M>, V>::value>>
		inline void deleteView( const M * const p_model, const App::Old::VTX_ID & p_id )
		{
			_lock();
			delete static_cast<MvcData *>( _container[ p_model->getId() ] )->removeView<M, V>( p_id );
			_unlock();
		}

		template<typename V,
				 typename M,
				 typename = std::enable_if<std::is_base_of<App::Old::Core::Model::BaseModel, M>::value>,
				 typename = std::enable_if<std::is_base_of<App::Old::Core::View::BaseView<M>, V>::value>>
		inline void deleteView( const V * const p_view, const App::Old::VTX_ID & p_id )
		{
			_lock();
			delete static_cast<MvcData *>( _container[ p_view->_model->getId() ] )->removeView( p_id );
			_unlock();
		}

		inline void deleteView( const App::Old::Core::Model::BaseModel * const p_model, const App::Old::VTX_ID & p_id )
		{
			_lock();
			delete _container[ p_model->getId() ]->removeView( p_id );
			_unlock();
		}

		inline const bool hasView( const App::Old::Core::Model::BaseModel * const p_model, const App::Old::VTX_ID & p_id )
		{
			return _container.find( p_model->getId() ) != _container.end()
				   && _container[ p_model->getId() ]->hasView( p_id );
		};

		inline void notifyViews( const App::Old::Core::Model::BaseModel * const	   p_caller,
								 const VTX::App::Old::Core::Event::VTXEvent * const p_event )
		{
			_lock();
			_container[ p_caller->getId() ]->notifyViews( p_event );
			_unlock();
		}

		inline App::Old::VTX_ID generateViewID( const App::Old::VTX_ID & p_baseID, const std::string & p_suffix )
		{
			return p_baseID + '_' + p_suffix;
		}

	  private:
		MvcManager()								 = default;
		MvcManager( const MvcManager & )			 = delete;
		MvcManager & operator=( const MvcManager & ) = delete;
		~MvcManager() { assert( _container.size() == 0 ); }

		std::unordered_map<VTX::App::Old::Core::Model::ID, MvcData *> _container
			= std::unordered_map<VTX::App::Old::Core::Model::ID, MvcData *>();
	};
} // namespace VTX::App::Old::Manager

#endif
