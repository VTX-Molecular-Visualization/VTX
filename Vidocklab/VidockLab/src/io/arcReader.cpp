/*
 * File: arcReader.cpp
 * Author : Maxime MARIA
 */

#include "arcReader.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

#include "io/ioUtils.hpp"

namespace vidocklab
{
	void ArcReader::read(const FilePath &fp, MoleculeDynamic &mol)
	{
		std::ifstream file;
		VerboseOStream::cout << "Reading .arc file: " << fp << std::endl;
		std::cout << "WARNING! storing bond twice A/B B/A" << std::endl;
		file.open(fp);
		if (!file.is_open())
		{
			throw std::ios_base::failure("cannot open file: " + fp.str());
		}

		std::vector<std::string> unknownTypes;
		std::vector<std::string> unknownTypesSubstitute;
		try
		{
			// temp data for reading
			std::istringstream iss;
			std::string line;
			std::string aString;
			int aInt;
			float x, y, z;

			// get number of lines
			int nbLines = 0;
			while (std::getline(file, line))
			{
				nbLines++;
			}

			// rewind file
			file.clear(); // clear file state
			file.seekg(0, file.beg);

			// read first line to get number of atoms and name
			std::getline(file, line);
			iss.str(line);
			// number of atoms
			iss >> mol.m_nbAtoms;
			// name
			mol.m_name.clear(); // clear name (should be "undefined")
			mol.m_name = iss.str().substr(iss.tellg());
			std::cout << nbLines << " / " << mol.m_nbAtoms << std::endl;
			// number of frames
			mol.m_nbFrames
				= nbLines / (mol.m_nbAtoms + 1); // +1: description line

			// Allocate memory
			mol.m_atoms.resize(mol.m_nbAtoms);
			mol.m_atomRadii.resize(mol.m_nbAtoms);
			mol.m_atomColors.resize(mol.m_nbAtoms);
			mol.m_frames.resize(mol.m_nbFrames);
			for (uint i = 0; i < mol.m_nbFrames; ++i)
			{
				mol.m_frames[i].m_atomPositions.resize(mol.m_nbAtoms);
			}

			// read first frame (description already read)
			for (uint i = 0; i < mol.m_nbAtoms; ++i)
			{
				Atom &atom = mol.m_atoms[i];
				std::getline(file, line);
				iss.str(line);

				// atom id
				iss >> atom.m_id;
				// -1 cause .arc starts at 1
				atom.m_id--;

				// atom type
				iss >> aString;
				atom.m_type = Atom::getTypeFromSymbol(aString);
				if (atom.m_type == Atom::AtomType::INVALID_TYPE)
				{
					// try with first char (for HN, HO, OH, etc....)
					std::string substitute(1, aString[0]);
					bool insert = true;
					for (uint v = 0; v < unknownTypes.size(); ++v)
					{
						if (unknownTypes[v] == aString)
						{
							insert = false;
							break;
						}
					}
					if (insert)
					{
						unknownTypes.emplace_back(aString);
						unknownTypesSubstitute.emplace_back(substitute);
					}

					atom.m_type = Atom::getTypeFromSymbol(substitute);
					if (atom.m_type == Atom::AtomType::INVALID_TYPE)
					{
						insert = true;
						for (uint v = 0; v < unknownTypes.size(); ++v)
						{
							if (unknownTypes[v] == aString)
							{
								insert = false;
								break;
							}
						}
						if (insert)
						{
							unknownTypes.emplace_back(substitute
													  + " (substitute)");
							unknownTypesSubstitute.emplace_back("nothing");
						}
					}
				}

				// position
				iss >> x;
				iss >> y;
				iss >> z;
				mol.m_frames[0].m_atomPositions[i] = Vec3f(x, y, z);
				mol.m_frames[0].m_aabb.extend(
					mol.m_frames[0].m_atomPositions[i]);

				// radius / color (CPK)
				mol.m_atomRadii[i] = Atom::s_vdwRadii[atom.m_type];
				mol.m_atomColors[i] = Atom::s_colors[atom.m_type];

				// atom "type" in Amoeba
				iss >> aInt;

				if ((aInt == 402 || aInt == 403)
					&& mol.m_frames[0].m_nbBondsNotWater == INVALID_ID)
				{
					mol.m_nbAtomsNotWater = i;
					// divide by 2 because half will be removed with
					// removeDuplicatedBonds
					mol.m_frames[0].m_nbBondsNotWater
						= uint(mol.m_frames[0].m_bonds.size()) / 2u;
					std::cout << "Nb Atoms not water: " << mol.m_nbAtomsNotWater
							  << std::endl;
					std::cout << "Nb bonds not water: "
							  << mol.m_frames[0].m_nbBondsNotWater << std::endl;
				}

				// read bonds
				while (iss >> aInt)
				{
					mol.m_frames[0].m_bonds.emplace_back(i);
					// -1 cause .arc starts at 1
					mol.m_frames[0].m_bonds.emplace_back(aInt - 1);
				}
				iss.clear();
			}

			// remove B/A when A/B exists
			removeDuplicatedBonds(mol.m_frames[0].m_bonds);

			if (mol.m_nbAtomsNotWater == INVALID_ID)
			{
				mol.m_nbAtomsNotWater = mol.m_nbAtoms;
				mol.m_frames[0].m_nbBondsNotWater
					= uint(mol.m_frames[0].m_bonds.size());
				std::cout << "Nb Atoms not water: " << mol.m_nbAtomsNotWater
						  << std::endl;
				std::cout << "Nb bonds not water: " << mol.m_nbBondsNotWater
						  << std::endl;
			}

			// precompute centroid
			mol.m_centroid = mol.m_frames[0].m_centroid
				= mol.m_frames[0].m_aabb.center();
			// TODO init with first frame data, then updated, useful ?
			mol.m_nbBonds = uint(mol.m_frames[0].m_bonds.size());
			mol.m_nbBondsNotWater = mol.m_frames[0].m_nbBondsNotWater;

			// read other frames, no need to read ids, etc.
			for (uint f = 1; f < mol.m_nbFrames; ++f)
			{
				std::getline(file, line); // ignore description line

				for (uint i = 0; i < mol.m_nbAtoms; ++i)
				{
					std::getline(file, line);
					iss.str(line);

					iss >> aInt;	// atom id ignored
					iss >> aString; // atom type ignored

					// position
					iss >> x;
					iss >> y;
					iss >> z;
					mol.m_frames[f].m_atomPositions[i] = Vec3f(x, y, z);

					// extend bb
					mol.m_frames[f].m_aabb.extend(
						mol.m_frames[f].m_atomPositions[i]);

					iss >> aInt; // what's this number ? ignored
								 // read bonds
					if ((aInt == 402 || aInt == 403)
						&& mol.m_frames[f].m_nbBondsNotWater == INVALID_ID)
					{
						// divide by 2 because half will be removed with
						// removeDuplicatedBonds
						mol.m_frames[f].m_nbBondsNotWater
							= uint(mol.m_frames[f].m_bonds.size()) / 2u;
					}
					// read bonds
					while (iss >> aInt)
					{
						mol.m_frames[f].m_bonds.emplace_back(i);
						// -1 cause .arc starts at 1
						mol.m_frames[f].m_bonds.emplace_back(aInt - 1);
					}
					iss.clear();
				}
				// remove B/A when A/B exists
				removeDuplicatedBonds(mol.m_frames[f].m_bonds);
				std::cout << f << " : " << mol.m_frames[f].m_atomPositions[0]
						  << std::endl;
				// precompute centroid
				mol.m_frames[f].m_centroid = mol.m_frames[f].m_aabb.center();

				if (mol.m_frames[f].m_nbBondsNotWater == INVALID_ID)
				{
					mol.m_frames[f].m_nbBondsNotWater
						= uint(mol.m_frames[f].m_bonds.size());
				}
			}

			mol.shrinkVectorsToFit();

			VerboseOStream::cout
				<< "Simulation name: " << mol.m_name << std::endl
				<< "Nb atoms: " << mol.m_nbAtoms << std::endl
				<< "Nb frames: " << mol.m_nbFrames << std::endl
				<< "Initial AABB: " << mol.m_frames[0].m_aabb << std::endl;

			VerboseOStream::cout << "Nb bonds removed: " << m_cptRemoved
								 << std::endl;

			VerboseOStream::cout << "Unknown atom types: " << std::endl;
			for (uint i = 0; i < unknownTypes.size(); ++i)
			{
				VerboseOStream::cout << " - " << unknownTypes[i] << " -> "
									 << unknownTypesSubstitute[i] << std::endl;
			}
		}
		catch (const std::exception &e)
		{
			file.close();
			throw e;
		}
	}

	uint ArcReader::m_cptRemoved = 0;

	// TODO: could be optimized... :-)
	void ArcReader::removeDuplicatedBonds(std::vector<uint> &bonds)
	{
		for (uint i = 0; i < bonds.size(); i += 2)
		{
			const uint id1 = bonds[i];
			const uint id2 = bonds[i + 1];

			for (uint j = i + 2; j < bonds.size(); ++j)
			{
				if (bonds[j] == id2 && bonds[j + 1] == id1)
				{
					bonds.erase(bonds.cbegin() + j);
					bonds.erase(bonds.cbegin() + j);
					m_cptRemoved++;
					// only one duplicated pair should exist
					break;
				}
			}
		}
	}
} // namespace vidocklab
