#include <string>

#include "ros/ros.h"

#include "aruco_msgs/MarkerArray.h"

#include "simulation_msgs/PoseRobotArray.h"

#include "geometry_msgs/Pose.h"
#include "geometry_msgs/PoseWithCovariance.h"

#include "tf2/LinearMath/Transform.h"
#include "tf2_ros/transform_broadcaster.h"
#include "tf2_geometry_msgs/tf2_geometry_msgs.h"

class RobotPose 
{
public:
	geometry_msgs::Pose pose;
	uint id;
	std::string childFrame;
	std::string parentFrame;
};

class PoseTf2Publisher 
{
public:
	PoseTf2Publisher() : _nh("~")
	{
		_simulationPoseSub = _nh.subscribe("/robots", 1, &PoseTf2Publisher::callback, this);
	}

	void publishRobotPose(RobotPose& robotPose) 
	{	
		geometry_msgs::TransformStamped transformStamped;

		std::ostringstream childFrame;
		childFrame << "Simulated_" << robotPose.id;

		transformStamped.header.stamp = ros::Time::now();
		transformStamped.header.frame_id = "world";
		transformStamped.child_frame_id = childFrame.str();
		
		transformStamped.transform.translation.x = robotPose.pose.position.x;
		transformStamped.transform.translation.y = robotPose.pose.position.y;
		transformStamped.transform.translation.z = robotPose.pose.position.z;

		transformStamped.transform.rotation = robotPose.pose.orientation;

		_transformBroadcaster.sendTransform(transformStamped);
	}

	void callback(const simulation_msgs::PoseRobotArray::ConstPtr &array)
	{
		for (int i = 0; i < array->robots.size(); i++) {
			RobotPose robotPose;
			robotPose.pose = array->robots[i].pose;
			robotPose.id = array->robots[i].id;

			publishRobotPose(robotPose);
		}
	}

private:
	ros::NodeHandle _nh;

	ros::Subscriber _simulationPoseSub;
	ros::Subscriber _arucoPoseSub;

	tf2_ros::TransformBroadcaster _transformBroadcaster;
};


int main(int argc, char** argv) 
{
	ros::init(argc, argv, "pose2tf2_publisher");

	PoseTf2Publisher node;

	ros::spin();
	return 0;
}