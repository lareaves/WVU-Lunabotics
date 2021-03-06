using System;
using System.Runtime.InteropServices;

namespace Slam
{
	public class Swarm
	{
		#region imports
		/*
		 * swarm_init
		 * m: number of sensor steps
		 * degrees: range covered by sensor in degrees
		 * long_side: length of long side of arena in mm
		 * short_side: length of short side of arena in mm
		 * start: length of start area in mm
		 */
		[DllImport ("slam")] public static extern int swarm_init(int m, int degrees, int long_side, int short_side, int start);
		/*
		 * swarm_move
		 * dx: change in x (long side) since last update in mm
		 * dy: change in y (short side) since last update in mm
		 * dtheta: change in theta (+ theta is counterclockwise)
		 */
		[DllImport ("slam")] public static extern void swarm_move(int dx, int dy, int dtheta);
		/*
		 * swarm_update
		 * sensor_distances: m measurements in mm (this is how the sensor returns it)
		 */
		[DllImport ("slam")] public static extern void swarm_update(ref int[] sensor_distances);
		[DllImport ("slam")] public static extern int swarm_get_best_x();
		[DllImport ("slam")] public static extern int swarm_get_best_y();
		[DllImport ("slam")] public static extern int swarm_get_best_theta();
		#endregion
	}
}
