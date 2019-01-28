#include "pch.h"
#include "SoundSystem.h"
#include "IEventManager.h"

SoundSystem::SoundSystem() : System::System(1), m_playMusic(false)
{
	
	// Handle any PlaySound events
	IEventManager::RegisterHandler(EventTypes::Sound_PlayEffect, std::function<void(string)>([=](string name) {
		m_effectBank->Play(name.c_str());
	}),true);
	// Handle any music events
	IEventManager::RegisterHandler(EventTypes::Sound_PlayMusic, std::function<void(void)>([=]() {
		m_playMusic = true;
	}), true);
	IEventManager::RegisterHandler(EventTypes::Sound_StopMusic, std::function<void(void)>([=]() {
		m_playMusic = false;
	}), true);
	// Handle application level events
	IEventManager::RegisterHandler(EventTypes::Game_Pause, std::function<void(void)>([=]() {
		m_audEngine->Suspend();
	}));
	IEventManager::RegisterHandler(EventTypes::Game_Resume, std::function<void(void)>([=]() {
		m_audEngine->Resume();
	}));

	AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
	eflags = eflags | AudioEngine_Debug;
#endif
	m_audEngine = std::make_unique<AudioEngine>(eflags);

	//----------------------------------------------------------------
	// Initialize wave banks
	m_musicBank = std::make_unique<WaveBank>(m_audEngine.get(),
		L"Assets/music.xwb");
	m_effectBank = std::make_unique<WaveBank>(m_audEngine.get(),
		L"Assets/effect.xwb");
	
}

SoundSystem::~SoundSystem()
{
	if (m_audEngine) {
		m_audEngine->Suspend();
	}
}

string SoundSystem::Name()
{
	return "Sound";
}

void SoundSystem::Update(double & elapsed)
{
	m_audEngine->Update();
	UpdateMusic();
}

void SoundSystem::UpdateMusic()
{
	if (m_playMusic) {
		if (!m_currentSong || m_currentSong->GetState() == DirectX::SoundState::STOPPED) {
			string name = "Piano" + to_string(Utility::randWithin(0, 12));
			m_currentSong = m_musicBank->CreateInstance(name.c_str());
			m_currentSong->Play();
			IEventManager::Invoke(EventTypes::GUI_ShowHint, "Playing: " + name);
		}
	}
	else if (m_currentSong && m_currentSong->GetState() == DirectX::SoundState::PLAYING) {
		m_currentSong->Stop();
	}
}

