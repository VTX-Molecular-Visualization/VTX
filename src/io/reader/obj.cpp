#include "obj.hpp"
#include "define.hpp"
#include "exception.hpp"
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
			bool OBJ::readFile( const Path & p_path, Model::Molecule & p_molecule )
			{
				VTX_INFO( "Loading " + p_path.getFileName() + "..." );

				Assimp::Importer Importer;

				const aiScene * const scene = Importer.ReadFile( p_path.c_str(), 0 );
				if ( !scene )
				{
					VTX_ERROR( "Could not decode file: " + p_path.getFileName() );
					return false;
				}

				VTX_INFO( "Creating models..." );

				// Set molecule properties.
				p_molecule.setName( p_path.getFileNameWithoutExtension() );
				p_molecule.setColor( Util::Color::randomPastelColor() );

				const Math::AABB & aabb				= p_molecule.AABB();
				uint			   chainGlobalIdx	= 0;
				uint			   residueGlobalIdx = 0;
				uint			   atomGlobalIdx	= 0;
				uint			   bondGlobalIdx	= 0;

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

				p_molecule.addChains( chainCount );
				p_molecule.addResidues( residueCount );
				p_molecule.addAtoms( atomCount );

				// Loop over meshes.
				for ( uint chainIdx = 0; chainIdx < chainCount; ++chainIdx, ++chainGlobalIdx )
				{
					const aiMesh * const	 mesh	  = scene->mMeshes[ chainIdx ];
					const aiMaterial * const material = scene->mMaterials[ mesh->mMaterialIndex ];

					// New chain.
					Model::Chain & chain = p_molecule.getChain( chainGlobalIdx );
					chain.setMoleculePtr( &p_molecule );
					chain.setId( chainGlobalIdx );
					chain.setName( mesh->mName.C_Str() );
					chain.setIdFirstResidue( residueGlobalIdx );
					chain.setResidueCount( mesh->mNumFaces );
					chain.setColor( Util::Color::randomPastelColor() );

					// Loop over faces.
					for ( uint residueIdx = 0; residueIdx < mesh->mNumFaces; ++residueIdx, ++residueGlobalIdx )
					{
						const aiFace face = mesh->mFaces[ residueIdx ];

						// New residue.
						Model::Residue & residue = p_molecule.getResidue( residueGlobalIdx );
						residue.setMoleculePtr( &p_molecule );
						residue.setChainPtr( &chain );
						residue.setId( residueGlobalIdx );
						residue.setSymbol( Model::Residue::RESIDUE_SYMBOL::UNKNOWN );
						residue.setIdFirstAtom( atomGlobalIdx );
						residue.setAtomCount( uint( mesh->mNumVertices ) );
						residue.setIdFirstBond( bondGlobalIdx );
						residue.setBondCount( uint( mesh->mNumFaces ) );
						residue.setColor( Util::Color::randomPastelColor() );

						// Loop over vertices in the face.
						for ( uint atomIdx = 0; atomIdx < face.mNumIndices;
							  ++atomIdx, ++atomGlobalIdx, ++bondGlobalIdx )
						{
							uint indice = face.mIndices[ atomIdx ];

							// New atom.
							Model::Atom & atom = p_molecule.getAtom( atomGlobalIdx );
							atom.setMoleculePtr( &p_molecule );
							atom.setChainPtr( &chain );
							atom.setResiduePtr( &residue );
							atom.setId( atomGlobalIdx );
							atom.setSymbol( Model::Atom::ATOM_SYMBOL::UNKNOWN );

							aiColor4D diffuse;
							if ( aiGetMaterialColor( material, AI_MATKEY_COLOR_DIFFUSE, &diffuse ) == AI_SUCCESS )
							{ atom.setColor( Vec3f( diffuse.r, diffuse.g, diffuse.b ) ); }

							const aiVector3D vector = mesh->mVertices[ indice ];
							p_molecule.addAtomPosition( Vec3f( vector.x, vector.y, vector.z ) );
							const Vec3f & atomPosition = p_molecule.getAtomPosition( atomGlobalIdx );
							const float	  atomRadius   = atom.getVdwRadius();
							p_molecule.addAtomRadius( atomRadius );

							p_molecule.extendAABB( atomPosition, atomRadius );

							// Bond.
							p_molecule.addBond( atomGlobalIdx );
							if ( atomIdx == face.mNumIndices - 1 )
							{ p_molecule.addBond( ( atomGlobalIdx - face.mNumIndices + 1 ) ); }
							else
							{
								p_molecule.addBond( ( atomGlobalIdx + 1 ) );
							}
						}
					}
				}

				VTX_INFO( "Models created" );

				return true;
			} // namespace IO
		}	  // namespace Reader
	}		  // namespace IO
} // namespace VTX
