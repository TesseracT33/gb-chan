#include "Timer.h"

using namespace Util;


void Timer::Initialize()
{
	DIV_bus = bus->ReadIOPointer(Bus::Addr::DIV);
	TAC = bus->ReadIOPointer(Bus::Addr::TAC);
	TIMA = bus->ReadIOPointer(Bus::Addr::TIMA);
	TMA = bus->ReadIOPointer(Bus::Addr::TMA);
}


void Timer::Reset()
{
	SetBit(TAC, 2);
	DIV = prev_TIMA_AND_result = prev_DIV_AND_result = AND_bit_pos_index = 0;
	DIV_enabled = TIMA_enabled = true;
	awaiting_interrupt_request = false;
}


void Timer::Update()
{
	// one iteration per t-cycle
	for (int i = 0; i < 4; i++)
	{
		if (awaiting_interrupt_request && --t_cycles_until_interrupt_request == 0)
		{
			*TIMA = *TMA;
			cpu->RequestInterrupt(CPU::Interrupt::Timer);
			awaiting_interrupt_request = false;
		}

		if (!DIV_enabled) continue;

		DIV++;

		bool AND_result = CheckBit(DIV, AND_bit_pos[AND_bit_pos_index]) && TIMA_enabled;
		if (AND_result == 0 && prev_TIMA_AND_result == 1)
		{
			(*TIMA)++;
			if (*TIMA == 0)
			{
				awaiting_interrupt_request = true;
				t_cycles_until_interrupt_request = 4;
			}
		}
		prev_TIMA_AND_result = AND_result;
	}

	// note: the below cond can only be true if (DIV mod 4 == 0)
	if ((DIV & 0xFF) == 0)
	{
		(*DIV_bus)++;

		// step the APU frame sequencer when bit 5 of upper byte of DIV becomes 1 (bit 6 in double speed mode)
		// this is equivalent to bits 0-4 being cleared (0-5 in double speed mode)
		bool AND_result = *DIV_bus & 0x3F >> (2 - System::speed_mode);
		if (AND_result == 0 && prev_DIV_AND_result == 1 && apu->enabled)
			apu->StepFrameSequencer();
		prev_DIV_AND_result = AND_result;
	}
}


void Timer::SetTIMAFreq(u8 index)
{
	AND_bit_pos_index = index;
}


void Timer::ResetDIV()
{
	DIV = *DIV_bus = 0;
}


void Timer::State(Serialization::BaseFunctor& functor)
{
	functor.fun(&DIV, sizeof(u16));
	functor.fun(&prev_TIMA_AND_result, sizeof(bool));
	functor.fun(&prev_DIV_AND_result, sizeof(bool));
	functor.fun(&TIMA_enabled, sizeof(bool));
	functor.fun(&DIV_enabled, sizeof(bool));
	functor.fun(&awaiting_interrupt_request, sizeof(bool));
	functor.fun(&t_cycles_until_interrupt_request, sizeof(unsigned));
	functor.fun(&AND_bit_pos_index, sizeof(unsigned));
}