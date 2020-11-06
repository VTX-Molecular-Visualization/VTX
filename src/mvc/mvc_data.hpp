#ifndef __VTX_MVC_DATA__
#define __VTX_MVC_DATA__

#ifdef _MSC_VER
#pragma once
#endif

#include "generic/has_collection.hpp"
#include "id.hpp"
#include "model/base_model.hpp"
#include "view/base_view.hpp"

namespace VTX
{
	namespace MVC
	{
		class MvcData
		{
		  public:
			MvcData( Model::BaseModel * const p_model ) : _model( p_model ) { _views = Generic::HasCollection<View::BaseView<Model::BaseModel>>(); };

			~MvcData() { _views.clear(); };

			const Model::ID &  getId() const { return _model->getId(); }
			Model::BaseModel & getModel() const { return *_model; };

			template<typename M,
					 typename V,
					 typename = std::enable_if<std::is_base_of<M, Model::BaseModel>::value>,
					 typename = std::enable_if<std::is_base_of<V, View::BaseView<M>>::value>>
			inline void addView( const ID::VTX_ID & p_id, V * const p_view )
			{
				_views.addItem( p_id, (View::BaseView<Model::BaseModel> * const)p_view );
			};

			template<typename M,
					 typename V,
					 typename = std::enable_if<std::is_base_of<M, Model::BaseModel>::value>,
					 typename = std::enable_if<std::is_base_of<V, View::BaseView<M>>::value>>
			inline V * const removeView( const ID::VTX_ID & p_id )
			{
				return (V * const)_views.removeItem( p_id );
			};

			inline bool hasView( const ID::VTX_ID & p_id ) { return _views.getItem( p_id ); };

			void notifyViews( const Event::VTX_EVENT_MODEL & p_event, const Event::VTXEventModelData * const p_eventData = 0 ) const
			{
 				const Generic::HasCollection<View::BaseView<Model::BaseModel>>::MapStringToItemPtr * const mapViews = _views.getItems();

				for ( const Generic::HasCollection<View::BaseView<Model::BaseModel>>::PairStringToItemPtr & pair : *mapViews )
				{
					pair.second->notify( p_event, p_eventData );
				}
			};

		  private:
			Model::BaseModel * const								 _model;
			Generic::HasCollection<View::BaseView<Model::BaseModel>> _views;
		};
	} // namespace MVC
} // namespace VTX

#endif
