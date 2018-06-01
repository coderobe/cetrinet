#ifndef _STATE_H
#define _STATE_H

#include <iostream>
#include <nlohmann/json.hpp>
#include "ui.h"

#include "proto/motd.h"
#include "proto/auth.h"
#include "proto/cmsg.h"
#include "proto/smsg.h"
#include "proto/part.h"
#include "proto/error.h"
#include "proto/join.h"
#include "proto/channellist.h"
#include "proto/userlist.h"
#include "proto/gtick.h"
#include "proto/greadystate.h"

void state_update(nlohmann::json event_payload);

#endif