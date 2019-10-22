/*
 * File: aMolecule.hpp
 * Author : Maxime MARIA
 */

#ifndef __A_MOLECULE_HPP__
#define __A_MOLECULE_HPP__

#include <vector>

#include "GL/gl3w.h"

#include "atom.hpp"
#include "chain.hpp"
#include "defines.hpp"
#include "math/aabb.hpp"
#include "math/modelMatrix.hpp"
#include "residue.hpp"

namespace vidocklab
{
	struct AMolecule
	{
		AMolecule() = default;
		virtual ~AMolecule()
		{
			glDeleteBuffers(1, &m_atomPosVBO);
			glDeleteBuffers(1, &m_bondsIBO);
			glDeleteBuffers(1, &m_radiiVBO);
			glDeleteBuffers(1, &m_colorVBO);
			glDeleteVertexArrays(1, &m_vao);
		}

		const float *getRadPtr() const
		{
			return m_atomRadii.data();
		}

		const Vec3f *getColorPtr() const
		{
			return m_atomColors.data();
		}

		void bindVAO() const
		{
			glBindVertexArray(m_vao);
		}
		void unbindVAO() const
		{
			glBindVertexArray(0);
		}

		void bindIBO() const
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bondsIBO);
		}
		void unbindIBO() const
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}

		virtual void shrinkVectorsToFit() = 0
		{
			m_atomRadii.shrink_to_fit();
			m_atomColors.shrink_to_fit();
			m_chains.shrink_to_fit();
			m_residues.shrink_to_fit();
			m_atoms.shrink_to_fit();
		}

		virtual void initGLBuffers() = 0;
		virtual void updateFrame(const uint idFrame) = 0;

		Vec3f m_centroid = VEC3F_ZERO;
		uint m_nbFrames = 1;

		// ============================ attributes
		ModelMatrix m_modelMatrix;

		std::string m_name = UNDEFINED_STRING;

		// position and bonds are in child class
		// depends if static or dynamic system
		std::vector<float> m_atomRadii;
		std::vector<Vec3f> m_atomColors;

		std::vector<Chain> m_chains;
		std::vector<Residue> m_residues;
		std::vector<Atom> m_atoms;

		uint m_nbChains = 0;
		uint m_nbResidues = 0;
		uint m_nbAtoms = 0;
		uint m_nbBonds = 0;
		uint m_nbAtomsNotWater = INVALID_ID;
		uint m_nbBondsNotWater = INVALID_ID;

		// ============================ OpenGL buffers
		// atom positions vbo
		GLuint m_atomPosVBO = GL_INVALID_VALUE;
		// bonds ibo
		GLuint m_bondsIBO = GL_INVALID_VALUE;
		// radii vbo
		// TODO compress with position
		GLuint m_radiiVBO = GL_INVALID_VALUE;
		// color vbo
		// TODO ? use SSBO ? ok for atom colors (CPK/residue/chain)
		// but for energy based coloration, useless...
		GLuint m_colorVBO = GL_INVALID_VALUE;
		// vao
		GLuint m_vao = GL_INVALID_VALUE;

		enum AttributeLocation
		{
			ATOM_POSITION = 0,
			ATOM_COLOR = 1,
			ATOM_RADIUS = 2
		};
	};
} // namespace vidocklab

#endif // __A_MOLECULE_HPP__
