#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <memory>
#include "../Geometry/Geometry.h"
#include "../Camera/Camera.h"
#include "../Light/PointLight.h"

namespace rt
{
	class AssimpLoader
	{
	public:
		AssimpLoader();

		AssimpLoader(const AssimpLoader &) = default;

		bool LoadFile(std::string const &filePath);
		Camera GetCameraFromScene() const;
		std::vector<std::shared_ptr<Object>> const &GetMeshesFromScene() const;
		std::vector<std::shared_ptr<PointLight>> const &GetLightsFromScene() const;

	private:
		const aiScene *_scene;
		std::shared_ptr<Assimp::Importer> _importer;
		Camera _camera;
		std::vector<std::shared_ptr<Object>> _meshes;
		std::vector<std::shared_ptr<PointLight>> _lights;

		Vector3<float> _transform(aiMatrix4x4 const &mat, Vector3<float> const &point) const;
		Vector3<float> const _loadMaterialFromMesh(unsigned int matIdx) const;
		void _loadNode(aiNode *node, aiMatrix4x4 const &parent);
	};
} // namespace rt
