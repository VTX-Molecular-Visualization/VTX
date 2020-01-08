#include "reader_obj.hpp"
#include "../../defines.hpp"
#include "../../exceptions.hpp"
#include "../../util/color.hpp"
#include "../../util/logger.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace VTX
{
	namespace IO
	{
		bool ReaderOBJ::readFile( const Path & p_path, Model::ModelMolecule & p_molecule )
		{
			VTX_INFO( "Loading " + p_path.getFileName() + "..." );

			Assimp::Importer Importer;

			const aiScene * scene = Importer.ReadFile( p_path.c_str(), 0 );
			if ( !scene )
			{
				VTX_ERROR( "Could not decode file: " + p_path.getFileName() );
				return false;
			}

			VTX_INFO( "Creating models..." );

			// Set molecule properties.
			p_molecule.setName( p_path.getFileNameWithoutExtension() );

			Math::AABB & aabb			  = p_molecule.AABB();
			uint		 chainGlobalIdx	  = 0;
			uint		 residueGlobalIdx = 0;
			uint		 atomGlobalIdx	  = 0;
			uint		 bondGlobalIdx	  = 0;

			// Reserve memory for vectors to avoid pointer loss.
			uint chainCount	  = scene->mNumMeshes;
			uint residueCount = 0;
			uint atomCount	  = 0;

			for ( uint i = 0; i < chainCount; ++i )
			{
				const aiMesh * mesh = scene->mMeshes[ i ];
				residueCount += mesh->mNumFaces;
				atomCount += mesh->mNumVertices;
			}

			p_molecule.getChains().resize( chainCount );
			p_molecule.getResidues().resize( residueCount );
			p_molecule.getAtoms().resize( atomCount );

			// Loop over meshes.
			for ( uint chainIdx = 0; chainIdx < chainCount; ++chainIdx, ++chainGlobalIdx )
			{
				const aiMesh * mesh = scene->mMeshes[ chainIdx ];

				// New chain.
				Model::ModelChain & chain = p_molecule.getChain( chainGlobalIdx );
				chain.setMoleculePtr( &p_molecule );
				chain.setId( chainGlobalIdx );
				chain.setName( mesh->mName.C_Str() );
				chain.setIdFirstResidue( residueGlobalIdx );
				chain.setResidueCount( mesh->mNumFaces );
				chain.setColor( Util::Color::randomPastelColor() );

				///////////////
				chain._fColor[ 0 ] = chain.getColor().x;
				chain._fColor[ 1 ] = chain.getColor().y;
				chain._fColor[ 2 ] = chain.getColor().z;
				//////////////

				// Loop over faces.
				for ( uint residueIdx = 0; residueIdx < mesh->mNumFaces; ++residueIdx, ++residueGlobalIdx )
				{
					const aiFace face = mesh->mFaces[ residueIdx ];

					// New residue.
					Model::ModelResidue & residue = p_molecule.getResidue( residueGlobalIdx );
					residue.setMoleculePtr( &p_molecule );
					residue.setChainPtr( &chain );
					residue.setId( residueGlobalIdx );
					residue.setSymbol( Model::ModelResidue::RESIDUE_SYMBOL::UNKNOWN );
					residue.setIdFirstAtom( atomGlobalIdx );
					residue.setAtomCount( uint( mesh->mNumVertices ) );
					residue.setIdFirstBond( bondGlobalIdx );
					residue.setBondCount( uint( mesh->mNumFaces ) );
					residue.setColor( Util::Color::randomPastelColor() );

					///////////////
					residue._fColor[ 0 ] = residue.getColor().x;
					residue._fColor[ 1 ] = residue.getColor().y;
					residue._fColor[ 2 ] = residue.getColor().z;
					//////////////

					// Loop over vertices in the face.
					for ( uint atomIdx = 0; atomIdx < face.mNumIndices; ++atomIdx, ++atomGlobalIdx, ++bondGlobalIdx )
					{
						uint indice = face.mIndices[ atomIdx ];

						// New atom.
						Model::ModelAtom & atom = p_molecule.getAtom( atomGlobalIdx );
						atom.setMoleculePtr( &p_molecule );
						atom.setChainPtr( &chain );
						atom.setResiduePtr( &residue );
						atom.setId( atomGlobalIdx );
						atom.setSymbol( Model::ModelAtom::ATOM_SYMBOL::UNKNOWN );

						////////////////
						atom._fColor[ 0 ] = atom.getColor().x;
						atom._fColor[ 1 ] = atom.getColor().y;
						atom._fColor[ 2 ] = atom.getColor().z;
						//////////////

						const aiVector3D vector = mesh->mVertices[ indice ];
						Vec3f & atomPosition	= p_molecule.addAtomPosition( Vec3f( vector.x, vector.y, vector.z ) );
						p_molecule.addAtomRadius( atom.getVdwRadius() );

						aabb.extend( atomPosition );

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

	} // namespace IO
} // namespace VTX
