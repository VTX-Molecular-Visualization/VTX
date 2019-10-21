/*
 * File: bboxRep.cpp
 * Author: Maxime MARIA
 */

#include "bboxRep.hpp"

#include "app.hpp"

namespace vidocklab
{
	BboxRep::BboxRep(const AMolecule *mol) : AMoleculeRep(mol)
	{
		m_program = App::get().createProgram("Line");
		m_program->attachShader("line.vert");
		m_program->attachShader("line.frag");

		m_program->link();

		m_uMVPMatrixLoc
			= glGetUniformLocation(m_program->getId(), "uMVPMatrix");

		glLineWidth(m_lineWidth);

		initBuffers();
	}

	BboxRep::~BboxRep()
	{
		glDeleteVertexArrays(1, &m_vao);
		glDeleteBuffers(1, &m_vbo);
		glDeleteBuffers(1, &m_ibo);
	}

	void BboxRep::initBuffers()
	{
		// TODO: does not work any more
		const AABB bb;
		// const AABB &bb = m_refMolecule->m_aabb;
		const Vec3f &bbMin = bb.getMin();
		const Vec3f &bbMax = bb.getMax();
		std::cout << "Bbox: " << bbMin << " / " << bbMax << std::endl;

		const size_t nbVertices = 8;
		const size_t nbIndices = 24;
		m_nbIndices = nbIndices;
		Vec3f bboxCorners[nbVertices] = { bbMin,
										  Vec3f(bbMax.x, bbMin.y, bbMin.z),
										  Vec3f(bbMax.x, bbMax.y, bbMin.z),
										  Vec3f(bbMin.x, bbMax.y, bbMin.z),

										  Vec3f(bbMin.x, bbMin.y, bbMax.z),
										  Vec3f(bbMax.x, bbMin.y, bbMax.z),
										  bbMax,
										  Vec3f(bbMin.x, bbMax.y, bbMax.z) };

		// clang-format off
		GLuint indices[nbIndices] = 
		{ 
			0, 1,
			1, 2,
			2, 3,
			3, 0,

			4, 5,
			5, 6,
			6, 7,
			7, 4,

			0, 4,
			1, 5,
			2, 6,
			3, 7
		};
		// clang-format on

		/// TODO: getPtr() instead of data()
		glGenBuffers(1, &m_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, nbVertices * sizeof(Vec3f),
					 &bboxCorners[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// ibo
		glGenBuffers(1, &m_ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_nbIndices * sizeof(GLuint),
					 &indices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		// vao
		glGenVertexArrays(1, &m_vao);
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
		const GLuint SPHERE_CENTER = 0;

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glEnableVertexAttribArray(SPHERE_CENTER);
		glVertexAttribPointer(SPHERE_CENTER, 3, GL_FLOAT, GL_FALSE,
							  sizeof(Vec3f), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
} // namespace vidocklab
