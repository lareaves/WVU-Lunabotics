#include "buffer.h"

uint8_t* buffer_allocate() {
  uint8_t *buffer = malloc(sizeof(uint8_t)*BUFFER_SIZE);
  bzero(buffer, sizeof(uint8_t)*BUFFER_SIZE);
  return buffer;
}

void buffer_attenuate(uint8_t *buffer, double factor) {
  int i;
  for (i = 0; i < BUFFER_SIZE; i++)
    buffer[i] *= 0.75;
}

int buffer_index_from_x_y(double x, double y) {
  int i_x, i_y;
  i_x = x/BUFFER_FACTOR;
  i_y = y/BUFFER_FACTOR;
  if (i_x < BUFFER_WIDTH && i_y < BUFFER_HEIGHT)
    return i_y*BUFFER_WIDTH + i_x;
  else return 0;
}

int x_from_buffer_index(int index) {
  return (index % BUFFER_WIDTH)*BUFFER_FACTOR;
}

int y_from_buffer_index(int index) {
  return (index / BUFFER_WIDTH)*BUFFER_FACTOR;
}

int index_protected(int index) {
  int x, y;
  x = x_from_buffer_index(index);
  y = y_from_buffer_index(index);
  return x_y_protected(x, y);
}

// returns 1 if the position is "protected"
// protected pixels are the 10 buffer pixels around the border
// returns 0 if the position is not protected
int x_y_protected(int x, int y) {
  int protected = 0;
  if (x < BORDER_WIDTH*BUFFER_FACTOR || x > ARENA_WIDTH - BORDER_WIDTH*BUFFER_FACTOR ||
      y < BORDER_WIDTH*BUFFER_FACTOR || y > ARENA_HEIGHT - BORDER_WIDTH*BUFFER_FACTOR)
    protected = 1;
  return protected;
}

int index_is_visible(int index, particle p, raw_sensor_scan s) {
  int visible = 1;
  int i;
  double dx, dy, angle;
  // find vector, centered at particle
  dx = x_from_buffer_index(index) - p.x;
  dy = y_from_buffer_index(index) - p.y;
  // find angle, convert to degrees, adjust for particle
  angle = atan2(dy, dx)*180/M_PI - p.theta;
  i = (angle + 120)/SENSOR_SPACING_USB;
  if (abs(angle) > SENSOR_RANGE_USB/2.0 || sqrt(dx*dx + dy*dy) > s.distances[i])
    visible = 0;
  return visible;
}
