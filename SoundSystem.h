#pragma once
#include "System.h"
class SoundSystem :
	public System
{
public:
	SoundSystem();
	~SoundSystem();
	// Inherited via System
	virtual string Name() override;
	virtual void Update(double & elapsed) override;
public:
	//----------------------------------------------------------------
	// Public interface

private:
	std::unique_ptr<DirectX::AudioEngine> m_audEngine;

	
	std::unique_ptr<WaveBank> m_effectBank;

	//----------------------------------------------------------------
	// Music
	bool m_playMusic;
	std::unique_ptr<SoundEffectInstance> m_currentSong;
	std::unique_ptr<WaveBank> m_musicBank;

	void UpdateMusic();
};

