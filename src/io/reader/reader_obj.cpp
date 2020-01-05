#include "reader_obj.hpp"
#include "../../defines.hpp"
#include "../../exceptions.hpp"
#include "../../util/color.hpp"
#include "../../util/logger.hpp"
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace VTX
{
	namespace IO
	{
		bool ReaderOBJ::readFile( const Path & p_path, Model::ModelMolecule & p_molecule )
		{
			VTX_INFO( "Loading " + p_path.getFileName() + "..." );
			tinyobj::attrib_t				 attrib;
			std::vector<tinyobj::shape_t>	 shapes;
			std::vector<tinyobj::material_t> materials;

			std::string warning;
			std::string error;

			bool result = tinyobj::LoadObj( &attrib, &shapes, &materials, &warning, &error, p_path.c_str() );

			if ( warning.size() ) { VTX_WARNING( warning ); }
			if ( error.size() ) { VTX_WARNING( error ); }

			if ( result == false )
			{
				VTX_ERROR( "Could not decode file: " + p_path.getFileName() );
				return false;
			}

			VTX_INFO( "Creating models..." );

			// Set molecule properties.
			p_molecule.setName( p_path.getFileNameWithoutExtension() );

			float		 x, y, z;
			Math::AABB & aabb			  = p_molecule.AABB();
			uint		 chainGlobalIdx	  = 0;
			uint		 residueGlobalIdx = 0;
			uint		 atomGlobalIdx	  = 0;
			uint		 bondGlobalIdx	  = 0;

			// Reserve memory for vectors to avoid pointer loss.
			uint chainCount	  = (uint)shapes.size();
			uint residueCount = 0;
			uint atomCount	  = 0;

			for ( size_t s = 0; s < chainCount; s++ )
			{
				residueCount += (uint)shapes[ s ].mesh.num_face_vertices.size();
				for ( size_t f = 0; f < shapes[ s ].mesh.num_face_vertices.size(); f++ )
				{
					atomCount += shapes[ s ].mesh.num_face_vertices[ f ];
				}
			}

			p_molecule.getChains().resize( chainCount );
			p_molecule.getResidues().resize( residueCount );
			p_molecule.getAtoms().resize( atomCount );

			// Loop over shapes
			for ( size_t s = 0; s < shapes.size(); s++, ++chainGlobalIdx )
			{
				// New chain.
				Model::ModelChain & chain = p_molecule.getChain( chainGlobalIdx );
				chain.setMoleculePtr( &p_molecule );
				chain.setId( chainGlobalIdx );
				chain.setName( shapes[ s ].name );
				chain.setIdFirstResidue( residueGlobalIdx );
				chain.setResidueCount( (uint)shapes[ s ].mesh.num_face_vertices.size() );
				chain.setColor( Util::Color::randomPastelColor() );

				///////////////
				chain._fColor[ 0 ] = chain.getColor().x;
				chain._fColor[ 1 ] = chain.getColor().y;
				chain._fColor[ 2 ] = chain.getColor().z;
				//////////////

				// Loop over faces(polygon)
				size_t index_offset = 0;
				for ( size_t f = 0; f < shapes[ s ].mesh.num_face_vertices.size(); f++, ++residueGlobalIdx )
				{
					// New residue.
					Model::ModelResidue & residue = p_molecule.getResidue( residueGlobalIdx );
					residue.setMoleculePtr( &p_molecule );
					residue.setChainPtr( &chain );
					residue.setId( residueGlobalIdx );
					residue.setSymbol( Model::ModelResidue::RESIDUE_SYMBOL::UNKNOWN );

					residue.setIdFirstAtom( atomGlobalIdx );
					residue.setAtomCount( uint( shapes[ s ].mesh.num_face_vertices[ f ] ) );
					residue.setIdFirstBond( bondGlobalIdx );
					residue.setBondCount( uint( shapes[ s ].mesh.num_face_vertices[ f ] ) ); // 2 index by bond.
					residue.setColor( Util::Color::randomPastelColor() );

					///////////////
					residue._fColor[ 0 ] = residue.getColor().x;
					residue._fColor[ 1 ] = residue.getColor().y;
					residue._fColor[ 2 ] = residue.getColor().z;
					//////////////

					// Loop over vertices in the face.
					int fv = shapes[ s ].mesh.num_face_vertices[ f ];
					for ( size_t v = 0; v < fv; v++, ++atomGlobalIdx )
					{
						// New atom.
						Model::ModelAtom & atom = p_molecule.getAtom( atomGlobalIdx );
						atom.setMoleculePtr( &p_molecule );
						atom.setChainPtr( &chain );
						atom.setResiduePtr( &residue );
						atom.setId( atomGlobalIdx );
						atom.setSymbol( Model::ModelAtom::ATOM_SYMBOL::UNKNOWN );

						// access to vertex
						tinyobj::index_t idx = shapes[ s ].mesh.indices[ index_offset + v ];
						tinyobj::real_t	 vx	 = attrib.vertices[ 3 * idx.vertex_index + 0 ];
						tinyobj::real_t	 vy	 = attrib.vertices[ 3 * idx.vertex_index + 1 ];
						tinyobj::real_t	 vz	 = attrib.vertices[ 3 * idx.vertex_index + 2 ];

						x = vx;
						y = vy;
						z = vz;

						tinyobj::real_t red	  = attrib.colors[ 3 * idx.vertex_index + 0 ];
						tinyobj::real_t green = attrib.colors[ 3 * idx.vertex_index + 1 ];
						tinyobj::real_t blue  = attrib.colors[ 3 * idx.vertex_index + 2 ];

						/*
						red	  = materials[ shapes[ s ].mesh.material_ids[ f ] ].ambient[ 0 ];
						green = materials[ shapes[ s ].mesh.material_ids[ f ] ].ambient[ 1 ];
						blue  = materials[ shapes[ s ].mesh.material_ids[ f ] ].ambient[ 2 ];
						*/

						atom.setColor( Vec3f( red, green, blue ) );

						////////////////
						atom._fColor[ 0 ] = atom.getColor().x;
						atom._fColor[ 1 ] = atom.getColor().y;
						atom._fColor[ 2 ] = atom.getColor().z;
						//////////////

						Vec3f & atomPosition = p_molecule.addAtomPosition( Vec3f( x, y, z ) );
						p_molecule.addAtomRadius( atom.getVdwRadius() );

						aabb.extend( atomPosition );
					}
					index_offset += fv;

					// For each bond in the residue.
					uint bondCount = residue.getBondCount();
					for ( uint boundIdx = 0; boundIdx < bondCount * 2u - 1u; boundIdx += 2u, bondGlobalIdx += 2u )
					{
						p_molecule.addBond( residue.getIdFirstAtom() + boundIdx );
						p_molecule.addBond( residue.getIdFirstAtom() + boundIdx + 1u );
						/*
						if ( boundIdx == bondCount * 2u - 2u ) { p_molecule.addBond( residue.getIdFirstAtom() ); }
						else
						{
							p_molecule.addBond( residue.getIdFirstAtom() + boundIdx + 1u );
						}
						*/
					}
				}
			}

			VTX_INFO( "Models created" );

			return true;
		} // namespace IO

	} // namespace IO
} // namespace VTX
