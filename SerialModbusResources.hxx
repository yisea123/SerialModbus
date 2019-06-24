#ifndef SerialModbusRESOURCES_H_
#define SerialModbusRESOURCES_H_

// local
#include <Global.h>
#include <PVSSDataPoint.h>
#include <Driver.h>
#include <MBusAbstract.h>
#include <MBusAsciiMaster.h>

// pvss
#include <DrvRsrce.hxx>
#include <ErrHdl.hxx>
#include <Variable.hxx>
#include <IntegerVar.hxx>

class SerialModbusResources : public DrvRsrce
{
  public:
	static const unsigned short int MAX_DPNAME_NUMBER;
	static const unsigned short int DP_NUMBER_OF_ENTRIES;

	static const QString ADDRESS_MODE;
	static const QString ADDRESS_PORT;
	static const QString ADDRESS_BAUDRATE;
	static const QString ADDRESS_DATABITS;
	static const QString ADDRESS_STOPBITS;
	static const QString ADDRESS_PARITY;
	static const QString ADDRESS_POLLING;
	static const QString ADDRESS_BUS;
	static const QString ADDRESS_TIMEOUT;
	static const QString ADDRESS_RETRY;
	static const QString ADDRESS_POLL_DELAY;
	static const QString STATUS_STATUS_1;
	static const QString STATUS_STATUS_2;
	static const QString STATUS_HEARTBEAT;
	static const QString STATUS_BUS;
	static const QString STATUS_BUS_INVALID;

	// constructor
	SerialModbusResources();
	~SerialModbusResources();

	// methods
    static void init(int &argc, char *argv[]); // Initialize statics
    static PVSSboolean readSection();          // read config file

    // Get our static Variables
    // TODO you likely have different things in the config file
    static unsigned short int getTCPPort() { return tcpPort; }

    // Get the number of names we need the DpId for
    virtual int getNumberOfDpNames();
	virtual const CharString& getDpName4Query(int index);
	PVSSboolean allIdsGot();
	PVSSboolean setDpIdentifier(CharString &name, DpIdentifier &dpId);

	int getNumberOfIds2Connect();
	const DpIdentifier & getId2Connect(int);
	InternalDpIdType isInternalDpId(const DpIdentifier &);
	void answer4DpId(const DpIdentifier &, Variable *);

  private:
    // TODO you likely have different things in the config file
    static unsigned short int tcpPort;
	static string loggerPropertyPath;
	QList<PVSSDataPoint*> *dps;

	// private functions
	PVSSDataPoint *findDpById(const DpIdentifier &);
	void parseBus(MBusAbstract*, DynVar*);

};

#endif
