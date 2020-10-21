#ifndef __VTX_MVC_MANAGER__
#define __VTX_MVC_MANAGER__

#ifdef _MSC_VER
#pragma once
#endif

#include "id.hpp"
#include "model/base_model.hpp"
#include "mvc_data.hpp"
#include "mvc_data_container.hpp"
#include "view/base_view.hpp"
//#include "view/ui/widget/molecule_inspector_view.hpp"
#include <type_traits>
#include <vector>

namespace VTX
{
	namespace MVC
	{
		class MvcManager final
		{
		  public:
			inline static MvcManager & get()
			{
				static MvcManager instance;
				return instance;
			};

		  public:
			template<typename M, typename = std::enable_if<std::is_base_of<M, Model::BaseModel>::value>>
			M * const instantiate()
			{
				M *		  model = new M();
				MvcData * mvc	= new MvcData( model );

				_mvcs->add( mvc );

				model->instantiateDefaultViews();

				return model;
			};

			template<typename M, typename P1, typename = std::enable_if<std::is_base_of<M, Model::BaseModel>::value>>
			M * const instantiate( P1 p_param1 )
			{
				M *		  model = new M( p_param1 );
				MvcData * mvc	= new MvcData( model );

				_mvcs->add( mvc );

				model->instantiateDefaultViews();

				return model;
			};

			template<typename M, typename = std::enable_if<std::is_base_of<M, Model::BaseModel>::value>>
			void deleteModel( M * p_model )
			{
				MvcData * mvc = _mvcs->remove( p_model );
				delete mvc;
				delete p_model;
			};

			template<typename M, typename = std::enable_if<std::is_base_of<M, Model::BaseModel>::value>>
			void deleteAllModels( std::vector<M *> & p_models )
			{
				for ( M * element : p_models )
					MVC::MvcManager::get().deleteModel( element );

				p_models.clear();
			}

			inline void addViewOnModel( const Model::BaseModel * const p_model, const ID::VTX_ID & p_id, View::BaseView * const p_view )
			{
				_getMvcData( p_model )->addView( p_id, p_view );
			};
			inline View::BaseView * removeViewOnModel( const Model::BaseModel * const p_model, const ID::VTX_ID & p_id ) { return _getMvcData( p_model )->removeView( p_id ); };
			inline bool				hasView( const Model::BaseModel * const p_model, const ID::VTX_ID & p_id ) { return _getMvcData( p_model )->hasView( p_id ); };
			inline void notifyView( const Model::BaseModel * const p_caller, const Event::VTX_EVENT_MODEL & p_event ) { _getMvcData( p_caller )->notifyViews( p_event ); };

		  private:
			inline MvcManager() {};

			MvcDataContainer * _mvcs = new MvcDataContainer();

			MvcData * _getMvcData( const Model::BaseModel * const p_model ) const { return ( *_mvcs )[ p_model ]; };
		};
	} // namespace MVC
} // namespace VTX

#endif
