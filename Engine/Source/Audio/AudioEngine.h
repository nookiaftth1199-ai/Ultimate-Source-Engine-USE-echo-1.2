#pragma once
#include <AL/al.h>
#include <AL/alc.h>
#include <string>
#include <vector>

namespace USE {

class AudioEngine {
public:
    AudioEngine();
    ~AudioEngine();

    bool Initialize();
    void Shutdown();

    bool LoadWAV(const std::string& filename, const std::string& name);
    bool PlaySound(const std::string& name, float x = 0, float y = 0, float z = 0);
    void SetListenerPosition(float x, float y, float z);
    void Update();  // for like"اتصال"

private:
    struct Sound {
        ALuint buffer;
        ALuint source;
        std::string name;
    };

    ALCdevice* m_device;
    ALCcontext* m_context;
    std::vector<Sound> m_sounds;

    ALuint FindBuffer(const std::string& name);
};

} // namespace USE