#include "FBXImporter.h"
#include "Skeleton.h"

FBXImporter::FBXImporter(const char * fileName) {
	manager = FbxManager::Create();

	FbxImporter* importer = FbxImporter::Create(manager, "");

	if (!importer->Initialize(fileName)) {
		printf("Call to FbxImporter::Initialize failed\n");
		importer->Destroy();
		return;
	}

	scene = FbxScene::Create(manager, fileName);
}

void FBXImporter::initialize(const char * fileName) {}

void FBXImporter::createSkeleton(Skeleton& _sk) {}