#ifndef HERKULEX_CONST_H
#define HERKULEX_CONST_H

#include "mbed.h"

namespace herkulex {
	namespace constants {
		static const uint8_t header = 0xff;
		static const uint8_t broadcast_id = 0xfe;

		namespace CMD {
			enum class toServo : uint8_t {
				EEPWrite = 0x01,
				EEPRead = 0x02,
				RAMWrite = 0x03,
				RAMRead = 0x04,
				IJOG = 0x05,
				SJOG = 0x06,
				Stat = 0x07,
				Rollback = 0x08,
				Reboot = 0x09
			};

			enum class fromServo : uint8_t {
				EEPWriteAck = 0x41,
				EEPReadAck = 0x42,
				RAMWriteAck = 0x43,
				RAMReadAck = 0x44,
				IJOGAck = 0x45,
				SJOGAck = 0x46,
				StatAck = 0x47,
				RollbackAck = 0x48,
				RebootAck = 0x49
			};
		}

		enum class StatusError : uint8_t {
			OK = 0x00,
			ExceedInputVoltage = 0x01,
			ExceedPotLimit = 0x02,
			ExceedTemperatureLimit = 0x04,
			InvalidPacket = 0x08,
			Overload = 0x10,
			DriverFault = 0x20,
			EEPRegDistort = 0x40
		};

		enum class StatusDetail : uint8_t {
			MovingFlag = 0x01,
			InpositionFlag = 0x02,
			ChecksumError = 0x04,
			UnknownCommand = 0x08,
			ExceedRegRange = 0x10,
			GarbageDetected = 0x20,
			MotorOnFlag = 0x40
		};

		enum class Size : uint8_t {
			MinPacketSize = 7,
			MinAckPacketSize = 9,
			WritePacketSize = 13,
			MaxPacketSize = 223,
			MaxDataSize = MaxPacketSize - MinPacketSize
		};


		enum class JOG_CMD : uint8_t {
			PositionMode = 0x00,
			Stop = 0x01,
			TurnMode = 0x02,
			GreenLedOn = 0x04,
			BlueLedOn = 0x08,
			RedLedOn = 0x10
		};

		enum class Torque_CMD : uint8_t { TorqueFree = 0x00, BreakOn = 0x40, TorqueOn = 0x60 };
		
				enum class EEPAddr : uint8_t {
			ModelNo1 = 0,
			ModelNo2 = 1,
			Version1 = 2,
			Version2 = 3,
			Baudrate = 4,
			Reserved5 = 5,
			ID = 6,
			AckPolicy = 7,
			AlarmLedPolicy = 8,
			Reserved10 = 10,
			MaxTemperature = 11,
			MinVoltage = 12,
			MaxVoltage = 13,
			AccelerationRatio = 14,
			MaxAccelerationTime = 15,
			DeadZone = 16,
			SaturatorOffset = 17,
			SaturatorSlope = 18, // 2 bytes
			PWMOffset = 20,
			MinPWM = 21,
			MaxPWM = 22,                     // 2 bytes
			OverloadPWMThresh = 24,          // 2 bytes
			MinPosition = 26,                // 2 bytes
			MaxPosition = 28,                // 2 bytes
			PositionKp = 30,                 // 2 bytes
			PositionKd = 32,                 // 2 bytes
			PositionKi = 34,                 // 2 bytes
			PositionFeedForward1stGain = 36, // 2 bytes
			PositionFeedForward2ndGain = 38, // 2 bytes
			Reserved40 = 40,                 // 2 bytes
			Reserved42 = 42,                 // 2 bytes
			LedBlinkPeriod = 44,
			ADCFaultCheckPeriod = 45,
			PacketGarbageCheckPeriod = 46,
			StopDetectionPeriod = 47,
			OverloadDetectionPeriod = 48,
			StopThreshold = 49,
			InpositionMargin = 50,
			Reserved51 = 51,
			Reserved52 = 52,
			CalibrationDifference = 53
		};

		enum class RAMAddr : uint8_t {
			ID = 0,
			AckPolicy = 1,
			AlarmLedPolicy = 2,
			TorquePolicy = 3,
			Reserved4 = 4,
			MaxTemperature = 5,
			MinVoltage = 6,
			MaxVoltage = 7,
			AccelerationRatio = 8,
			MaxAcceleration = 9,
			DeadZone = 10,
			SaturatorOffset = 11,
			SaturatorSlope = 12, // 2 bytes
			PWMOffset = 14,
			MinPWM = 15,
			MaxPWM = 16,                     // 2 bytes
			OverloadPWMThresh = 18,          // 2 bytes
			MinPosition = 20,                // 2 bytes
			MaxPosition = 22,                // 2 bytes
			PositionKp = 24,                 // 2 bytes
			PositionKd = 26,                 // 2 bytes
			PositionKi = 28,                 // 2 bytes
			PositionFeedForward1stGain = 30, // 2 bytes
			PositionFeedForward2ndGain = 32, // 2 bytes
			Reserved34 = 34,                 // 2 bytes
			Reserved36 = 36,                 // 2 bytes
			LedBlinkPeriod = 38,             // 2 bytes
			ADCFaultCheckPeriod = 39,
			PacketGarbageCheckPeriod = 40,
			StopDetectionPeriod = 41,
			OverloadDetectionPeriod = 42,
			StopThreshold = 43,
			InpositionMargin = 44,
			Reserved45 = 45,
			Reserved46 = 46,
			CalibrationDifference = 47,
			StatusError = 48,
			StatusDetail = 49,
			Reserved50 = 50,
			Reserved51 = 51,
			TorqueControl = 52,
			LedControl = 53,
			Voltage = 54,
			Temperature = 55,
			CurrentControlMode = 56,
			Tick = 57,
			CalibratedPosition = 58,           // 2 bytes
			AbsolutePosition = 60,             // 2 bytes
			DifferentialPosition = 62,         // 2 bytes
			PWM = 64,                          // 2 bytes
			Reserved66 = 66,                   // 2 bytes
			AbsoluteGoalPosition = 68,         // 2 bytes
			AbsoluteDesiredTrajectoryPos = 70, // 2 bytes
			DesiredVelocity = 72,              // 2 bytes
		};


	}
}


#endif
