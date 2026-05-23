#include "Audio/AudioEngine.h"
#include <iostream>
#include <dr_wav.h>

namespace USE {

AudioEngine::AudioEngine() : m_device(nullptr), m_context(nullptr) {}

AudioEngine::~AudioEngine() {
    Shutdown();
}

bool AudioEngine::Initialize() {
    // 1. فتح جهاز الصوت الافتراضي
    m_device = alcOpenDevice(nullptr);
    if (!m_device) {
        std::cerr << "فشل فتح جهاز الصوت!" << std::endl;
        return false;
    }

    // 2. إنشاء سياق الصوت
    m_context = alcCreateContext(m_device, nullptr);
    if (!m_context) {
        std::cerr << "فشل إنشاء سياق الصوت!" << std::endl;
        alcCloseDevice(m_device);
        return false;
    }

    // 3. تفعيل السياق
    if (!alcMakeContextCurrent(m_context)) {
        std::cerr << "فشل تفعيل سياق الصوت!" << std::endl;
        alcDestroyContext(m_context);
        alcCloseDevice(m_device);
        return false;
    }

    // 4. التحقق من الإصدار
    std::cout << "OpenAL initialized: " << alGetString(AL_VERSION) << std::endl;
    
    // 5. إعداد المستمع (Listener) في المنتصف
    alListener3f(AL_POSITION, 0, 0, 0);
    alListener3f(AL_VELOCITY, 0, 0, 0);
    
    return true;
}

void AudioEngine::Shutdown() {
    // تفريغ كل الأصوات
    for (auto& sound : m_sounds) {
        alDeleteSources(1, &sound.source);
        alDeleteBuffers(1, &sound.buffer);
    }
    m_sounds.clear();

    // تنظيف OpenAL
    if (m_context) {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(m_context);
        m_context = nullptr;
    }
    if (m_device) {
        alcCloseDevice(m_device);
        m_device = nullptr;
    }
}

bool AudioEngine::LoadWAV(const std::string& filename, const std::string& name) {
    // تحميل ملف WAV باستخدام dr_wav
    drwav wav;
    if (!drwav_init_file(&wav, filename.c_str(), nullptr)) {
        std::cerr << "فشل فتح ملف WAV: " << filename << std::endl;
        return false;
    }

    // قراءة كل بيانات الصوت
    std::vector<ALshort> samples(wav.totalPCMFrameCount * wav.channels);
    drwav_read_pcm_frames_s16(&wav, wav.totalPCMFrameCount, samples.data());

    // تحديد تنسيق OpenAL حسب عدد القنوات
    ALenum format;
    if (wav.channels == 1) format = AL_FORMAT_MONO16;
    else if (wav.channels == 2) format = AL_FORMAT_STEREO16;
    else {
        std::cerr << "عدد قنوات غير مدعوم: " << wav.channels << std::endl;
        drwav_uninit(&wav);
        return false;
    }

    // إنشاء Buffer في OpenAL
    ALuint buffer;
    alGenBuffers(1, &buffer);
    alBufferData(buffer, format, samples.data(), 
                 static_cast<ALsizei>(samples.size() * sizeof(ALshort)),
                 wav.sampleRate);

    // إنشاء Source
    ALuint source;
    alGenSources(1, &source);
    alSourcei(source, AL_BUFFER, buffer);
    
    // إعدادات افتراضية
    alSourcef(source, AL_GAIN, 1.0f);
    alSourcef(source, AL_PITCH, 1.0f);
    alSourcei(source, AL_LOOPING, AL_FALSE);

    // حفظ الصوت
    Sound sound;
    sound.buffer = buffer;
    sound.source = source;
    sound.name = name;
    m_sounds.push_back(sound);

    drwav_uninit(&wav);
    std::cout << "تم تحميل الصوت: " << filename << " (" << wav.sampleRate << "Hz)" << std::endl;
    
    return true;
}

bool AudioEngine::PlaySound(const std::string& name, float x, float y, float z) {
    ALuint buffer = FindBuffer(name);
    if (buffer == 0) {
        std::cerr << "الصوت غير موجود: " << name << std::endl;
        return false;
    }

    // البحث عن مصدر غير مستخدم أو إنشاء واحد
    for (auto& sound : m_sounds) {
        if (sound.name == name) {
            ALint state;
            alGetSourcei(sound.source, AL_SOURCE_STATE, &state);
            
            if (state != AL_PLAYING) {
                // تعيين الموضع ثلاثي الأبعاد
                alSource3f(sound.source, AL_POSITION, x, y, z);
                alSourcePlay(sound.source);
                return true;
            }
        }
    }
    
    return false;
}

void AudioEngine::SetListenerPosition(float x, float y, float z) {
    alListener3f(AL_POSITION, x, y, z);
}

void AudioEngine::Update() {
    // يمكن إضافة منطق هنا لإدارة الأصوات (مثل إيقاف الأصوات المنتهية)
}

ALuint AudioEngine::FindBuffer(const std::string& name) {
    for (const auto& sound : m_sounds) {
        if (sound.name == name) return sound.buffer;
    }
    return 0;
}

} // namespace USE
//sorry I am lazy to write it in english ;)