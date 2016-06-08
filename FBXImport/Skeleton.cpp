#include "Skeleton.h"

Skeleton::Joint& Skeleton::Joint::operator=(const Joint& _jt) {
	this->m_id = _jt.m_id;
	this->m_name = _jt.m_name;
	this->m_offset = _jt.m_offset;
	this->m_rotation = _jt.m_rotation;

	this->m_parent = NULL;

	this->m_children.clear();
	this->m_children.resize(_jt.m_children.size());
	for (int i = 0; i < this->m_children.size(); i++) {
		this->m_children[i] = new Joint();
		*(this->m_children[i]) = *(_jt.m_children[i]);
		this->m_children[i]->m_parent = this;
	}

	return *this;
}

Skeleton::Joint::~Joint() {}

Eigen::Vector3d Skeleton::computeCom() {
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

Skeleton::~Skeleton() {}

Skeleton::Skeleton() {}

void Skeleton::setPose(const Skeleton& _sk) {
	*m_root = *_sk.m_root;
}

int Skeleton::updateAnimationTime() {
	m_currentPose++;
	setToTime(m_currentPose);
	return m_currentPose;
}

int Skeleton::setToTime(int _i) {
	int pose = _i % numPoses();

	m_root->m_offset = m_X[pose];
	m_root->m_rotation = m_Q[pose][0];
	int nJoints = numJoints();
	for (int jt = 1; jt < nJoints; jt++) {
		Joint* tJoint = findJoint(jt);
		tJoint->m_rotation = m_Q[pose][jt];
	}
}

int Skeleton::numJoints() const {
	return numJoints(m_root);
}

int Skeleton::numJoints(Joint* _jt) const {
	int size = 0;
	if (_jt) {
		size = 1;
		for (int i = 0; i < _jt->m_children.size(); i++) {
			size += numJoints(_jt->m_children[i]);
		}
	}
	return size;
}

int Skeleton::numPoses() const {
	return m_X.size();
}

Skeleton::Joint* Skeleton::findJoint(int _idx) const {
	return findJoint(_idx, m_root);
}

Skeleton::Joint* Skeleton::findJoint(int _idx, Joint* _jt) const {
	if (_jt->m_id == _idx) {
		return _jt;
	} else {
		for (int i = 0; i < _jt->m_children.size(); i++) {
			Joint* tJoint = findJoint(_idx, _jt->m_children[i]);
			if (tJoint) {
				return tJoint;
			}
		}
	}
	return NULL;
}

Skeleton::Joint* Skeleton::findJoint(const std::string& _name) const {
	return findJoint(_name, m_root);
}

Skeleton::Joint* Skeleton::findJoint(const std::string& _name, Joint* _jt) const {
	if (_jt->m_name == _name) {
		return _jt;
	} else {
		for (int i = 0; i < _jt->m_children.size(); i++) {
			Joint* tJoint = findJoint(_name, _jt->m_children[i]);
			if (tJoint) {
				return tJoint;
			}
		}
	}
	return NULL;
}

Eigen::Vector3d Skeleton::getJointPos(Joint* _jt) {
	Eigen::Vector3d pos;
	if (_jt) {
		Joint* tJoint = _jt;
		while (tJoint->m_parent) {
			Eigen::Vector3d offset = tJoint->m_offset;
			pos = tJoint->m_parent->m_rotation._transformVector(pos + offset);
			tJoint = tJoint->m_parent;
		}
		pos += m_root->m_offset;
	}
	return pos;
}

Eigen::Vector3d Skeleton::getJointPos(int _idx) {
	Joint* tJoint = findJoint(_idx);
	return getJointPos(tJoint);
}

Eigen::Vector3d Skeleton::getJointPos(const std::string& _name) {
	Joint* tJoint = findJoint(_name);
	return getJointPos(tJoint);
}

Eigen::Quaterniond Skeleton::getJointRot(Joint* _jt) {
	Eigen::Quaterniond rot = Eigen::Quaterniond::Identity();
	if (_jt) {
		rot = _jt->m_rotation;
		Joint* tJoint = _jt;
		while (tJoint->m_parent) {
			tJoint = tJoint->m_parent;
			rot = tJoint->m_rotation*rot;
		}
	}
	return rot;
}

Eigen::Quaterniond Skeleton::getJointRot(int _idx) {
	Joint* tJoint = findJoint(_idx);
	return getJointRot(tJoint);
}

Eigen::Quaterniond Skeleton::getJointRot(const std::string& _name) {
	Joint* tJoint = findJoint(_name);
	return getJointRot(tJoint);
}
