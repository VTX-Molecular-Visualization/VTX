#ifndef __VTX_CORE_STRUCT_MESH__
#define __VTX_CORE_STRUCT_MESH__

#include <util/types.hpp>
#include <vector>

namespace VTX::Core::Struct
{
	/**
	 * @brief Raw data structure representing a mesh.
	 */
	struct Mesh
	{
		/**
		 * @brief Vertices list.
		 */
		std::vector<Vec3f> vertices;

		/**
		 * @brief Normals list.
		 */
		std::vector<Vec3f> normals;

		/**
		 * @brief Indices list.
		 */
		std::vector<Index> indices;

		// TODO: move logic to Util?
		/**
		 * @brief Recompute normals.
		 */
		void computeNormals();

		/**
		 * @brief Merge same vertices and reuse indice.
		 */
		void toIndexed();

		/**
		 * @brief Separate same vertices and generate contigus indices.
		 */
		void toNonIndexed();
	};

} // namespace VTX::Core::Struct

#endif
