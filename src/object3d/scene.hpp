#ifndef __VTX_SCENE__
#define __VTX_SCENE__

#include "event/event_manager.hpp"
#include "generic/base_scene_item.hpp"
#include "generic/base_updatable.hpp"
#include "math/aabb.hpp"
#include <map>
#include <type_traits>
#include <vector>

namespace VTX::Model
{
	class MeshTriangle;
	class Molecule;
	class Path;
	class Label;
	class Box;
} // namespace VTX::Model

namespace VTX::Object3D
{
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
		using MoleculePtr			= Model::Molecule *;
		using PathPtr				= Model::Path *;
		using MeshTrianglePtr		= Model::MeshTriangle *;
		using LabelPtr				= Model::Label *;
		using BoxPtr				= Model::Box *;
		using MapMoleculePtrFloat	= std::map<MoleculePtr, float>;
		using PairMoleculePtrFloat	= std::pair<const MoleculePtr, float>;
		using VectorPathPtr			= std::vector<PathPtr>;
		using VectorMeshTrianglePtr = std::vector<MeshTrianglePtr>;
		using VectorLabelPtr		= std::vector<LabelPtr>;
		using VectorBoxPtr			= std::vector<BoxPtr>;

		Scene();
		~Scene();

		inline Camera &						 getCamera() { return *_camera; }
		inline const Camera &				 getCamera() const { return *_camera; }
		inline MapMoleculePtrFloat &		 getMolecules() { return _molecules; };
		inline const MapMoleculePtrFloat &	 getMolecules() const { return _molecules; };
		inline const VectorPathPtr &		 getPaths() const { return _paths; };
		inline const VectorMeshTrianglePtr & getMeshes() const { return _meshes; };
		inline const VectorLabelPtr &		 getLabels() const { return _labels; };
		inline const VectorBoxPtr &			 getBoxes() const { return _boxes; };

		const Math::AABB & getAABB();

		void addMolecule( MoleculePtr const, const bool p_sendEvent = true );
		void removeMolecule( MoleculePtr const );
		void addPath( PathPtr const );
		void removePath( PathPtr const );
		void addMesh( MeshTrianglePtr const );
		void removeMesh( MeshTrianglePtr const );
		void addLabel( LabelPtr const );
		void removeLabel( LabelPtr const );
		void addBox( BoxPtr const );
		void removeBox( BoxPtr const );

		const Generic::BaseSceneItem * const getItemAtPosition( const int p_index ) const;
		int									 getItemPosition( const Generic::BaseSceneItem & p_item ) const;
		void changeModelPosition( const Generic::BaseSceneItem & p_item, const int p_position );
		void changeModelsPosition( const std::vector<const Generic::BaseSceneItem *> & p_items, const int p_position );
		void sortMoleculesBySceneIndex( std::vector<Model::Molecule *> & p_molecules ) const;

		bool isEmpty() const;

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
					  const VTX::Event::Global &	   p_removeEvent,
					  const ModelCharacteristicsFlag & p_flag )
		{
			_itemOrder.erase( std::find( _itemOrder.begin(), _itemOrder.end(), p_item ) );
			p_container.erase( std::find( p_container.begin(), p_container.end(), p_item ) );

			if ( ( int( p_flag ) & int( ModelCharacteristicsFlag::AABB ) ) != 0 )
				_aabb.invalidate();

			VTX_EVENT( new Event::VTXEventPtr<Model::BaseModel>( Event::Global::MODEL_REMOVED, p_item ) );
			VTX_EVENT( new Event::VTXEventPtr<T>( p_removeEvent, p_item ) );

			if ( ( int( p_flag ) & int( ModelCharacteristicsFlag::GRAPHIC ) ) != 0 )
				_updateGraphicMask();
		}
		template<typename T1,
				 typename T2,
				 typename = std::enable_if<std::is_base_of<Generic::BaseSceneItem, T1>::value>>
		void _remove( T1 *							   p_item,
					  std::map<T1 *, T2> &			   p_container,
					  const VTX::Event::Global &	   p_removeEvent,
					  const ModelCharacteristicsFlag & p_flag )
		{
			_itemOrder.erase( std::find( _itemOrder.begin(), _itemOrder.end(), p_item ) );
			p_container.erase( p_item );

			if ( ( int( p_flag ) & int( ModelCharacteristicsFlag::AABB ) ) != 0 )
				_aabb.invalidate();

			VTX_EVENT( new Event::VTXEventPtr<Model::BaseModel>( Event::Global::MODEL_REMOVED, p_item ) );
			VTX_EVENT( new Event::VTXEventPtr<T1>( p_removeEvent, p_item ) );

			if ( ( int( p_flag ) & int( ModelCharacteristicsFlag::GRAPHIC ) ) != 0 )
				_updateGraphicMask();
		}

	  private:
		Camera *			  _camera = nullptr;
		Math::AABB			  _aabb;
		MapMoleculePtrFloat	  _molecules = MapMoleculePtrFloat();
		VectorPathPtr		  _paths	 = VectorPathPtr();
		VectorMeshTrianglePtr _meshes	 = VectorMeshTrianglePtr();
		VectorLabelPtr		  _labels	 = VectorLabelPtr();
		VectorBoxPtr		  _boxes	 = VectorBoxPtr();

		std::vector<const Generic::BaseSceneItem *> _itemOrder = std::vector<const Generic::BaseSceneItem *>();
		void										_applySceneID( Generic::BaseSceneItem & p_item );
	};
} // namespace VTX::Object3D
#endif
