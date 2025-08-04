#ifndef SENSORS_LAYOUT_H
#define SENSORS_LAYOUT_H

#ifdef HARDWARE_LAYOUT_FIREHORN
#include "sensors_layout_fh.h"
#endif

enum ptes_alias_t {
#define DEF(alias, channel, mux, id, min_time) alias,
    PTES_LIST
#undef DEF
    PTES_AMOUNT
};

constexpr pte_config_t PTES_CONFIG[] = {
#define DEF(alias, channel, mux, id, min_time) {&channel, &mux, id, min_time},
    PTES_LIST
#undef DEF
};

#endif // SENSORS_LAYOUT_H