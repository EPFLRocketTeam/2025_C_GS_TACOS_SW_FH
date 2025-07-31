#ifndef SENSORS_LAYOUT_H
#define SENSORS_LAYOUT_H

#include "pte.h"

#ifdef HARDWARE_LAYOUT_FIREHORN

enum sensors_aliases_t {
    SENSOR_1 = 0,
    SENSOR_2 = 1,
    SENSOR_3 = 2,
    SENSOR_4 = 3,
    SENSOR_5 = 4,
    SENSOR_6 = 5,
};

constexpr pte_config_t PTES_CONFIG[] = {
    {&Wire, 0, 10},
    {&Wire, 1, 10},
    {&Wire, 2, 10},
    {&Wire, 3, 10},
    {&Wire, 4, 10},
    {&Wire2, 0, 10},
    {&Wire2, 1, 10},
};

constexpr size_t PTES_AMOUNT = sizeof(PTES_CONFIG) / sizeof(pte_config_t);

#endif
#endif // SENSORS_LAYOUT_H