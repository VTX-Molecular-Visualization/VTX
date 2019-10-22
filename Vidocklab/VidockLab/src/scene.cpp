/*
 * File: scene.cpp
 * Author: Maxime MARIA
 */
#include "scene.hpp"

#include "app.hpp"
#include "appConfig.hpp"
#include "freeflyCamera.hpp"
#include "io/arcReader.hpp"
#include "io/ioUtils.hpp"
#include "io/mmtfReader.hpp"
#include "io/pdbFile.hpp"
#include "molecules/moleculeDynamic.hpp"
#include "molecules/moleculeStatic.hpp"

namespace vidocklab
{
	Scene::~Scene()
	{
		if (m_camera != nullptr)
			delete m_camera;
		if (m_molecule != nullptr)
			delete m_molecule;

		for (uint i = 0; i < AMoleculeRep::RepType::NB_REP_TYPES; ++i)
		{
			if (m_moleculeReps[i] != nullptr)
				delete m_moleculeReps[i];
		}
	}

	void Scene::init(const int width, const int height,
					 const FilePath &filePath)
	{
		m_camera = new FreeflyCamera;
		m_camera->setScreenSize(width, height);

		VerboseOStream::cout << "Initializing scene" << std::endl;
		if (!filePath.empty())
		{
		}
		else
		{
			initDefaultStatic();
			// initDefaultDynamic();
		}
	}

	void Scene::initDefaultStatic()
	{
		const FilePath mol
			//	= AppConfig::exeDirPath + "../../VidockLab/data/1aon.mmtf";
			//= AppConfig::exeDirPath + "../../VidockLab/data/4v6x.mmtf";
			//= AppConfig::exeDirPath + "../../VidockLab/data/3kgv.mmtf";
			= AppConfig::exeDirPath + "../../VidockLab/data/3j3q.mmtf";
		//= AppConfig::exeDirPath + "../../VidockLab/data/173D.mmtf";

		m_molecule = new MoleculeStatic();
		MMTFReader::readFirstModel(
			*(dynamic_cast<MoleculeStatic *>(m_molecule)), mol);
		m_molecule->initGLBuffers();
	}

	void Scene::initDefaultDynamic()
	{
		const FilePath traj = AppConfig::exeDirPath
							  //				  +
							  + "../../VidockLab/data/md/dhfr2.arc";
		//+ "../../VidockLab/data/md/dhfr2_2_frame.arc";

		m_molecule = new MoleculeDynamic();
		ArcReader::read(traj, *(dynamic_cast<MoleculeDynamic *>(m_molecule)));
		m_molecule->initGLBuffers();
	}

	void Scene::initFromFile(const FilePath &filePath)
	{
		throw std::exception("Scene::initFromFile not implemented");
	}
} // namespace vidocklab
