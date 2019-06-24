#ifndef SerialModbusHWSERVICE_H_
#define SerialModbusHWSERVICE_H_

// local
#include "Global.h"

// pvss
#include <HWService.hxx>
#include <Variable.hxx>

// Qt
#include <QtCore\QQueue>
#include <QtCore\QMutex>
#include <QtCore\QStringList>

struct Message {
	QString address;
	DriverTypes type;
	QString value;
	bool invalid;
};

struct DpValue {
	DpIdentifier dpId;
	Variable* value;
};

class SerialModbusHWService : public HWService
{
  public:
	static const char* DATA_FORMAT;

	// constructor
	SerialModbusHWService();
	~SerialModbusHWService();
	
	// abstract methods
	virtual PVSSboolean initialize(int argc, char *argv[]);
    virtual PVSSboolean start();
    virtual void stop();
    virtual void workProc();
    virtual PVSSboolean writeData(HWObject *objPtr);

	void setStatus(int, int, int, DriverTypes, QString, bool);
	void setDpValue(DpIdentifier, Variable*);

private:
	HWObject* storeToDB(QString, PVSSchar*, bool);

};

#endif
