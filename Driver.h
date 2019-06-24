#pragma once

// local
#include "MBusAbstract.h"
#include <SerialModbusHWService.hxx>

// qt
#include "QtCore\QMap.h"
#include "QtCore\QQueue.h"
#include <QtCore\QString>

class Driver {
	
public:
	static Driver &getInstance ();

	// operator
	void operator=(Driver const&);

	// property
	SerialModbusHWService* getService();

	// methods
	void addMBus(int, MBusAbstract *);
	MBusAbstract* getMBus(int);

	void registerService(SerialModbusHWService*);
	
	bool hasParameter(const QString& fullName);
	QString extractAddress(const QString& fullName);
	int extractTime(const QString& fullName);

	void manageParamAddress(const QString&, const QString&);
	QString getParamAddress(const QString& address);

	bool applyValueAgainstSmoothing(const QString& address, int time, const PVSSchar& value);

	void stop();

private:
	// attributs
	QMap<int, MBusAbstract*>* mbus;
	QMap<QString, QString>* paramAddress;
	QMap<QString, QQueue<PVSSchar>*>* queues;

	SerialModbusHWService* service;

	// constructor
	Driver();
	Driver(Driver const&);
	~Driver();

};
