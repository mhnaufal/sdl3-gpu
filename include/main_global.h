// NOT A PROJECT TO BE TAKEN SERIOUSLY

/*
! JANGAN PERNAH PAKAI VECTOR
*/

#pragma once

#include "tiny_fiber.h"

namespace context {
struct ContextGlobal {
    bool is_playing{false};
    Uint64 last_tick = {};
    Uint64 new_tick = {};
    uint16_t window_width{1280};
    uint16_t window_height{720};
};
} // namespace context