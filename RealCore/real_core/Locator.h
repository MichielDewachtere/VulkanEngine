#ifndef LOCATOR_H
#define LOCATOR_H

#include <memory>

#include "Audio.h"
#include "NullAudio.h"

namespace real
{
    class Locator final
    {
    public:
        static Audio& GetAudioSystem() { return *m_AudioSystem; }
        static void RegisterAudioSystem(Audio* audio);

        static void ShutDown();
    private:
        inline static std::unique_ptr<Audio> m_AudioSystem = nullptr;
        inline static NullAudio m_DefaultAudioSystem{};
    };
}
#endif // LOCATOR_H