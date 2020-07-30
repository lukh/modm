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
#define MODM_ADS868x_HPP

#include <modm/architecture/interface/spi_device.hpp>
#include <modm/architecture/interface/accessor.hpp>
#include <modm/architecture/interface/register.hpp>
#include <modm/architecture/interface/gpio.hpp>
#include <modm/architecture/interface/delay.hpp>
#include <modm/processing/resumable.hpp>
#include <modm/io/iostream.hpp>

namespace modm
{

/// @ingroup modm_driver_ads868x
struct ads868x
{
public:
	/// Device ID register
	enum class
	DeviceIDRegister : uint32_t
	{
		Addr0 = Bit16,
		Addr1 = Bit17,
		Addr2 = Bit18,
		Addr3 = Bit19
	};

	MODM_FLAGS32(DeviceIDRegister);

	typedef modm::Value<DeviceIDRegister_t, 4, 16> DeviceID_t;


	// Reset and Power Control register
	enum class
	ResetPowerControlRegister: uint32_t
	{
		WKey0 = Bit8,
		WKey1 = Bit9,
		WKey2 = Bit10,
		WKey3 = Bit11,
		WKey4 = Bit12,
		WKey5 = Bit13,
		WKey6 = Bit14,
		WKey7 = Bit15,

		VDDAlarmDisable = Bit5,

		InputAlarmDisable = Bit4,
		RSTn_ApplicationReset = Bit2,
		NapModeEnable = Bit1,
		PowerDownEnable = Bit0
	};
	MODM_FLAGS32(ResetPowerControlRegister);

	typedef modm::Value<ResetPowerControlRegister_t, 8, 8> WriteKey_t;

	// SDI Data Input Control register
	enum class
	SDIControlRegister: uint32_t
	{
		SDIMode0 = Bit0,
		SDIMode1 = Bit1
	};
	MODM_FLAGS32(SDIControlRegister);

	enum class 
	SDIMode : uint8_t
	{
		Std_Pol0_Phase0 = 0,
		Std_Pol0_Phase1 = 1,
		Std_Pol1_Phase0 = 2,
		Std_Pol1_Phase1 = 3
	};
	typedef modm::Configuration<SDIControlRegister_t, SDIMode, 0b11, 0> SDIMode_t;

	// SDO Data Output Control Register
	enum class
	SDOControlRegister: uint32_t
	{
		SDOMode0 = Bit0,
		SDOMode1 = Bit1,

		SSyncClock = Bit6,

		SDO1Config0 = Bit8,
		SDO1Config1 = Bit9,

		GPOValue = Bit12
	};
	MODM_FLAGS32(SDOControlRegister);

	enum class
	SDOMode: uint8_t
	{
		SameAsSDI0 = 0b00,
		SameAsSDI0 = 0b01,
		Invalid = 0b10,
		ADCMasterClk_SourcSync = 0b11
	};
	typedef modm::Configuration<SDOControlRegister_t, SDOMode, 0b11, 0> SDOMode_t;


	enum class
	SourceSyncClock: uint8_t
	{
		External = 0,
		Internal = 1
	};
	typedef modm::Configuration<SDOControlRegister_t, SourceSyncClock, 0b1, 6> SourceSyncClock_t;

	enum class
	SDO1Config: uint8_t
	{
		SDO1_Tristated = 0b00,
		SDO1_Alarm = 0b01,
		SDO1_GPO = 0b10,
		SDO1_2BitsSDO = 0b11
	};
	typedef modm::Configuration<SDOControlRegister_t, SDO1Config, 0b1, 8> SourceSyncClock_t;


	enum class
	DataOutControlRegister: uint32_t
	{
		DataVal0 = Bit0,
		DataVal1 = Bit1,
		DataVal2 = Bit2,

		ParityEnable = Bit3

		Inc_Range = Bit8,

		Inc_InActiveAlarm_High = Bit10,
		Inc_InActiveAlarm1_Low = Bit11,

		Inc_VDDActiveAlarm0_High = Bit12,
		Inc_VDDActiveAlarm1_Low = Bit13,

		Inc_DeviceAddr = Bit14
	};
	MODM_FLAGS32(DataOutControlRegister);

