#include <ros/ros.h>

#include <tf/tf.h> // tf header for resolving tf prefix
#include <sensor_msgs/LaserScan.h>

#include "urg_sensor.h"
#include "urg_utils.h"

std::string frame_id_; ///< Parent tf frame of this link

ros::Publisher laser_pub_;
ros::Publisher primary_echo_pub_;
ros::Publisher secondary_echo_pub_;

/*void print_echo_data(long data[], unsigned short intensity[], int index)
{
    int i;

    // [mm]
    for (i = 0; i < URG_MAX_ECHO; ++i) {
        printf("%ld, ", data[(URG_MAX_ECHO * index) + i]);
    }

    // [1]
    for (i = 0; i < URG_MAX_ECHO; ++i) {
        printf("%d, ", intensity[(URG_MAX_ECHO * index) + i]);
    }
}*/

void publishData(urg_t *urg, long data[], unsigned short intensity[], int data_n, long time_stamp){
  (void)urg;
  
  // Set up multiple output topics
  sensor_msgs::LaserScan scan;
  scan.header.stamp = ros::Time::now();
  scan.header.frame_id = frame_id_;
  scan.angle_min = -2.35619445;
  scan.angle_max = 2.35619445;
  scan.angle_increment = 0.004363322;
  scan.time_increment = 0.000017329;
  scan.scan_time = 0.025;
  long minr;
  long maxr;
  urg_distance_min_max(urg, &minr, &maxr);
  scan.range_min = (float)minr/1000.0;
  scan.range_max = (float)maxr/1000.0;
  
  scan.ranges.resize(data_n);
  scan.intensities.resize(data_n);
  
  sensor_msgs::LaserScan primary = scan;
  sensor_msgs::LaserScan secondary = scan;
  
  //printf("# n = %d, time_stamp = %ld\n", data_n, time_stamp);
  for (int i = 0; i < data_n; i++) {
    scan.ranges[i] = (float)data[(URG_MAX_ECHO * i) + 0]/1000.0;
    primary.ranges[i] = (float)data[(URG_MAX_ECHO * i) + 1]/1000.0;
    secondary.ranges[i] = (float)data[(URG_MAX_ECHO * i) + 2]/1000.0;
    scan.intensities[i] = intensity[(URG_MAX_ECHO * i) + 0];
    primary.intensities[i] = intensity[(URG_MAX_ECHO * i) + 1];
    secondary.intensities[i] = intensity[(URG_MAX_ECHO * i) + 2];
  }
    
  laser_pub_.publish(scan);
  primary_echo_pub_.publish(primary);
  secondary_echo_pub_.publish(secondary);
}

int main(int argc, char **argv)
{
  // Initialize node and nodehandles
  ros::init(argc, argv, "urg_driver");
  ros::NodeHandle n;
  ros::NodeHandle pnh("~");
  
  // Get parameters so we can change these later.
  std::string tf_prefix;
  pnh.param<std::string>("tf_prefix", tf_prefix, "");
  pnh.param<std::string>("frame_id", frame_id_, "laser");
  frame_id_ = tf::resolve(tf_prefix, frame_id_);

  laser_pub_ = n.advertise<sensor_msgs::LaserScan>("scan", 20);
  primary_echo_pub_ = n.advertise<sensor_msgs::LaserScan>("secondary_scan", 20);
  secondary_echo_pub_ = n.advertise<sensor_msgs::LaserScan>("third_scan", 20);
  
  // Set up the urgwidget
  urg_t urg;
  int max_data_size;
  long *data = NULL;
  unsigned short *intensity = NULL;
  long time_stamp;
  
  const char *ip_address = "192.168.0.10";
  urg_connection_type_t connection_type = URG_ETHERNET;
  long baudrate_or_port = 10940;
  const char *device = ip_address;

  ROS_INFO("Opening laser.");
  if (urg_open(&urg, connection_type, device, baudrate_or_port) < 0) {
      printf("urg_open: %s, %ld: %s\n", device, baudrate_or_port, urg_error(&urg));
      return 1;
  }

  max_data_size = urg_max_data_size(&urg);
  data = (long *)malloc(max_data_size * 3 * sizeof(data[0]));
  intensity = (unsigned short *)malloc(max_data_size * 3 *
					sizeof(intensity[0]));

  if (!data) {
      return 1;
  }
  
  urg_start_measurement(&urg, URG_MULTIECHO_INTENSITY, 0, 0);
 
  // Loop until ros exits
  while(ros::ok()){
    int n = urg_get_multiecho_intensity(&urg, data, intensity, &time_stamp);
    if (n <= 0) {
	printf("urg_get_multiecho_intensity: %s\n", urg_error(&urg));
	break;
    }
    publishData(&urg, data, intensity, n, time_stamp);
    ros::spinOnce(); // Check in with ros
  }
  
  free(data);
  free(intensity);
  urg_close(&urg);

  return EXIT_SUCCESS;
}