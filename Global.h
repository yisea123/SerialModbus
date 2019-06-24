#pragma once

// local
#include "Logger.h"

// pvss
#include <CharString.hxx>
#include <PVSSMacros.hxx>

// QT
#include <QtCore\QString>
#include <QtCore\QList>
#include <QtCore\QDateTime>

#if !defined(ARRAY_SIZE)
    #define ARRAY_SIZE(x) (sizeof((x)) / sizeof((x)[0]))
#endif

class Global {

public:
	static const int V_MAJOR = 0;
	static const int V_MINOR = 9;
};

enum DriverTypes {
	COILS = 1701,
	REGISTERS = 1702
};

namespace DPUsage {
	enum {
		ADDRESS_PORT,
		ADDRESS_MODE,
		ADDRESS_BAUDRATE,
		ADDRESS_DATABITS,
		ADDRESS_STOPBITS,
		ADDRESS_PARITY,
		ADDRESS_POLLING,
		ADDRESS_BUS,
		ADDRESS_TIMEOUT,
		ADDRESS_RETRY,
		ADDRESS_POLL_DELAY,
		STATUS_STATUS_1,
		STATUS_STATUS_2,
		STATUS_HEARTBEAT,
		STATUS_BUS,
		STATUS_BUS_INVALID
	};
};

enum Status {
	STOPPED = 0,
	INITIALIZE = 1,
	CONNECTED = 2,
	STANDBY = 3,
	DISCONNECTED = 4,
	CONNECTED_NO_HEARTBEAT = 5,
	DEGRADED = 6
};