/*
 * File: mmtfReader.cpp
 * Author : Maxime MARIA
 */

#include "mmtfReader.hpp"

#include "glm/gtc/matrix_transform.hpp"

#include "colorGenerators.hpp"
#include "exceptions.hpp"
#include "ioUtils.hpp"
#include "math/mathUtils.hpp"

namespace vidocklab
{
	void MMTFReader::readFirstModel(MoleculeStatic &mol, const FilePath &fp)
	{
		/// TODO: bioAssemblyList ? -> transforms

		// Decode MMTF
		mmtf::StructureData data;
		mmtf::decodeFromFile(data, fp.c_str());

		// Check for consistency
		if (data.hasConsistentData(true))
		{
			VerboseOStream::cout << "Successfully read " << fp << std::endl;
		}
		else
		{
			throw MMTFIOException("Inconsistent file");
		}

		// Set protein properties
		mol.m_name = data.title;
		mol.m_nbChains = data.chainsPerModel[0];
		mol.m_chains.resize(mol.m_nbChains);

		uint chainId = 0;
		uint residueId = 0;
		uint atomId = 0;
		uint bondId = 0;

		// ====================================================
		// ======== 1
		// ====================================================

		// To read coordinates
		float x, y, z;

		// For each chain in the model 0
		for (uint chainId = 0; chainId < mol.m_nbChains; ++chainId)
		{
			Chain &chain = mol.m_chains[chainId];
			// Set chain properties
			chain.m_refMolecule = &mol;
			chain.m_idFirstResidue = residueId;
			chain.m_nbResidues = data.groupsPerChain[chainId];
			chain.m_id = chainId;
			chain.m_name = data.chainNameList[chain.m_id];
			// Increment protein number of residues
			mol.m_nbResidues += chain.m_nbResidues;

			const Vec3f chainColor = randomPastelColor();

			// For each residue in the chain
			for (uint j = 0; j < chain.m_nbResidues; ++j, ++residueId)
			{
				// Get the MMTF residue (group)
				const mmtf::GroupType &group
					= data.groupList[data.groupTypeList[residueId]];

				// Add a new residue
				mol.m_residues.emplace_back(Residue());
				Residue &residue = mol.m_residues.back();

				// Set residue properties
				residue.m_refMolecule = &mol;
				residue.m_refChain = &chain;
				residue.m_idFirstAtom = atomId;
				residue.m_nbAtoms = uint(group.atomNameList.size());
				residue.m_idFirstBond = bondId;
				residue.m_nbBonds = uint(group.bondAtomList.size()) / 2u;
				residue.m_id = residueId;
				const std::string &resSymbol = group.groupName;
				residue.m_type = Residue::getTypeFromSymbol(resSymbol);

				// For each atom in the residue
				for (uint k = 0; k < residue.m_nbAtoms; ++k, ++atomId)
				{
					/// TODO: mmtf::is_hetatm ?
					mol.m_atoms.emplace_back(Atom());
					Atom &atom = mol.m_atoms.back();

					atom.m_refMolecule = &mol;
					atom.m_refChain = &chain;
					atom.m_refResidue = &residue;

					atom.m_id = atomId;
					const std::string &atomSymbol = group.elementList[k];
					atom.m_type = Atom::getTypeFromSymbol(atomSymbol);

					x = data.xCoordList[atomId];
					y = data.yCoordList[atomId];
					z = data.zCoordList[atomId];

					mol.m_atomPositions.emplace_back(Vec3f(x, y, z));
					mol.m_atomRadii.emplace_back(Atom::s_vdwRadii[atom.m_type]);
					// mol.m_atomColors.emplace_back(Atom::s_colors[atom.m_type]);

					// mol.m_atomColors.emplace_back(
					// Residue::s_colors[residue.m_type]);
					mol.m_atomColors.emplace_back(chainColor);
					mol.m_aabb.extend(mol.m_atomPositions[atom.m_id]);
				}

				// For each bond in the residue
				for (uint k = 0; k < residue.m_nbBonds * 2; k += 2)
				{
					mol.m_bonds.emplace_back(mol.m_nbAtoms
											 + group.bondAtomList[k]);
					mol.m_bonds.emplace_back(mol.m_nbAtoms
											 + group.bondAtomList[k + 1u]);
				}
				mol.m_nbAtoms += residue.m_nbAtoms;
			}
		}

		mol.m_bonds.insert(mol.m_bonds.end(), data.bondAtomList.begin(),
						   data.bondAtomList.end());
		mol.m_nbBonds = uint(mol.m_bonds.size());

		//assert(mol.m_nbResidues == mol.m_residues.size());
		//assert(mol.m_nbAtoms == mol.m_atoms.size());
		//assert(mol.m_nbBonds * 2 == mol.m_bonds.size());
		mol.m_centroid = mol.m_aabb.center();
		mol.shrinkVectorsToFit();

		if (mol.m_nbAtomsNotWater == INVALID_ID)
		{
			std::cout << "WARNING! water molecules not identified" << std::endl;
			mol.m_nbAtomsNotWater = mol.m_nbAtoms;
			mol.m_nbBondsNotWater = mol.m_nbBonds;
		}

		std::cout << "Nb atoms: " << mol.m_nbAtoms << std::endl;
		std::cout << "Nb bonds: " << mol.m_nbBonds << std::endl;
		std::cout << "Centroid: " << mol.m_centroid << std::endl;
		std::cout << "AABB: " << mol.m_aabb.getMin() << " - "
				  << mol.m_aabb.getMax() << std::endl;

		if (!Residue::s_unknownTypes.empty())
		{
			std::cout << "Unknown residue type(s):" << std::endl;
			for (uint i = 0; i < Residue::s_unknownTypes.size(); ++i)
			{
				std::cout << " - " << Residue::s_unknownTypes[i] << std::endl;
			}
		}
		if (!Atom::s_unknownTypes.empty())
		{
			std::cout << "Unknown atom type(s):" << std::endl;
			for (uint i = 0; i < Atom::s_unknownTypes.size(); ++i)
			{
				std::cout << " - " << Atom::s_unknownTypes[i] << std::endl;
			}
		}
	}
} // namespace vidocklab
