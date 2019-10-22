/*
 * File: scene.hpp
 * Author: Maxime MARIA
 */

#ifndef __SCENE_HPP__
#define __SCENE_HPP__

#include <vector>

#include "GL/gl3w.h"
#include "GLSLProgram.hpp"
#include "freeflyCamera.hpp"
#include "io/filePath.hpp"
#include "molecules/aMolecule.hpp"

#include "aMoleculeRep.hpp"

namespace vidocklab
{
	class Scene
	{
	public:
		Scene() = default;
		~Scene();

		void init(const int width, const int height,
				  const FilePath &filePath = "");

	private:
		void initDefaultStatic();
		void initDefaultDynamic();
		void initFromFile(const FilePath &filePath);

	public: /// TODO PRIVATE
		FreeflyCamera *m_camera = nullptr;
		AMolecule *m_molecule = nullptr;

		// set of molecule models
		// std::vector<AMolecule *> m_molecules;

		uint m_currentFrame = 0;

		// TODO put in molecule ?

		// clang-format off
		/*il faut :
		- préparer la selection: faire un conteneur de paire 
		(partie de molécule (bornes), représentation)
		- gérer les résidus et chaines dans arcReader
		;*/
		// clang-format on

		AMoleculeRep *m_moleculeReps[AMoleculeRep::RepType::NB_REP_TYPES]
			= { nullptr };

		// lights...
		//
		// skybox...
		//
	};
} // namespace vidocklab

#endif // __SCENE_HPP__
