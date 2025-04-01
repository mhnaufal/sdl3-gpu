// NOT A PROJECT TO BE TAKEN SERIOUSLY
// Audio: FMOD (https://www.fmod.com/docs)

#pragma once

#include "main_helper.h"

#include "FMOD/fmod.hpp"
#include "FMOD/fmod_errors.h"

/****************************************/
// Audio Context
/****************************************/
namespace context {
struct ContextAudio {
public:
    ContextAudio()
        : music(nullptr)
        , channel(nullptr)
    {
        FMOD_CHECK_ERROR(FMOD::System_Create(&system));
        FMOD_CHECK_ERROR(system->init(512, FMOD_INIT_NORMAL, nullptr));
    }

    auto PlayMusic(const char* path, bool loop = true)
    {
        if (channel) {
            channel->stop();
            music->release();
        }

        FMOD_MODE mode = FMOD_LOOP_NORMAL | FMOD_2D;
        if (!loop)
            mode = FMOD_LOOP_OFF;

        FMOD_CHECK_ERROR(system->createStream(path, mode, nullptr, &music));

        FMOD_CHECK_ERROR(system->playSound(music, nullptr, false, &channel));
    }

    auto UpdateMusic() -> void { system->update(); }

    ~ContextAudio()
    {
        if (music)
            music->release();
        system->close();
        system->release();
    }

private:
    FMOD::System* system{};
    FMOD::Sound* music{};
    FMOD::Channel* channel{};
};
} // namespace context