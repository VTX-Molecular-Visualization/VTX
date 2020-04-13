#include "lib_assimp.hpp"
#include "define.hpp"
#include "util/color.hpp"
#include "util/logger.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace VTX
{
	namespace IO
	{
		namespace Reader
		{
			void LibAssimp::readFile( const Path & p_path, Model::MeshTriangle & p_mesh )
			{
				Assimp::Importer Importer;

				const aiScene * const scene = Importer.ReadFile( p_path.c_str(), aiProcess_Triangulate );
				if ( !scene ) { throw Exception::IOException( "File has not scene" ); }

				const uint nbMeshes	   = scene->mNumMeshes;
				uint	   nbTriangles = 0;
				uint	   nbVertices  = 0;

				for ( uint i = 0; i < nbMeshes; ++i )
				{
					const aiMesh * const mesh = scene->mMeshes[ i ];
					nbTriangles += mesh->mNumFaces;
					nbVertices += mesh->mNumVertices;
				}

				p_mesh.getVertices().resize( nbVertices );
				p_mesh.getIndices().resize( nbTriangles * 3u );

				uint currentTriangle = 0;
				uint currentVertex	 = 0;

				for ( uint m = 0; m < nbMeshes; ++m )
				{
					const aiMesh * const	 mesh	  = scene->mMeshes[ m ];
					const aiMaterial * const material = scene->mMaterials[ mesh->mMaterialIndex ];

					for ( uint f = 0; f < mesh->mNumFaces; ++f, ++currentTriangle )
					{
						const aiFace face = mesh->mFaces[ f ];

						// Model::MeshTriangle::Triangle & tri = p_mesh.getTriangle( currentTriangle );

						for ( uint v = 0; v < 3; ++v )
						{
							// const uint idV								 = face.mIndices[ v ];
							// tri._v[ v ]									 = idV;
							p_mesh.getIndice( currentTriangle * 3u + v ) = face.mIndices[ v ];
						}
					}

					for ( uint v = 0; v < mesh->mNumVertices; ++v, ++currentVertex )
					{
						Vec3f & vertex = p_mesh.getVertice( currentVertex );
						vertex.x	   = mesh->mVertices[ v ].x;
						vertex.y	   = mesh->mVertices[ v ].y;
						vertex.z	   = mesh->mVertices[ v ].z;
					}
				}
			}

			void LibAssimp::readFile( const Path & p_path, Model::Molecule & p_molecule )
			{
				Assimp::Importer Importer;

				const aiScene * const scene = Importer.ReadFile( p_path.c_str(), 0 );
				if ( !scene ) { throw Exception::IOException( "File has not scene" ); }

				// Set molecule properties.
				p_molecule.setName( p_path.getFileNameWithoutExtension() );
				p_molecule.setColor( Util::Color::randomPastelColor() );

				uint chainGlobalIdx	  = 0;
				uint residueGlobalIdx = 0;
				uint atomGlobalIdx	  = 0;
				uint bondGlobalIdx	  = 0;

				// Reserve memory for vectors to avoid pointer loss.
				uint chainCount	  = scene->mNumMeshes;
				uint residueCount = 0;
				uint atomCount	  = 0;

				for ( uint i = 0; i < chainCount; ++i )
				{
					const aiMesh * const mesh = scene->mMeshes[ i ];
					residueCount += mesh->mNumFaces;
					atomCount += mesh->mNumVertices;
				}

				p_molecule.addAtomPositionFrame();
				Model::Molecule::AtomPositionsFrame & frame = p_molecule.getAtomPositionFrame( 0 );

				// Loop over meshes.
				for ( uint chainIdx = 0; chainIdx < chainCount; ++chainIdx, ++chainGlobalIdx )
				{
					const aiMesh * const	 mesh	  = scene->mMeshes[ chainIdx ];
					const aiMaterial * const material = scene->mMaterials[ mesh->mMaterialIndex ];

					// New chain.
					p_molecule.addChain();
					Model::Chain & chain = p_molecule.getChain( chainGlobalIdx );
					chain.setMoleculePtr( &p_molecule );
					chain.setIndex( chainGlobalIdx );
					chain.setName( mesh->mName.C_Str() );
					chain.setIdFirstResidue( residueGlobalIdx );
					chain.setResidueCount( mesh->mNumFaces );
					chain.setColor( Util::Color::randomPastelColor() );

					// Loop over faces.
					for ( uint residueIdx = 0; residueIdx < mesh->mNumFaces; ++residueIdx, ++residueGlobalIdx )
					{
						const aiFace face = mesh->mFaces[ residueIdx ];

						// New residue.
						p_molecule.addResidue();
						Model::Residue & residue = p_molecule.getResidue( residueGlobalIdx );
						residue.setMoleculePtr( &p_molecule );
						residue.setChainPtr( &chain );
						residue.setIndex( residueGlobalIdx );
						residue.setSymbol( Model::Residue::RESIDUE_SYMBOL::UNKNOWN );
						residue.setIdFirstAtom( atomGlobalIdx );
						residue.setAtomCount( uint( mesh->mNumVertices ) );
						// residue.setIdFirstBond( bondGlobalIdx );
						// residue.setBondCount( uint( mesh->mNumFaces ) );
						residue.setColor( Util::Color::randomPastelColor() );

						// Loop over vertices in the face.
						for ( uint atomIdx = 0; atomIdx < face.mNumIndices;
							  ++atomIdx, ++atomGlobalIdx, ++bondGlobalIdx )
						{
							uint indice = face.mIndices[ atomIdx ];

							// New atom.
							p_molecule.addAtom();
							Model::Atom & atom = p_molecule.getAtom( atomGlobalIdx );
							atom.setMoleculePtr( &p_molecule );
							atom.setChainPtr( &chain );
							atom.setResiduePtr( &residue );
							atom.setIndex( atomGlobalIdx );
							atom.setSymbol( Model::Atom::ATOM_SYMBOL::UNKNOWN );

							aiColor4D diffuse;
							if ( aiGetMaterialColor( material, AI_MATKEY_COLOR_DIFFUSE, &diffuse ) == AI_SUCCESS )
							{ atom.setColor( Vec3f( diffuse.r, diffuse.g, diffuse.b ) ); }

							const aiVector3D vector = mesh->mVertices[ indice ];
							frame.emplace_back( Vec3f( vector.x, vector.y, vector.z ) );
							const Vec3f & atomPosition = frame[ atomGlobalIdx ];
							const float	  atomRadius   = atom.getVdwRadius();

							// Bond.
							p_molecule.addBond();
							Model::Bond & bond = p_molecule.getBond( atomGlobalIdx );
							bond.setIndexFirstAtom( atomGlobalIdx );
							bond.setIndexSecondAtom( ( atomIdx == face.mNumIndices - 1 )
														 ? ( atomGlobalIdx - face.mNumIndices + 1 )
														 : ( atomGlobalIdx + 1 ) );
						}
					}
				}
			} // namespace IO
		}	  // namespace Reader
	}		  // namespace IO
} // namespace VTX
