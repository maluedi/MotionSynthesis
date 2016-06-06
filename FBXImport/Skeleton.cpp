#include "Skeleton.h"

Skeleton::Joint& Skeleton::Joint::operator=(const Joint & _jt)
{
	this->m_id = _jt.m_id;
	this->m_name = _jt.m_name;
	this->m_offset = _jt.m_offset;
	this->m_rotation = _jt.m_rotation;

	this->m_parent = NULL;

	this->m_children.clear();
	this->m_children.resize(_jt.m_children.size());
	for (int i = 0; i < this->m_children.size(); i++) {
		this->m_children[i] = _jt.m_children[i];
		this->m_children[i].m_parent = this;
	}

	return *this;
}

Skeleton::Joint::~Joint()
{
}

Eigen::Vector3d Skeleton::computeCom()
{
	Eigen::Vector3d xCom;
	int numJ = numJoints();
	if (numJ > 0) {
		for (int i = 0; i < numJ; i++) {
			Skeleton::Joint* tJoint = findJoint(i);
			xCom += getJointPos(tJoint);
		}
		xCom = xCom*(1.0 / (double)numJ);
	}
	return xCom;
}

Skeleton::~Skeleton()
{
}

Skeleton::Skeleton()
{
}

void Skeleton::setPose(const Skeleton & _sk)
{
	*m_root = *_sk.m_root;
}

int Skeleton::updateAnimationTime()
{
	m_currentPose++;
	setToTime(m_currentPose);
	return m_currentPose;
}

int Skeleton::setToTime(int _i)
{
	int pose = _i % numPoses();

	m_root->m_offset = m_X[pose];
	m_root->m_rotation = m_Q[pose][0];
	int nJoints = numJoints();
	for (int i = 1; i < nJoints; i++) {
		Joint* tJoint = findJoint(i);
		tJoint->m_rotation = m_Q[pose][i];
	}
}

int Skeleton::numJoints() const
{
	return numJoints(m_root);
}

int Skeleton::numJoints(Joint* _jt) const
{
	int size = 0;
	if (_jt) {
		size = 1;
		for (int i = 0; i < _jt->m_children.size(); i++) {
			size += numJoints(&_jt->m_children[i]);
		}
	}
	return size;
}

int Skeleton::numPoses() const
{
	return m_X.size();
}

Skeleton::Joint * Skeleton::findJoint(int _idx) const
{
	return nullptr;
}

Skeleton::Joint * Skeleton::findJoint(int _idx, Joint * _jt) const
{
	return nullptr;
}

Skeleton::Joint * Skeleton::findJoint(const std::string & _name) const
{
	return nullptr;
}

Skeleton::Joint * Skeleton::findJoint(const std::string & _name, Joint * _jt) const
{
	return nullptr;
}

Eigen::Vector3d Skeleton::getJointPos(Joint * _jt)
{
	return Eigen::Vector3d();
}

Eigen::Vector3d Skeleton::getJointPos(int _idx)
{
	return Eigen::Vector3d();
}

Eigen::Vector3d Skeleton::getJointPos(const std::string & _name)
{
	return Eigen::Vector3d();
}

Eigen::Quaterniond Skeleton::getJointRot(Joint * _jt)
{
	return Eigen::Quaterniond();
}

Eigen::Quaterniond Skeleton::getJointRot(int _idx)
{
	return Eigen::Quaterniond();
}

Eigen::Quaterniond Skeleton::getJointRot(const std::string & _name)
{
	return Eigen::Quaterniond();
}
