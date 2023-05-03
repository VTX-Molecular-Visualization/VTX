#ifndef __VTX_APP_APPLICATION_SCENE__
#define __VTX_APP_APPLICATION_SCENE__

#include "app/application/generic/base_updatable.hpp"
#include "app/component/chemistry/_fwd.hpp"
#include "app/component/object3d/_fwd.hpp"
#include "app/component/object3d/helper/aabb.hpp"
#include "app/component/render/_fwd.hpp"
#include "app/component/video/_fwd.hpp"
#include "app/core/scene/base_scene_item.hpp"
#include "app/event.hpp"
#include "app/event/global.hpp"
#include "app/internal/scene/_fwd.hpp"
#include <map>
#include <type_traits>
#include <vector>

namespace VTX::App::Application
{
	class Scene : public Application::Generic::BaseUpdatable
	{
	  private:
		enum class ModelCharacteristicsFlag
		{
			AABB	= 1 << 0,
			GRAPHIC = 1 << 1,

			NONE = 0,

			MOLECULE = AABB | GRAPHIC,
			PATH	 = NONE,
			MESH	 = AABB | GRAPHIC,
			LABEL	 = GRAPHIC,
			BOX		 = GRAPHIC,

		};

	  public:
		using MoleculePtr			= Component::Chemistry::Molecule *;
		using PathPtr				= Component::Video::Path *;
		using MeshTrianglePtr		= Component::Object3D::MeshTriangle *;
		using LabelPtr				= Component::Object3D::Label *;
		using HelperPtr				= Component::Object3D::Helper::BaseHelper *;
		using MapMoleculePtrFloat	= std::map<MoleculePtr, float>;
		using PairMoleculePtrFloat	= std::pair<const MoleculePtr, float>;
		using VectorPathPtr			= std::vector<PathPtr>;
		using VectorMeshTrianglePtr = std::vector<MeshTrianglePtr>;
		using VectorLabelPtr		= std::vector<LabelPtr>;
		using VectorHelperPtr		= std::vector<HelperPtr>;

		Scene();
		~Scene();

		Component::Render::Camera &				getCamera();
		const Component::Render::Camera &		getCamera() const;
		inline Internal::Scene::CameraManager & getCameraManager() const { return *_cameraManager; }
		inline MapMoleculePtrFloat &			getMolecules() { return _molecules; };
		inline const MapMoleculePtrFloat &		getMolecules() const { return _molecules; };
		inline const VectorPathPtr &			getPaths() const { return _paths; };
		inline const VectorMeshTrianglePtr &	getMeshes() const { return _meshes; };
		inline const VectorLabelPtr &			getLabels() const { return _labels; };
		inline const VectorHelperPtr &			getHelpers() const { return _helpers; };

		const Component::Object3D::Helper::AABB & getAABB();

		void addMolecule( MoleculePtr const, const bool p_sendEvent = true );
		void removeMolecule( MoleculePtr const );
		void addPath( PathPtr const );
		void removePath( PathPtr const );
		void addMesh( MeshTrianglePtr const );
		void removeMesh( MeshTrianglePtr const );
		void addLabel( LabelPtr const );
		void removeLabel( LabelPtr const );

		void addHelper( HelperPtr const );
		void removeHelper( HelperPtr const );

		const Core::Scene::BaseSceneItem * const getItemAtPosition( const int p_index ) const;
		int										 getItemPosition( const Core::Scene::BaseSceneItem & p_item ) const;
		void changeModelPosition( const Core::Scene::BaseSceneItem & p_item, const int p_position );
		void changeModelsPosition( const std::vector<const Core::Scene::BaseSceneItem *> & p_items,
								   const int											   p_position );
		void sortMoleculesBySceneIndex( std::vector<Component::Chemistry::Molecule *> & p_molecules ) const;

		bool isEmpty() const;

		const Component::Video::Path * const getDefaultPath() const { return _defaultPath; };

		void clear();
		void reset();

		virtual void update( const float & ) override;

	  private:
		int _persistentIDCounter = 0;

		void _computeAABB();
		void _createDefaultPath();

		template<typename T, typename = std::enable_if<std::is_base_of<Core::Scene::BaseSceneItem, T>::value>>
		void _add( T * p_item, std::vector<T *> & p_container )
		{
			p_container.emplace_back( p_item );
			_itemOrder.emplace_back( p_item );
			_applySceneID( *p_item );
		}

		void _updateGraphicMask() const;

		template<typename T, typename = std::enable_if<std::is_base_of<Core::Scene::BaseSceneItem, T>::value>>
		void _remove( T *							   p_item,
					  std::vector<T *> &			   p_container,
					  const Event::Global &			   p_removeEvent,
					  const ModelCharacteristicsFlag & p_flag )
		{
			_itemOrder.erase( std::find( _itemOrder.begin(), _itemOrder.end(), p_item ) );
			p_container.erase( std::find( p_container.begin(), p_container.end(), p_item ) );

			if ( ( int( p_flag ) & int( ModelCharacteristicsFlag::AABB ) ) != 0 )
				_aabb.invalidate();

			VTX_EVENT<Core::Scene::BaseSceneItem *>( Event::Global::SCENE_ITEM_REMOVED, p_item );
			VTX_EVENT<T *>( p_removeEvent, p_item );

			if ( ( int( p_flag ) & int( ModelCharacteristicsFlag::GRAPHIC ) ) != 0 )
				_updateGraphicMask();
		}
		template<typename T1,
				 typename T2,
				 typename = std::enable_if<std::is_base_of<Core::Scene::BaseSceneItem, T1>::value>>
		void _remove( T1 *							   p_item,
					  std::map<T1 *, T2> &			   p_container,
					  const Event::Global &			   p_removeEvent,
					  const ModelCharacteristicsFlag & p_flag )
		{
			_itemOrder.erase( std::find( _itemOrder.begin(), _itemOrder.end(), p_item ) );
			p_container.erase( p_item );

			if ( ( int( p_flag ) & int( ModelCharacteristicsFlag::AABB ) ) != 0 )
				_aabb.invalidate();

			VTX_EVENT<Core::Scene::BaseSceneItem *>( Event::Global::SCENE_ITEM_REMOVED, p_item );
			VTX_EVENT<T1 *>( p_removeEvent, p_item );

			if ( ( int( p_flag ) & int( ModelCharacteristicsFlag::GRAPHIC ) ) != 0 )
				_updateGraphicMask();
		}

	  private:
		Internal::Scene::CameraManager *  _cameraManager = nullptr;
		Component::Object3D::Helper::AABB _aabb;
		MapMoleculePtrFloat				  _molecules = MapMoleculePtrFloat();
		VectorPathPtr					  _paths	 = VectorPathPtr();

		VectorMeshTrianglePtr _meshes  = VectorMeshTrianglePtr();
		VectorLabelPtr		  _labels  = VectorLabelPtr();
		VectorHelperPtr		  _helpers = VectorHelperPtr();

		Component::Video::Path * _defaultPath = nullptr;

		std::vector<const Core::Scene::BaseSceneItem *> _itemOrder = std::vector<const Core::Scene::BaseSceneItem *>();
		void											_applySceneID( Core::Scene::BaseSceneItem & p_item );
	};
} // namespace VTX::App::Application
#endif
