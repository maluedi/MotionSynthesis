#pragma once

#include <eigen3.2.5\Eigen\Dense>
//#include <eigen3.2.5\Eigen\Geometry>

#include <string>
#include <vector>

class Skeleton
{
public:

	struct Joint
	{
		int m_id;
		std::string	m_name;
		Eigen::Vector3d m_offset;
		Eigen::Quaterniond m_rotation;
		Joint* m_parent;
		std::vector<Joint*> m_children;

		Joint& operator=(const Joint& _jt);

		~Joint();
		Joint() : m_parent(NULL), m_id(-1) {}
	};

	double m_scale;

	Joint* m_root;

	int m_currentPose;

	typedef std::vector<Eigen::Quaterniond> m_PoseQ;
	std::vector<Eigen::Vector3d> m_X;
	std::vector<m_PoseQ> m_Q;

	int updateAnimationTime();
	int setToTime(int _i);

	int numJoints() const;

	int numPoses() const;

	Joint* findJoint(int _idx) const;
	Joint* findJoint(const std::string& _name) const;
	
	Eigen::Vector3d getJointPos(Joint* _jt);
	Eigen::Vector3d getJointPos(int _idx);
	Eigen::Vector3d getJointPos(const std::string& _name);

	Eigen::Quaterniond getJointRot(Joint* _jt);
	Eigen::Quaterniond getJointRot(int _idx);
	Eigen::Quaterniond getJointRot(const std::string& _name);

	Eigen::Vector3d computeCom();

	~Skeleton();
	Skeleton();

	void setPose(const Skeleton& _sk);

private:
	int numJoints(Joint* _jt) const;
	Joint* findJoint(int _idx, Joint* _jt) const;
	Joint* findJoint(const std::string& _name, Joint* _jt) const;

};