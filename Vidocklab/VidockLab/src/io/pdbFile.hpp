/*
 * File: pdbFile.hpp
 * Author: Maxime MARIA
 */

#ifndef __PDB_FILE_HPP__
#define __PDB_FILE_HPP__

#include <map>

#include "exceptions.hpp"
#include "io/filePath.hpp"

namespace vidocklab
{
	// enum class PDBLineTag
	//{
	//	// first line of the entry, contains PDB ID code, classification, and
	//	// date of deposition
	//	HEADER,
	//	// description of the experiment represented in the entry
	//	TITLE,
	//	// list of contributors
	//	AUTHOR,
	//	// contain a listing of the consecutive chemical components covalently
	//	// linked in a linear fashion to form a polymer
	//	SEQRES,
	//	// atomic coordinates for standard amino acids and nucleotides
	//	ATOM,
	//	// non-polymer or other “non-standard” chemical coordinates
	//	HETATM,
	//	// model serial number when multiple models of the same structure
	//	MODEL,
	//	// paired with MODEL
	//	ENDMDL,
	//	// indicates the end of a list of ATOM/HETATM records for a chain
	//	TER,
	//	// invalid tag
	//	INVALID
	//};

	///// TODO: handle multiple models from same file PDBModel is ready for
	// class PDBFile
	//{
	// public:
	//	PDBFile() = delete;
	//	explicit PDBFile(const FilePath &filePath) : m_filePath(filePath) {}
	//	void read(Molecule &protein);
	//	void write(const Molecule &protein)
	//	{
	//		throw PDBFileException("not implemented");
	//	}

	// private:
	//	PDBLineTag getLineTag(const std::string &line);
	//	void readAtom(const std::string &line, Atom &atom);

	// private:
	//	FilePath m_filePath = "";

	//	std::string m_header = UNDEFINED_STRING;
	//	std::vector<std::string> m_title;
	//	std::vector<std::string> m_authors;
	//	std::vector<std::string> m_seqRes;

	// private:
	//	using MapPDBLineTag = std::map<std::string, PDBLineTag>;
	//	static const MapPDBLineTag s_mapPDBLineTag;

	//	static const int PDB_LINE_SIZE = 81;
	//};
} // namespace vidocklab

#endif // __PDB_FILE_HPP__
