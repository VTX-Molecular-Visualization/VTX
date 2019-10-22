/*
 * File: pdbFile.cpp
 * Author: Maxime MARIA
 */

#include "pdbFile.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

#include "ioUtils.hpp"

#include "molecules/atom.hpp"

namespace vidocklab
{
	// PDBLineTag PDBFile::getLineTag(const std::string &line)
	//{
	//	const MapPDBLineTag::const_iterator it
	//		= s_mapPDBLineTag.find(line.substr(0, 6));

	//	return (it != s_mapPDBLineTag.end() ? it->second : PDBLineTag::INVALID);
	//}

	///// TODO: this only read the first protein model in the file
	///// -> handle multiple models ?
	// void PDBFile::read(Molecule &protein)
	//{
	//	VerboseOStream::cout << "Reading file: " << m_filePath << std::endl;

	//	std::ifstream file;

	//	file.open(m_filePath);
	//	if (!file.is_open())
	//	{
	//		throw std::ios_base::failure("cannot open file: "
	//									 + m_filePath.str());
	//	}

	//	try
	//	{
	//		std::string line;

	//		while (std::getline(file, line))
	//		{
	//			// 1 - 6: line tag
	//			const PDBLineTag tag = getLineTag(line);

	//			if (tag == PDBLineTag::HEADER)
	//			{
	//				m_header = line;
	//			}
	//			else if (tag == PDBLineTag::TITLE)
	//			{
	//				m_title.emplace_back(line);
	//			}
	//			else if (tag == PDBLineTag::AUTHOR)
	//			{
	//				m_authors.emplace_back(line);
	//			}
	//			else if (tag == PDBLineTag::SEQRES)
	//			{
	//				m_seqRes.emplace_back(line);
	//			}
	//			else if (tag == PDBLineTag::ENDMDL)
	//			{
	//				// molecule read we can leave
	//				/// TODO hangle multiple models
	//				break;
	//			}
	//			else if (tag == PDBLineTag::ATOM || tag == PDBLineTag::HETATM)
	//			{
	//				Atom &atom = protein.newAtom();
	//				readAtom(line, atom);
	//				protein.extendAABB(atom);
	//			}
	//			else if (tag == PDBLineTag::INVALID)
	//			{
	//				// invalid tag ignored
	//			}
	//			else
	//			{
	//				// should not occur
	//			}
	//		}
	//		file.close();
	//		protein.initRadiiColors();
	//		protein.computeCentroid();
	//	}
	//	catch (const std::exception &e)
	//	{
	//		file.close();
	//		throw PDBFileException(e.what());
	//	}
	//} // namespace vidocklab

	// void PDBFile::readAtom(const std::string &line, Atom &atom)
	//{
	//	std::istringstream iss;

	//	int anInt;
	//	float aFloat;
	//	char aChar;
	//	std::string aString;
	//	Vec3f aVec3;

	//	// 7 - 11: atom serial number
	//	iss.str(line.substr(6, 5));
	//	iss >> anInt;
	//	atom.setSerialNum(anInt);

	//	// 13 - 16: atom name
	//	aString = line.substr(12, 4);
	//	StringFormat::removeSpaces(aString);
	//	StringFormat::toupper(aString);
	//	atom.setName(aString);

	//	// 17: alternate location indicator
	//	aChar = line[16];
	//	atom.setAltLoc(aChar);

	//	// 18 - 20: residue name
	//	aString = line.substr(17, 3);
	//	StringFormat::removeSpaces(aString);
	//	StringFormat::toupper(aString);
	//	atom.setResName(aString);

	//	// 22: chain identifier
	//	aChar = line[21];
	//	atom.setChainId(aChar);

	//	// 23 - 26: residue sequence number
	//	iss.clear();
	//	iss.str(line.substr(22, 4));
	//	iss >> anInt;
	//	atom.setResId(anInt);

	//	// 27: code for insertion of residues
	//	aChar = line[26];
	//	atom.setResInsertCode(aChar);

	//	// 31 - 38: orthogonal coordinates for X in Angstroms
	//	iss.clear();
	//	iss.str(line.substr(30, 8));
	//	iss >> aVec3.x;

	//	// 39 - 46: orthogonal coordinates for Y in Angstroms
	//	iss.clear();
	//	iss.str(line.substr(38, 8));
	//	iss >> aVec3.y;

	//	// 47 - 54: orthogonal coordinates for Z in Angstroms
	//	iss.clear();
	//	iss.str(line.substr(46, 8));
	//	iss >> aVec3.z;

	//	atom.setPos(aVec3);

	//	// 55 - 60: occupancy
	//	iss.clear();
	//	iss.str(line.substr(54, 6));
	//	iss >> aFloat;
	//	atom.setOccupancy(aFloat);

	//	// 61 - 66: temparature factor
	//	iss.clear();
	//	iss.str(line.substr(60, 6));
	//	iss >> aFloat;
	//	atom.setTempFactor(aFloat);

	//	// 77 - 78: element symbol
	//	aString = line.substr(76, 2);
	//	StringFormat::removeSpaces(aString);
	//	StringFormat::toupper(aString);
	//	atom.setType(Atom::getTypeFromSymbol(aString));

	//	// 79 - 80: charge on the atom
	//	aString = line.substr(78, 2);
	//	StringFormat::removeSpaces(aString);
	//	StringFormat::toupper(aString);
	//	atom.setCharge(aString);
	//}

	//// clang-format off
	// const PDBFile::MapPDBLineTag PDBFile::s_mapPDBLineTag =
	//{
	//	{
	//		// key must be a string of 6 chars
	//		{ "HEADER", PDBLineTag::HEADER },
	//		{ "TITLE ", PDBLineTag::TITLE },
	//		{ "AUTHOR", PDBLineTag::AUTHOR },
	//		{ "SEQRES", PDBLineTag::SEQRES },
	//		{ "ATOM  ", PDBLineTag::ATOM },
	//		{ "HETATM", PDBLineTag::HETATM },
	//		{ "MODEL ", PDBLineTag::MODEL },
	//		{ "ENDMDL", PDBLineTag::ENDMDL },
	//		{ "TER   ", PDBLineTag::TER }
	//	}
	//};
	// clang-format on
} // namespace vidocklab
