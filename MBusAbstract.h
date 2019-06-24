#pragma once

// local
#include <Global.h>

// pvss
#include <Manager.hxx>

// qt
#include <QtCore\QThread>
#include <QtCore\QStringBuilder>

// mbus protocol
#include "MbusAsciiMasterProtocol.hpp"

class Driver;

class MBusAbstract : public QThread {

public:
	static const unsigned short int COIL_NUMBER = 512;
	static const unsigned short int REGISTER_NUMBER = 32;

	// constructor
	MBusAbstract(const QString &);
	~MBusAbstract();

	// properties
	int getAgentID();
	void setAgentID(int);

	// address
	QString getPort();
	void setPort(QString);
	int getBaudRate();
	void setBaudRate(int);
	int getDataBits();
	void setDataBits(int);
	int getStopBits();
	void setStopBits(int);
	int getParity();
	void setParity(int);
	int getPolling();
	void setPolling(int);
	int getTimeout();
	void setTimeout(int);
	int getRetry();
	void setRetry(int);
	int getPollDelay();
	void setPollDelay(int);

	// bus
	void addBus(int);

	// status
	DpIdentifier getStatusId();
	void setStatusId_1(DpIdentifier);
	void setStatusId_2(DpIdentifier);
	void setAgentStatus(int);
	DpIdentifier getHeartbeatId();
	void setHeartbeatId(DpIdentifier);
	void setHeartbeat();
	DpIdentifier getBusStatusId();
	void setBusStatusId(DpIdentifier);
	DpIdentifier getBusStatusInvalidId();
	void setBusStatusInvalidId(DpIdentifier);
	void setBusStatus(QList<int> &status);

	// property
	bool isValid();

	// method
	void sendInvalidCoils(int, int);
	void sendInvalidRegisters(int, int);

	void stop();

protected:
	// member
	bool running;

	// properties
	int agentID;
	QString dpName;

	// address
	QString port;
	int baudRate;
	int dataBits;
	int stopBits;
	int parity;
	int polling;
	int timeout;
	int retry;
	int pollDelay;

	// bus
	QList<int>* bus;

	// status
	DpIdentifier statusId_1;
	DpIdentifier statusId_2;
	DpIdentifier heartbeatId;
	DpIdentifier statusBus;
	DpIdentifier statusBusInvalid;

	// modbus
	MbusSerialMasterProtocol* mbusProtocol;

};