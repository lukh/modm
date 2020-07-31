// coding: utf-8
// ----------------------------------------------------------------------------
/*
 * Copyright (c) 2020, Vivien Henry
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_ADS868x_HPP
#	error "Don't include this file directly! Use 'ads868x.hpp' instead."
#endif

// ----------------------------------------------------------------------------
namespace modm
{

template <typename SpiMaster, typename Cs>
void Ads868x<SpiMaster, Cs>::writeRegister(Register reg, uint32_t data){
	uint32_t cmd;

	Cs::reset();

	// LSB (0-15)
	cmd = (0b11010'00 << 25) | (static_cast<uint8_t>(reg) << 16) | (data & 0xFFFF);
	SpiMaster::transferBlocking((uint8_t)(cmd >> 24));
	SpiMaster::transferBlocking((uint8_t)(cmd >> 16));
	SpiMaster::transferBlocking((uint8_t)(cmd >> 8));
	SpiMaster::transferBlocking((uint8_t)(cmd >> 0));

	// MSB (16-31)
	cmd = (0b11010'00 << 25) | ((static_cast<uint8_t>(reg) + 2) << 16) | (data >> 16);
	SpiMaster::transferBlocking((uint8_t)(cmd >> 24));
	SpiMaster::transferBlocking((uint8_t)(cmd >> 16));
	SpiMaster::transferBlocking((uint8_t)(cmd >> 8));
	SpiMaster::transferBlocking((uint8_t)(cmd >> 0));

	Cs::set();
}

template <typename SpiMaster, typename Cs>
uint32_t Ads868x<SpiMaster, Cs>::readRegister(Register reg){
	uint32_t cmd;
	uint32_t data = 0;

	Cs::reset();

	// MSB (31-16)
	cmd = (0b11001'00 << 25) | ((static_cast<uint8_t>(reg) + 2) << 16);
	SpiMaster::transferBlocking((uint8_t)(cmd >> 24));
	SpiMaster::transferBlocking((uint8_t)(cmd >> 16));
	SpiMaster::transferBlocking((uint8_t)(cmd >> 8));
	SpiMaster::transferBlocking((uint8_t)(cmd >> 0));

	data |= SpiMaster::transferBlocking(0) << 24;
	data |= SpiMaster::transferBlocking(0) << 16;
	SpiMaster::transferBlocking(0);
	SpiMaster::transferBlocking(0);
	

	// LSB (0-15)
	cmd = (0b11001'00 << 25) | (static_cast<uint8_t>(reg) << 16);
	SpiMaster::transferBlocking((uint8_t)(cmd >> 24));
	SpiMaster::transferBlocking((uint8_t)(cmd >> 16));
	SpiMaster::transferBlocking((uint8_t)(cmd >> 8));
	SpiMaster::transferBlocking((uint8_t)(cmd >> 0));

	data |= SpiMaster::transferBlocking(0) << 8;
	data |= SpiMaster::transferBlocking(0);
	SpiMaster::transferBlocking(0);
	SpiMaster::transferBlocking(0);

	Cs::set();

	return data;
}

} // namespace modm
