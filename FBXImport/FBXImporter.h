#pragma once

#include <fbxsdk\fbxsdk.h>

#include "Skeleton.h"

class FBXImporter
{
public:
	FbxManager*	manager;
	FbxScene* scene;
	FbxNode* root;
	FbxTime startTime, stopTime, currentTime, frameTime;

	FBXImporter(const char* fileName);
	~FBXImporter();

	bool initialize(const char* fileName, double fps = -1);

	void initImporterAtts(FbxImporter* importer, double fps);

	void createSkeleton(Skeleton& _sk);

	FbxNode* findSkeletonRoot(FbxNode* _pNode);

	void buildHierarchy(FbxNode* _pNode, Skeleton::Joint* _pJoint, int count = 0);

};