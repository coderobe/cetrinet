#ifndef _STATE_H
#define _STATE_H

#include <iostream>
#include <nlohmann/json.hpp>
#include "ui.h"

void state_update(nlohmann::json event_payload);

#endif