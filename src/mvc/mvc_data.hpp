#ifndef __VTX_MVC_DATA__
#define __VTX_MVC_DATA__

#include "id.hpp"
#include "model/base_model.hpp"
#include "view/base_view_3d.hpp"

namespace VTX::MVC
{
	class MvcData
	{
	  public:
		MvcData( Model::BaseModel * const p_model ) : _model( p_model ) {}
		~MvcData() = default;

		const Model::ID &		 getId() const { return _model->getId(); }
		const Model::BaseModel & getModel() const { return *_model; };
		Model::BaseModel &		 getModel() { return *_model; };

		template<typename M,
				 typename V,
				 typename = std::enable_if<std::is_base_of<M, Model::BaseModel>::value>,
				 typename = std::enable_if<std::is_base_of<V, View::BaseView<M>>::value>>
		inline void addView( const ID::VTX_ID & p_id, V * const p_view )
		{
			_views.emplace( p_id, (View::BaseView<Model::BaseModel> * const)p_view );
		}

		template<typename M,
				 typename V,
				 typename = std::enable_if<std::is_base_of<M, Model::BaseModel>::value>,
				 typename = std::enable_if<std::is_base_of<V, View::BaseView<M>>::value>>
		inline V * const removeView( const ID::VTX_ID & p_id )
		{
			V * const view = (V * const)_views.at( p_id );
			_views.erase( p_id );
			return view;
		}

		inline View::BaseView<Model::BaseModel> * removeView( const ID::VTX_ID & p_id )
		{
			View::BaseView<Model::BaseModel> * const view = _views.at( p_id );
			_views.erase( p_id );
			return view;
		}

		template<typename M,
				 typename V,
				 typename = std::enable_if<std::is_base_of<M, Model::BaseModel>::value>,
				 typename = std::enable_if<std::is_base_of<V, View::BaseView<M>>::value>>
		inline V * const getView( const ID::VTX_ID & p_id )
		{
			return (V * const)_views.at( p_id );
		}

		inline const bool hasView( const ID::VTX_ID & p_id ) const { return _views.find( p_id ) != _views.end(); }
		inline std::map<ID::VTX_ID, View::BaseView<Model::BaseModel> *> &		getViews() { return _views; }
		inline const std::map<ID::VTX_ID, View::BaseView<Model::BaseModel> *> & getViews() const { return _views; }

		void notifyViews( const Event::VTXEvent * const p_event ) const
		{
			for ( const std::pair<ID::VTX_ID, View::BaseView<Model::BaseModel> *> & pair : _views )
			{
				pair.second->notify( p_event );
			}
		};

	  private:
		Model::BaseModel * const								 _model;
		std::map<ID::VTX_ID, View::BaseView<Model::BaseModel> *> _views
			= std::map<ID::VTX_ID, View::BaseView<Model::BaseModel> *>();
	};

} // namespace VTX::MVC

#endif
