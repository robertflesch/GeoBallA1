#ifndef GRAPH_GRAPH_H
#define GRAPH_GRAPH_H

#include <Arduino.h>

typedef uint8_t path_t[6];
typedef path_t graph_t[142];

extern const graph_t ball;

extern const uint8_t ringCenter[10];
extern const uint8_t ringCenterTop[20];
extern const uint8_t ringCenterBottom[20];
#endif //GRAPH_GRAPH_H
