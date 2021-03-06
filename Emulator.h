#pragma once

#include "SDL.h"
#include <wx/filename.h>
#include <wx/msgdlg.h>
#include <wx/stdpaths.h>

#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "BusImpl.h"
#include "Configurable.h"
#include "Observer.h"

class Emulator final : public Snapshottable, public Configurable
{
public:
	Emulator();
	~Emulator();

	void State(Serialization::BaseFunctor& functor) override;
	void Configure(Serialization::BaseFunctor& functor) override;
	void SetDefaultConfig() override;

	void ConnectSystemComponents();
	void BuildComponentVector();
	void StartGame(wxString rom_path);
	void MainLoop();
	void Pause();
	void Reset();
	void Resume();
	void Stop();
	void LoadState();
	void SaveState();

	bool SetupSDLVideo(const void* window_handle);

	void SetWindowSize(unsigned width, unsigned height);
	void SetWindowSize(wxSize size);
	void SetColour(int id, SDL_Color colour);
	void SetColour(int id, wxColour colour);
	void SetEmulationSpeed(unsigned speed);

	wxSize GetWindowSize();

	bool emu_is_paused = false;
	bool emu_is_running = false;
	bool load_boot_rom = true;
	bool render_graphics_every_system_frame = true;
	bool emulation_speed_uncapped = false;
	unsigned emulation_speed = 100; // emulation speed in %.
	unsigned target_fps = 60; // todo: make this assignable by user

	APU apu;
	BusImpl bus;
	Cartridge cartridge;
	CPU cpu;
	DMA dma;
	Joypad joypad;
	PPU ppu;
	Serial serial;
	Timer timer;

	Observer* gui = nullptr; // used for setting the current fps in the window title. Observer : GUI : wxFrame

	std::string dmg_boot_path = "./DMG_boot.gb", cgb_boot_path = "./CGB_boot.gbc";

private:
	const unsigned microseconds_per_frame_DMG = double(System::m_cycles_per_frame_DMG) / double(System::m_cycles_per_sec_DMG) * 1E+6; // 16742
	unsigned microseconds_per_frame = microseconds_per_frame_DMG; // not affected by 'speed_mode', only by 'emulation_speed'

	bool save_state_on_next_cycle = false;
	bool load_state_on_next_cycle = false;

	std::string rom_path;
	std::string save_state_path = wxFileName(wxStandardPaths::Get().GetExecutablePath())
		.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) + "state.bin";

	std::vector<Snapshottable*> snapshottable_components{};
};