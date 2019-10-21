/*
 * File: vdwSphereSSBORep.cpp
 * Author: Maxime MARIA
 */

#include "vdwSphereSSBORep.hpp"

#include "app.hpp"

namespace vidocklab
{
	VdwSphereSSBORep::VdwSphereSSBORep(const Molecule &mol) : AMoleculeRep(mol)
	{
		m_program = App::get().createProgram("SphereImpostor");
		m_program->attachShader("sphereImpostor.vert");
		m_program->attachShader("sphereImpostor.frag");

		m_program->link();

		m_uMVMatrixLoc = glGetUniformLocation(m_program->getId(), "uMVMatrix");
		m_uProjMatrixLoc
			= glGetUniformLocation(m_program->getId(), "uProjMatrix");

		initBuffers();
	}

	VdwSphereSSBORep::~VdwSphereSSBORep()
	{
		glDeleteBuffers(1, &m_ssbo);
		glDeleteBuffers(1, &m_ibo);
		glDeleteVertexArrays(1, &m_vao);
	}

	void VdwSphereSSBORep::initBuffers()
	{
		m_nbAtoms = m_refProtein.m_nbAtoms;

		std::cout << m_nbAtoms << std::endl;
		struct Vertex
		{
			glm::vec3 position; // vec3 aligne sur 4 float
			float radius;
			glm::vec3 color;
			int pad0 = 0;

			Vertex(const glm::vec3 &pos, const glm::vec3 &col, float rad)
				: position(pos), radius(rad), color(col)
			{
			}
		};

		std::vector<Vertex> data;
		for (int i = 0; i < m_nbAtoms; ++i)
		{
			data.emplace_back(m_refProtein.m_atomPositions[i],
							  m_refProtein.m_atomColors[i],
							  m_refProtein.m_atomRadii[i]);
		}

		// generate indices
		GLuint quadIndices[6] = { 0, 1, 2, 2, 1, 3 };
		m_indices.reserve(m_nbAtoms * 6);
		for (GLint i = 0; i < m_nbAtoms; ++i)
		{
			std::transform(quadIndices, quadIndices + 6,
						   std::back_inserter(m_indices),
						   [&i](const unsigned &d) { return d + 4 * i; });
		}

		// vao
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);

		// index buffer
		glGenBuffers(1, &m_ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint),
					 m_indices.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glGenBuffers(1, &m_ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, data.size() * sizeof(Vertex),
					 data.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		glBindVertexArray(0);
	}
} // namespace vidocklab
