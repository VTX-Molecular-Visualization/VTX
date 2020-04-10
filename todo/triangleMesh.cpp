#include "triangleMesh.hpp"

#include <iostream>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>


namespace ISICG_ISIR
{
	// Load a mesh with assimp
	void TriangleMesh::load(const std::string &path)
	{
		Assimp::Importer importer;

		// Read scene and triangulate meshes
		const aiScene *const scene
			= importer.ReadFile(path.c_str(), aiProcess_Triangulate);

		if (scene == nullptr)
		{
			throw std::runtime_error("Cannot import file: " + path);
		}

		const uint nbMeshes = scene->mNumMeshes;
		uint nbTriangles = 0;
		uint nbVertices = 0;

		for (uint i = 0; i < nbMeshes; ++i)
		{
			const aiMesh *const mesh = scene->mMeshes[i];
			nbTriangles += mesh->mNumFaces;
			nbVertices += mesh->mNumVertices;
		}

		_triangles.resize(nbTriangles);
		_vertices.resize(nbVertices);

		uint currentTriangle = 0;
		uint currentVertex = 0;

		for (uint m = 0; m < nbMeshes; ++m)
		{
			const aiMesh *const mesh = scene->mMeshes[m];
			const aiMaterial *const material
				= scene->mMaterials[mesh->mMaterialIndex];

			for (uint f = 0; f < mesh->mNumFaces; ++f, ++currentTriangle)
			{
				const aiFace face = mesh->mFaces[f];

				MeshTriangle &tri = _triangles[currentTriangle];

				// triangulated ! :-)
				for (uint v = 0; v < 3; ++v)
				{
					const uint idV = face.mIndices[v];
					tri._v[v] = idV;
				}
			}

			for (uint v = 0; v < mesh->mNumVertices; ++v, ++currentVertex)
			{
				Vec3f &vertex = _vertices[currentVertex];
				vertex.x = mesh->mVertices[v].x;
				vertex.y = mesh->mVertices[v].y;
				vertex.z = mesh->mVertices[v].z;
			}
		}

		std::cout << "Loaded: " << std::endl
				  << "- nb faces: " << _triangles.size() << std::endl
				  << "- nb vertices: " << _vertices.size() << std::endl;
	}
} // namespace ISICG_ISIR
