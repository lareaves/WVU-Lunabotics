#ifndef __SWARM_H__
#define __SWARM_H__

#include <string.h>

#include "particle.h"
#include "sensor.h"
#include "slam.h"

#define PARTICLE_COUNT 250
#define INITIAL_POSITION_VARIANCE 15
#define INITIAL_ANGLE_VARIANCE 30

void swarm_init();
void swarm_filter(raw_sensor_scan*, uint8_t*, int);
particle swarm_get_best();

#endif