	enum class
	DataValue: uint8_t
	{
		ConversionData = 0b000,
		All0 = 0b100,
		All1 = 0b101,
		Seq01 = 0b110,
		Seq0011 = 0b111
	};
	typedef modm::Configuration<DataOutControlRegister_t, DataValue, 0b111, 0> DataValue_t;


	enum class
	RangeSelectionRegister: uint32_t
	{
		RangeSel0 = Bit0,
		RangeSel1 = Bit1,
		RangeSel2 = Bit2,
		RangeSel3 = Bit3,

		InternalRefDisabled = Bit6
	};
	MODM_FLAGS32(RangeSelectionRegister);

	enum class
	RangeSel: uint8_t
	{
		Range_Bipolar_3_000_VRef = 0b0000,
		Range_Bipolar_2_500_VRef = 0b0001,
		Range_Bipolar_1_500_VRef = 0b0010,
		Range_Bipolar_1_250_VRef = 0b0011,
		Range_Bipolar_0_625_VRef = 0b0100,
		Range_Unipolar_3_00_VRef = 0b1000,
		Range_Unipolar_2_50_VRef = 0b1001,
		Range_Unipolar_1_50_VRef = 0b1010,
		Range_Unipolar_1_25_VRef = 0b1011
	};
	typedef modm::Configuration<RangeSelectionRegister_t, RangeSel, 0b1111, 0> RangeSel_t;



	// struct modm_packed
	// Data
	// {
	// 	/// @return 12 bit result, for AD7918 and AD7908, respectively, 4 or 2 LSB are 0
	// 	inline uint16_t
	// 	value() const
	// 	{
	// 		return (data[1] & 0xFF) | ((static_cast<uint16_t>(data[0] & 0b1111)) << 8);
	// 	}

	// 	/// @return adc input channel
	// 	inline InputChannel
	// 	channel() const
	// 	{
	// 		return static_cast<InputChannel>((data[0] & 0b01110000) >> 4);
	// 	}

	// 	uint8_t data[2];
	// };
}; // struct ads868x

/**
 * @tparam	SpiMaster	SpiMaster interface
 * @tparam	Cs			Chip-select pin
 *
 * @author	Vivien Henry
 * @ingroup modm_driver_ads868x
 */
template <typename SpiMaster, typename Cs>
class Ads868x : public ads868x, public modm::SpiDevice<SpiMaster>, protected modm::NestedResumable<3>
{
public:
	Ads868x();

	/// Call this function once before using the device
	modm::ResumableResult<void>
	initialize();

	/// Initiate a single conversion and return the result of the previous conversion
	/// A running sequence will be aborted.
	/// If the device is in full shutdown, it will be woken up.
	modm::ResumableResult<Data>
	singleConversion(InputChannel channel);

	/// Start a conversion sequence.
	/// The device will automatically cycle through the specified channels, starting
	/// with the lowest channel index in sequence1, when nextSequenceConversion() is called.
	modm::ResumableResult<void>
	startSequence(SequenceChannels_t channels1, SequenceChannels_t channels2 = SequenceChannels_t(0));

	/// Perform the next sequence conversion
	/// The result is undefined if the device is not in sequence mode or not in normal power mode.
	modm::ResumableResult<Data>
	nextSequenceConversion();

	/// Enable extended range mode (0V < input < 2*Vref)
	/// The configuration will be applied after the next conversion
	/// Default mode: (0V < input < Vref)
	void
	setExtendedRange(bool enabled);

	/// Test if extended range mode is enabled
	bool
	isExtendedRange();

	/// Shutdown device after each conversion, not supported in sequence mode
	void
	setAutoShutdownEnabled(bool enabled);

	/// Test if auto-shutdown is enabled
	bool
	isAutoShutdownEnabled();

	/// Shutdown device
	/// Calling wakeup() or initiating a conversion will wake up the device
	modm::ResumableResult<void>
	fullShutdown();

	/// Wake up the device from full shutdown mode
	modm::ResumableResult<void>
	wakeup();

private:
	modm::ResumableResult<void>
	transfer(Register_t reg);

	ControlRegister_t config;
	ShadowRegister_t sequenceChannels;

	uint8_t outBuffer[2];
	Data data;

	PowerMode currentPowerMode;
};

IOStream&
operator<<(IOStream& out, const ad7928::Data& data);

} // namespace modm

#include "ad7928_impl.hpp"

#endif // MODM_ADS868x_HPP
