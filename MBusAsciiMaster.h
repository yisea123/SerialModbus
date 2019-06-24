#pragma once

// local
#include "MBusAbstract.h"

// mbus protocol
#include "MbusAsciiMasterProtocol.hpp"

// pvss

// qt
class MBusAsciiMaster : public MBusAbstract {

public:
	static const qint64 HEARTBEAT_MINIMUM_REWRITE = 500;

	MBusAsciiMaster(const QString &);
	~MBusAsciiMaster();
	
	// modbus
	bool tryToConnect();
	void tryToDisconnect();

	// thread
	void run();
	void poll();

	// toString
	QString toString();

};
