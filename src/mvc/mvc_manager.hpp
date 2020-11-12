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
			}

		  public:
			template<typename M, typename = std::enable_if<std::is_base_of<M, Model::BaseModel>::value>>
			M * const instantiate()
			{
				M * const		model = new M();
				MvcData * const mvc	  = new MvcData( model );

				_mvcs->add( mvc );

				model->instantiateDefaultViews();

				return model;
			}

			template<typename M, typename P1, typename = std::enable_if<std::is_base_of<M, Model::BaseModel>::value>>
			M * const instantiate( P1 & p_param1 )
			{
				M * const		model = new M( p_param1 );
				MvcData * const mvc	  = new MvcData( model );

				_mvcs->add( mvc );

				model->instantiateDefaultViews();

				return model;
			}

			template<typename M, typename = std::enable_if<std::is_base_of<M, Model::BaseModel>::value>>
			void deleteModel( M * const p_model )
			{
				MvcData * const mvc = _mvcs->remove( p_model );
				delete mvc;
				delete p_model;
			}

			template<typename M, typename = std::enable_if<std::is_base_of<M, Model::BaseModel>::value>>
			void deleteAllModels( std::vector<M *> & p_models )
			{
				if ( p_models.size() <= 0 )
					return;

				_mvcs->deleteAllMVCs( p_models );

				for ( auto it = p_models.begin(); it != p_models.end(); it++ )
					delete ( *it );

				p_models.clear();
			}

			template<typename M, typename = std::enable_if<std::is_base_of<M, Model::BaseModel>::value>>
			M & getModel( const Model::ID & _id ) const
			{
				Model::BaseModel & model	= _getMvcData( _id )->getModel();
				M &				   modelPtr = static_cast<M &>( model );
				return modelPtr;
			}

			ID::VTX_ID getModelTypeID( const Model::ID & _id ) const { return _getMvcData( _id )->getModel().getTypeId(); };

			template<typename M,
					 typename V,
					 typename = std::enable_if<std::is_base_of<M, Model::BaseModel>::value>,
					 typename = std::enable_if<std::is_base_of<V, View::BaseView<M>>::value>>
			inline void addViewOnModel( const M * const p_model, const ID::VTX_ID & p_id, V * const p_view )
			{
				_getMvcData( p_model )->addView<M, V>( p_id, p_view );
			}

			template<typename M,
					 typename V,
					 typename = std::enable_if<std::is_base_of<M, Model::BaseModel>::value>,
					 typename = std::enable_if<std::is_base_of<V, View::BaseView<M>>::value>>
			inline V * const removeViewOnModel( const M * const p_model, const ID::VTX_ID & p_id )
			{
				return (V * const)_getMvcData( p_model )->removeView<M, V>( p_id );
			}

			inline bool hasView( const Model::BaseModel * const p_model, const ID::VTX_ID & p_id ) { return _getMvcData( p_model )->hasView( p_id ); };

			inline void notifyView( const Model::BaseModel * const p_caller, const Event::VTXEvent * const p_event ) { _getMvcData( p_caller )->notifyViews( p_event ); }

		  private:
			MvcManager()					 = default;
			MvcManager( const MvcManager & ) = delete;
			MvcManager & operator=( const MvcManager & ) = delete;
			~MvcManager() { delete _mvcs; }

			MvcDataContainer * const _mvcs = new MvcDataContainer();

			MvcData * const _getMvcData( const Model::BaseModel * const p_model ) const { return ( *_mvcs )[ p_model ]; }

			MvcData * const _getMvcData( const Model::ID & p_modelId ) const { return ( *_mvcs )[ p_modelId ]; }
		};
	} // namespace MVC
} // namespace VTX

#endif
