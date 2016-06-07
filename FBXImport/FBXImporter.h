#pragma once

#include <fbxsdk\fbxsdk.h>

#include "Skeleton.h"

class FBXImporter
{
public:
	FbxManager*	manager;
	FbxScene* scene;
	FbxNode* root;

	FBXImporter(const char* fileName);
	~FBXImporter();

	void initialize(const char* fileName);

	void createSkeleton(Skeleton& _sk);

};