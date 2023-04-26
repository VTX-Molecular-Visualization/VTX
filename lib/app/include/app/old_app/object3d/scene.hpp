#ifndef __VTX_SCENE__
#define __VTX_SCENE__

#include "app/component/chemistry/_fwd.hpp"
#include "app/event.hpp"
#include "app/event/global.hpp"
#include "app/old_app/generic/base_scene_item.hpp"
#include "app/old_app/generic/base_updatable.hpp"
#include "app/old_app/object3d/helper/aabb.hpp"
#include <map>
#include <type_traits>
#include <vector>

namespace VTX::Model
{
	class MeshTriangle;
	class Path;
	class Label;
} // namespace VTX::Model

namespace VTX::Object3D
{
	namespace Helper
	{
		class BaseHelper;
	}

	class CameraManager;
	class Camera;

	class Scene : public Generic::BaseUpdatable
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
		using MoleculePtr			= App::Component::Chemistry::Molecule *;
		using PathPtr				= Model::Path *;
		using MeshTrianglePtr		= Model::MeshTriangle *;
		using LabelPtr				= Model::Label *;
		using HelperPtr				= Object3D::Helper::BaseHelper *;
		using MapMoleculePtrFloat	= std::map<MoleculePtr, float>;
		using PairMoleculePtrFloat	= std::pair<const MoleculePtr, float>;
		using VectorPathPtr			= std::vector<PathPtr>;
		using VectorMeshTrianglePtr = std::vector<MeshTrianglePtr>;
		using VectorLabelPtr		= std::vector<LabelPtr>;
		using VectorHelperPtr		= std::vector<HelperPtr>;

		Scene();
		~Scene();

		Camera &							 getCamera();
		const Camera &						 getCamera() const;
		inline CameraManager &				 getCameraManager() const { return *_cameraManager; }
		inline MapMoleculePtrFloat &		 getMolecules() { return _molecules; };
		inline const MapMoleculePtrFloat &	 getMolecules() const { return _molecules; };
		inline const VectorPathPtr &		 getPaths() const { return _paths; };
		inline const VectorMeshTrianglePtr & getMeshes() const { return _meshes; };
		inline const VectorLabelPtr &		 getLabels() const { return _labels; };
		inline const VectorHelperPtr &		 getHelpers() const { return _helpers; };

		const Object3D::Helper::AABB & getAABB();

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

		const Generic::BaseSceneItem * const getItemAtPosition( const int p_index ) const;
		int									 getItemPosition( const Generic::BaseSceneItem & p_item ) const;
		void changeModelPosition( const Generic::BaseSceneItem & p_item, const int p_position );
		void changeModelsPosition( const std::vector<const Generic::BaseSceneItem *> & p_items, const int p_position );
		void sortMoleculesBySceneIndex( std::vector<App::Component::Chemistry::Molecule *> & p_molecules ) const;

		bool isEmpty() const;

		const Model::Path * const getDefaultPath() const { return _defaultPath; };

		void clear();
		void reset();

		virtual void update( const float & ) override;

	  private:
		int _persistentIDCounter = 0;

		void _computeAABB();
		void _createDefaultPath();

		template<typename T, typename = std::enable_if<std::is_base_of<Generic::BaseSceneItem, T>::value>>
		void _add( T * p_item, std::vector<T *> & p_container )
		{
			p_container.emplace_back( p_item );
			_itemOrder.emplace_back( p_item );
			_applySceneID( *p_item );
		}

		void _updateGraphicMask() const;

		template<typename T, typename = std::enable_if<std::is_base_of<Generic::BaseSceneItem, T>::value>>
		void _remove( T *							   p_item,
					  std::vector<T *> &			   p_container,
					  const VTX::App::Event::Global &  p_removeEvent,
					  const ModelCharacteristicsFlag & p_flag )
		{
			_itemOrder.erase( std::find( _itemOrder.begin(), _itemOrder.end(), p_item ) );
			p_container.erase( std::find( p_container.begin(), p_container.end(), p_item ) );

			if ( ( int( p_flag ) & int( ModelCharacteristicsFlag::AABB ) ) != 0 )
				_aabb.invalidate();

			VTX_EVENT<Generic::BaseSceneItem *>( VTX::App::Event::Global::SCENE_ITEM_REMOVED, p_item );
			VTX_EVENT<T *>( p_removeEvent, p_item );

			if ( ( int( p_flag ) & int( ModelCharacteristicsFlag::GRAPHIC ) ) != 0 )
				_updateGraphicMask();
		}
		template<typename T1,
				 typename T2,
				 typename = std::enable_if<std::is_base_of<Generic::BaseSceneItem, T1>::value>>
		void _remove( T1 *							   p_item,
					  std::map<T1 *, T2> &			   p_container,
					  const VTX::App::Event::Global &  p_removeEvent,
					  const ModelCharacteristicsFlag & p_flag )
		{
			_itemOrder.erase( std::find( _itemOrder.begin(), _itemOrder.end(), p_item ) );
			p_container.erase( p_item );

			if ( ( int( p_flag ) & int( ModelCharacteristicsFlag::AABB ) ) != 0 )
				_aabb.invalidate();

			VTX_EVENT<Generic::BaseSceneItem *>( VTX::App::Event::Global::SCENE_ITEM_REMOVED, p_item );
			VTX_EVENT<T1 *>( p_removeEvent, p_item );

			if ( ( int( p_flag ) & int( ModelCharacteristicsFlag::GRAPHIC ) ) != 0 )
				_updateGraphicMask();
		}

	  private:
		CameraManager *		   _cameraManager = nullptr;
		Object3D::Helper::AABB _aabb;
		MapMoleculePtrFloat	   _molecules = MapMoleculePtrFloat();
		VectorPathPtr		   _paths	  = VectorPathPtr();

		VectorMeshTrianglePtr _meshes  = VectorMeshTrianglePtr();
		VectorLabelPtr		  _labels  = VectorLabelPtr();
		VectorHelperPtr		  _helpers = VectorHelperPtr();

		Model::Path * _defaultPath = nullptr;

		std::vector<const Generic::BaseSceneItem *> _itemOrder = std::vector<const Generic::BaseSceneItem *>();
		void										_applySceneID( Generic::BaseSceneItem & p_item );
	};
} // namespace VTX::Object3D
#endif
