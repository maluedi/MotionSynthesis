#include "FBXImporter.h"
#include "Skeleton.h"

FBXImporter::FBXImporter(const char* fileName) {
	manager = FbxManager::Create();
	initialize(fileName);
}

FBXImporter::~FBXImporter() {
	this->manager->Destroy();
	this->manager = NULL;
}

bool FBXImporter::initialize(const char* fileName, double fps) {
	if (scene) {
		scene->Destroy();
		scene = NULL;
		root = NULL;
	}
	FbxImporter* importer = FbxImporter::Create(manager, "");

	if (!importer->Initialize(fileName)) {
		printf("Call to FbxImporter::Initialize failed\n");
		importer->Destroy();
		return false;
	}

	scene = FbxScene::Create(manager, fileName);
	importer->Import(scene);

	initImporterAtts(importer, fps);

	root = (FbxNode *)scene->GetRootNode();

	importer->Destroy();
	//maxNumOfParts = root->GetChildCount(true);

	//numOfPoses = (int)((stopTime.GetMilliSeconds() - startTime.GetMilliSeconds()) / frameTime.GetMilliSeconds()) + 1;
	//joints = std::vector<FBXJoint>(maxNumOfParts + 1);

	return true;
}

void FBXImporter::initImporterAtts(FbxImporter* importer, double fps) {
	if (importer) {
		int animStacks = importer->GetAnimStackCount();

		// Create one animation stack
		//FbxAnimStack* lAnimStack = FbxAnimStack::Create(pScene, "Stack001");

		// Check if there ?s any take info, and assign 
		FbxTakeInfo* lCurrentTakeInfo = importer->GetTakeInfo(0);
		if (lCurrentTakeInfo) {
			startTime = lCurrentTakeInfo->mLocalTimeSpan.GetStart();
			stopTime = lCurrentTakeInfo->mLocalTimeSpan.GetStop();
		} else {
			FbxTimeSpan timeLineTimeSpan;
			scene->GetGlobalSettings().GetTimelineDefaultTimeSpan(timeLineTimeSpan);
			startTime = timeLineTimeSpan.GetStart();
			stopTime = timeLineTimeSpan.GetStop();
		}

		///< NUMBER OF FRAMES TO LOAD:
		// Usually 30 frames per second, 
		// MOCAP comes at 120 fps.
		// Use eCustom to set manual framerate 
		if (fps > 0.0) {
			FbxTime::SetGlobalTimeMode(FbxTime::eCustom, fps);
		}
		frameTime.SetTime(0, 0, 0, 1, 0, scene->GetGlobalSettings().GetTimeMode());
	}
}

void FBXImporter::createSkeleton(Skeleton& _sk) {
	FbxNode* pSkeletonRoot = findSkeletonRoot(root);
	if (pSkeletonRoot) {
		_sk.m_root = new Skeleton::Joint();
		buildHierarchy(pSkeletonRoot, _sk.m_root);
		loadAnimation(_sk);
	}
}

FbxNode* FBXImporter::findSkeletonRoot(FbxNode* _pNode) {
	if (_pNode) {
		FbxNodeAttribute* att = _pNode->GetNodeAttribute();
		if (att && att->GetAttributeType() == FbxNodeAttribute::eSkeleton) {
			return _pNode;
		}
		for (int i = 0; i < _pNode->GetChildCount(); i++) {
			FbxNode* pFound = findSkeletonRoot(_pNode->GetChild(i));
			if (pFound) {
				return pFound;
			}
		}
	}
	return NULL;
}

void FBXImporter::buildHierarchy(FbxNode* _pNode, Skeleton::Joint* _pJoint, int count) {
	if (_pNode && _pNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton) {
		_pJoint->m_id = count++;
		_pJoint->m_name = _pNode->GetName();

		FbxAMatrix& lLocalTransform = _pNode->EvaluateLocalTransform(0, FbxNode::eDestinationPivot);
		FbxAMatrix T;
		T.SetT(lLocalTransform.GetT());
		FbxAMatrix R;
		R.SetR(lLocalTransform.GetR());

		FbxAMatrix localTransform = T * R;

		double* pos = localTransform.GetT().Buffer();
		double* quat = localTransform.GetQ().Buffer();

		_pJoint->m_offset = Eigen::Vector3d(pos[0], pos[1], pos[2]);
		_pJoint->m_rotation = Eigen::Quaterniond(quat[3], quat[0], quat[1], quat[2]).normalized();

		int numChildren = _pNode->GetChildCount();
		if (numChildren > 0) {
			_pJoint->m_children.resize(numChildren);
			for (int i = 0; i < numChildren; i++) {
				Skeleton::Joint* cJoint = new Skeleton::Joint();
				cJoint->m_parent = _pJoint;
				_pJoint->m_children[i] = cJoint;
				buildHierarchy(_pNode->GetChild(i), cJoint, count);
			}
		}
	}
}

void FBXImporter::loadAnimation(Skeleton& _sk) {
	int numOfPoses = (int)((stopTime.GetMilliSeconds() - startTime.GetMilliSeconds()) / frameTime.GetMilliSeconds()) + 1;
	_sk.m_X.resize(numOfPoses);
	_sk.m_Q.resize(numOfPoses);

	int numOfJoints = _sk.numJoints();
	for (int i = 0; i < numOfPoses; i++) {
		_sk.m_Q[i].resize(numOfJoints);
	}

	for (int jointId = 0; jointId < numOfJoints; jointId++) {
		Skeleton::Joint* pJoint = _sk.findJoint(jointId);
		FbxNode* pNode = scene->FindNodeByName(pJoint->m_name.c_str());

		FbxTime curTime = this->startTime;
		for (int pose = 0; pose < numOfPoses; pose++) {
			FbxAMatrix& lLocalTransform = pNode->EvaluateLocalTransform(curTime, FbxNode::eDestinationPivot);
			FbxAMatrix T;
			T.SetT(lLocalTransform.GetT());
			FbxAMatrix R;
			R.SetR(lLocalTransform.GetR());

			FbxAMatrix localTransform = T * R;

			double* pos = localTransform.GetT().Buffer();
			double* rot = localTransform.GetR().Buffer();
			double* quat = localTransform.GetQ().Buffer();

			if (jointId == 0) {
				_sk.m_X[pose] = Eigen::Vector3d(pos[0], pos[1], pos[2]);
			}
			_sk.m_Q[pose][jointId] = Eigen::Quaterniond(quat[3], quat[0], quat[1], quat[2]).normalized();
			curTime += this->frameTime;
		}
	}
	
}
