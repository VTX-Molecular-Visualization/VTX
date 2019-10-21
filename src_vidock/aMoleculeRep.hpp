/*
 * File: aMoleculeRep.hpp
 * Author: Maxime MARIA
 */

#ifndef __A_MOLECULE_REP_HPP__
#define __A_MOLECULE_REP_HPP__

#include "GLSLProgramManager.hpp"

#include "freeflyCamera.hpp"
#include "molecules/aMolecule.hpp"

namespace vidocklab
{
	/// TODO : don't copy common buffers
	class AMoleculeRep
	{
	public:
		enum RepType
		{
			WIRE = 0,
			VDW_SPHERES,
			VDW_SPHERES_GEOM_SHADER,
			VDW_SPHERES_GEOM_SHADER_DEFERRED,
			VDW_SPHERES_POINT_SPRITES,
			BBOX,
			CYLINDER,
			CYLINDER_DEFERRED,
			// keep at last
			NB_REP_TYPES
		};

		AMoleculeRep(const AMolecule *mol) : m_refMolecule(mol) {}

		virtual ~AMoleculeRep() = default;

		virtual void setCameraMatrices(const FreeflyCamera &cam) const = 0;

		virtual void draw(const uint first, const uint count) const = 0;

		void use() const
		{
			m_program->use();
		}

		bool isVisible() const
		{
			return m_isVisible;
		}

		void changeVisibility()
		{
			m_isVisible = !m_isVisible;
		}

	protected:
		const AMolecule *m_refMolecule = nullptr;
		GLSLProgram *m_program = nullptr;
		bool m_isVisible = false;
	};
} // namespace vidocklab

#endif // __A_MOLECULE_REP_HPP__
