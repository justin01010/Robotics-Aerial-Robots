// include ros library
#include "ros/ros.h"

// include msg library
#include <turtlesim/Pose.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Point.h>

// include math 
#include <math.h>

using namespace std;

float theta_error;
const float kp_vel[]={1,2};
const float kp_ang=1.8;
const float norm_tolerance = 0.1;
const float angle_tolerance = 0.01;

// turtle pose
turtlesim::Pose leader;
turtlesim::Pose follower1;
turtlesim::Pose follower2;

// goal points
geometry_msgs::Point leader_goal;
geometry_msgs::Point follower1_goal;
geometry_msgs::Point follower2_goal;

// turtle twist
geometry_msgs::Twist follower1_twist;
geometry_msgs::Twist follower2_twist;
geometry_msgs::Twist leader_twist;

// turtle publisher
ros::Publisher follower1_pub;
ros::Publisher follower2_pub;
ros::Publisher leader_pub;


bool reset;

struct XY{
    float x;
	float y;
};

struct XY pos_err_I;



// declare call back function
void leader_cb(const turtlesim::Pose::ConstPtr& msg)
{
	leader = *msg;
}

void follower_cb1(const turtlesim::Pose::ConstPtr& msg)
{
	follower1 = *msg;
}

void follower_cb2(const turtlesim::Pose::ConstPtr& msg)
{
	follower2 = *msg;
}


// transform leader frame to world frame
void leadertoworld2D(geometry_msgs::Point &follower_goal, turtlesim::Pose &leader)
{
	float temp_x = follower_goal.x; float temp_y = follower_goal.y;
	follower_goal.x = cos(-leader.theta)*temp_x + sin(-leader.theta)*temp_y + leader.x;
    follower_goal.y =-sin(-leader.theta)*temp_x + cos(-leader.theta)*temp_y + leader.y;
} 



// rotate the world frame coordinate to body frame 
void worldtobody2D(float &x, float &y, float theta)
{
    float temp_x = x; float temp_y = y;
    x = cos(theta)*temp_x + sin(theta)*temp_y;
    y =-sin(theta)*temp_x + cos(theta)*temp_y;
}


// P control for goal position in world frame 
void Positioncontrol(geometry_msgs::Point &goal, turtlesim::Pose &follower, geometry_msgs::Twist &vel_msg) {

	// error in inertia frame
	pos_err_I.x = goal.x - follower.x;
	pos_err_I.y = goal.y - follower.y;

	// Find the goal_point position in Body(turtlesim) frame
	worldtobody2D(pos_err_I.x, pos_err_I.y, follower.theta);

	// Find the error postion 
	float error_norm = sqrt(pow(pos_err_I.x, 2) + pow(pos_err_I.y, 2));

	// Output boundary
	if (error_norm > 2) error_norm = 2;
	
    // Find the error angle
	float error_theta = atan2(pos_err_I.y,pos_err_I.x);

    if(error_norm < norm_tolerance){
        //in specified distance(tolerance)
        vel_msg.linear.x = 0;
        error_theta =  leader.theta - follower.theta; 
        if(error_theta < angle_tolerance && error_theta > -angle_tolerance){
            //vel_msg.angular.z=0;    
            error_theta =0;
        }
    }else if(error_norm > 3){
        vel_msg.linear.x = kp_vel[1] * error_norm;	
    }else{
        vel_msg.linear.x = kp_vel[0] * error_norm;	
    }
    vel_msg.angular.z= kp_ang * error_theta;  

}  

void run(){
        /*     define formation of turtle 

				follower2 >
						       leader >
				follower1 >    
		*/

        follower1_goal.x = -1;
        follower1_goal.y = -1;

		follower2_goal.x = -1;
		follower2_goal.y = 1;
		
        

        //rotate leader frame to world frame
        leadertoworld2D(follower1_goal, leader);
        leadertoworld2D(follower2_goal, leader);

        Positioncontrol(leader_goal, leader, leader_twist);
        Positioncontrol(follower1_goal, follower1, follower1_twist);
        Positioncontrol(follower2_goal, follower2, follower2_twist);

        leader_pub.publish(leader_twist);
        follower1_pub.publish(follower1_twist);
        follower2_pub.publish(follower2_twist);

    	
}


int main(int argc, char **argv)
{
	ros::init(argc, argv, "turtle_formation");
  	ros::NodeHandle n;

  	// declare publisher & subscriber

	// turtle subscriber
  	ros::Subscriber leader_sub = n.subscribe<turtlesim::Pose>("/turtlesim/leader/pose", 1, leader_cb); 
  	ros::Subscriber follower_sub1 = n.subscribe<turtlesim::Pose>("/turtlesim/follower1/pose", 1, follower_cb1);
	ros::Subscriber follower_sub2 = n.subscribe<turtlesim::Pose>("/turtlesim/follower2/pose", 1, follower_cb2);

	leader_pub = n.advertise<geometry_msgs::Twist>("/turtlesim/leader/cmd_vel",1);
	follower1_pub = n.advertise<geometry_msgs::Twist>("/turtlesim/follower1/cmd_vel", 1);
	follower2_pub = n.advertise<geometry_msgs::Twist>("/turtlesim/follower2/cmd_vel", 1);

		
	// define leader goal point 
    leader_goal.x = 6; 
    leader_goal.y = 6;

	// setting frequency as 10 Hz
  	ros::Rate loop_rate(10);
	while(1){
      	do{
		
        /*
		ROS_INFO("goal x : %f \t y : %f\n",leader_goal.x,leader_goal.y);
    	ROS_INFO("pose x : %f \t y : %f\n",leader.x,leader.y);
    	ROS_INFO("pose theta: %f \n",leader.theta);
        */
        run();

        ros::spinOnce();
		loop_rate.sleep();

	    }while (ros::ok() && ( follower1_twist.linear.x != 0 || follower1_twist.angular.z != 0 || 
                               follower2_twist.linear.x != 0 || follower2_twist.angular.z != 0 ||
                               leader_twist.linear.x != 0    || leader_twist.angular.z != 0       ));
        
        
    	ROS_INFO("Please input (x,y). x>0,y>0");
	    cout<<"desired_X:";
	    cin>>leader_goal.x;
	    cout<<"desired_Y:";
	    cin>>leader_goal.y;	
    }
	return 0;
}


