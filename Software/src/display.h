//#include "common.h"

int distance_state = 0;
    // distance states:
    // 0:                 distance >= DIST_MAX
    // 1:      DIST_MAX > distance >= DIST_THRESH_1
    // 2: DIST_THRESH_1 > distance >= DIST_THRESH_2
    // 3: DIST_THRESH_2 > distance >= DIST_THRESH_3
    // 4: DIST_THRESH_3 > distance >= DIST_MIN
    // 5:      DIST_Min > distance

void Do_Display(float sensor_value);