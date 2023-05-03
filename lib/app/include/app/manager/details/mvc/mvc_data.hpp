#ifndef __VTX_APP_MANAGER_DETAILS_MVC_MVC_DATA__
#define __VTX_APP_MANAGER_DETAILS_MVC_MVC_DATA__

#include "app/core/model/base_model.hpp"
#include "app/core/view/base_view.hpp"
#include "app/id.hpp"
#include <map>

namespace VTX::App::Manager::Details::MVC
{
	class MvcData
	{
	  public:
		using MapViews	= std::map<VTX_ID, App::Core::View::BaseView<App::Core::Model::BaseModel> *>;
		using PairViews = std::pair<const App::VTX_ID, App::Core::View::BaseView<App::Core::Model::BaseModel> *>;

		MvcData( App::Core::Model::BaseModel * const p_model ) : _model( p_model ) {}
		~MvcData() = default;

		const App::Core::Model::ID &		getId() const { return _model->getId(); }
		const App::Core::Model::BaseModel & getModel() const { return *_model; };
		App::Core::Model::BaseModel &		getModel() { return *_model; };

		template<typename M,
				 typename V,
				 typename = std::enable_if<std::is_base_of<M, App::Core::Model::BaseModel>::value>,
				 typename = std::enable_if<std::is_base_of<V, App::Core::View::BaseView<M>>::value>>
		inline void addView( const App::VTX_ID & p_id, V * const p_view )
		{
			_views.emplace( p_id, (App::Core::View::BaseView<App::Core::Model::BaseModel> * const)p_view );
		}

		template<typename M,
				 typename V,
				 typename = std::enable_if<std::is_base_of<M, App::Core::Model::BaseModel>::value>,
				 typename = std::enable_if<std::is_base_of<V, App::Core::View::BaseView<M>>::value>>
		inline V * const removeView( const App::VTX_ID & p_id )
		{
			V * const view = (V * const)_views[ p_id ];
			_views.erase( p_id );
			return view;
		}

		inline App::Core::View::BaseView<App::Core::Model::BaseModel> * removeView( const App::VTX_ID & p_id )
		{
			App::Core::View::BaseView<App::Core::Model::BaseModel> * const view = _views[ p_id ];
			_views.erase( p_id );
			return view;
		}

		template<typename M,
				 typename V,
				 typename = std::enable_if<std::is_base_of<M, App::Core::Model::BaseModel>::value>,
				 typename = std::enable_if<std::is_base_of<V, App::Core::View::BaseView<M>>::value>>
		inline V * const getView( const App::VTX_ID & p_id )
		{
			return (V * const)_views[ p_id ];
		}

		inline const bool		hasView( const App::VTX_ID & p_id ) const { return _views.find( p_id ) != _views.end(); }
		inline MapViews &		getViews() { return _views; }
		inline const MapViews & getViews() const { return _views; }

		void notifyViews( const VTX::App::Core::Event::VTXEvent * const p_event ) const
		{
			for ( const PairViews & pair : _views )
			{
				pair.second->notify( p_event );
			}
		};

	  private:
		App::Core::Model::BaseModel * const _model;
		MapViews							_views = MapViews();
	};

} // namespace VTX::App::Manager::Details::MVC

#endif
