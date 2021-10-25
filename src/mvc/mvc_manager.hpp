#ifndef __VTX_MVC_MANAGER__
#define __VTX_MVC_MANAGER__

#include "generic/base_lockable.hpp"
#include "id.hpp"
#include "model/base_model.hpp"
#include "mvc_data.hpp"
#include "view/base_view_3d.hpp"
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace VTX
{
	namespace MVC
	{
		class MvcManager : public Generic::BaseLockable
		{
		  public:
			inline static MvcManager & get()
			{
				static MvcManager instance;
				return instance;
			}

		  public:
			template<typename M, typename = std::enable_if<std::is_base_of<Model::BaseModel, M>::value>>
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

			template<typename M, typename P1, typename = std::enable_if<std::is_base_of<Model::BaseModel, M>::value>>
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

			template<typename M, typename P1, typename = std::enable_if<std::is_base_of<Model::BaseModel, M>::value>>
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

			template<typename M, typename = std::enable_if<std::is_base_of<Model::BaseModel, M>::value>>
			void deleteModel( M * const & p_model )
			{
				const MvcData * const mvc = _container[ p_model->getId() ];
				_container.erase( p_model->getId() );

				for ( const std::pair<ID::VTX_ID, View::BaseView<Model::BaseModel> *> & pair : mvc->getViews() )
				{
					delete pair.second;
				}

				delete mvc;
				delete p_model;
			}

			template<typename M, typename = std::enable_if<std::is_base_of<Model::BaseModel, M>::value>>
			void deleteAllModels( std::vector<M *> & p_models )
			{
				for ( auto it = p_models.begin(); it != p_models.end(); it++ )
				{
					if ( *it != nullptr )
						deleteModel( *it );
				}
			}

			template<typename M, typename = std::enable_if<std::is_base_of<Model::BaseModel, M>::value>>
			const M & getModel( const Model::ID & p_id ) const
			{
				const Model::BaseModel & model	  = _container.at( p_id )->getModel();
				const M &				 modelPtr = static_cast<const M &>( model );
				return modelPtr;
			}

			template<typename M, typename = std::enable_if<std::is_base_of<Model::BaseModel, M>::value>>
			M & getModel( const Model::ID & p_id )
			{
				Model::BaseModel & model	= _container[ p_id ]->getModel();
				M &				   modelPtr = static_cast<M &>( model );
				return modelPtr;
			}

			bool doesModelExists( const Model::ID & p_id ) { return _container.find( p_id ) != _container.end(); }

			const ID::VTX_ID & getModelTypeID( const Model::ID & p_id )
			{
				return _container[ p_id ]->getModel().getTypeId();
			};

			template<typename V,
					 typename M,
					 typename = std::enable_if<std::is_base_of<Model::BaseModel, M>::value>,
					 typename = std::enable_if<std::is_base_of<View::BaseView<M>, V>::value>>
			inline V * const instantiateView( M * const p_model, const ID::VTX_ID & p_id )
			{
				V * const view = new V( p_model );

				static_cast<MvcData *>( _container[ p_model->getId() ] )->addView<M, V>( p_id, view );
				return view;
			}

			template<typename V,
					 typename M,
					 typename = std::enable_if<std::is_base_of<Model::BaseModel, M>::value>,
					 typename = std::enable_if<std::is_base_of<View::BaseView<M>, V>::value>>
			inline V * const instantiateViewWidget( M * const		   p_model,
													const ID::VTX_ID & p_id,
													QWidget * const	   p_parentWidget = nullptr )
			{
				V * const view = new V( p_model, p_parentWidget );
				static_cast<MvcData *>( _container[ p_model->getId() ] )->addView<M, V>( p_id, view );
				return view;
			}

			template<typename V,
					 typename M,
					 typename = std::enable_if<std::is_base_of<Model::BaseModel, M>::value>,
					 typename = std::enable_if<std::is_base_of<View::BaseView<M>, V>::value>>
			inline V * const getView( const M * const p_model, const ID::VTX_ID & p_id )
			{
				return static_cast<MvcData *>( _container[ p_model->getId() ] )->getView<M, V>( p_id );
			}

			template<typename V,
					 typename M,
					 typename = std::enable_if<std::is_base_of<Model::BaseModel, M>::value>,
					 typename = std::enable_if<std::is_base_of<View::BaseView<M>, V>::value>>
			inline void deleteView( const M * const p_model, const ID::VTX_ID & p_id )
			{
				delete static_cast<MvcData *>( _container[ p_model->getId() ] )->removeView<M, V>( p_id );
			}

			inline void deleteView( const Model::BaseModel * const p_model, const ID::VTX_ID & p_id )
			{
				delete _container[ p_model->getId() ]->removeView( p_id );
			}

			inline const bool hasView( const Model::BaseModel * const p_model, const ID::VTX_ID & p_id )
			{
				return _container.find( p_model->getId() ) != _container.end()
					   && _container[ p_model->getId() ]->hasView( p_id );
			};

			inline void notifyViews( const Model::BaseModel * const p_caller, const Event::VTXEvent * const p_event )
			{
				_container[ p_caller->getId() ]->notifyViews( p_event );
			}

		  private:
			MvcManager()					 = default;
			MvcManager( const MvcManager & ) = delete;
			MvcManager & operator=( const MvcManager & ) = delete;
			~MvcManager() { assert( _container.size() == 0 ); }

			std::unordered_map<uint, MvcData *> _container = std::unordered_map<uint, MvcData *>();
		};
	} // namespace MVC
} // namespace VTX

#endif
