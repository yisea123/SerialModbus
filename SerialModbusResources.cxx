// Our Resource class.
// This class will interpret the command line and read the config file

#include "SerialModbusResources.hxx"

// TODO you likely have different things in the config file
unsigned short int SerialModbusResources::tcpPort = 12402;
string SerialModbusResources::loggerPropertyPath = "log.properties";

const unsigned short int SerialModbusResources::MAX_DPNAME_NUMBER = 100;
const unsigned short int SerialModbusResources::DP_NUMBER_OF_ENTRIES = 16;

// config
const QString SerialModbusResources::ADDRESS_MODE = "_SerialModbus_%d.Address.Mode:_online.._value";
const QString SerialModbusResources::ADDRESS_PORT = "_SerialModbus_%d.Address.Port:_online.._value";
const QString SerialModbusResources::ADDRESS_BAUDRATE = "_SerialModbus_%d.Address.BaudRate:_online.._value";
const QString SerialModbusResources::ADDRESS_DATABITS = "_SerialModbus_%d.Address.DataBits:_online.._value";
const QString SerialModbusResources::ADDRESS_STOPBITS = "_SerialModbus_%d.Address.StopBits:_online.._value";
const QString SerialModbusResources::ADDRESS_PARITY = "_SerialModbus_%d.Address.Parity:_online.._value";
const QString SerialModbusResources::ADDRESS_POLLING = "_SerialModbus_%d.Address.Polling:_online.._value";
const QString SerialModbusResources::ADDRESS_BUS = "_SerialModbus_%d.Address.Bus:_online.._value";
const QString SerialModbusResources::ADDRESS_TIMEOUT = "_SerialModbus_%d.Address.Timeout:_online.._value";
const QString SerialModbusResources::ADDRESS_RETRY = "_SerialModbus_%d.Address.Retry:_online.._value";
const QString SerialModbusResources::ADDRESS_POLL_DELAY = "_SerialModbus_%d.Address.PollDelay:_online.._value";

// status
const QString SerialModbusResources::STATUS_STATUS_1 = "_SerialModbus_%d.Status.Status_1:_original.._value";
const QString SerialModbusResources::STATUS_STATUS_2 = "_SerialModbus_%d.Status.Status_2:_original.._value";
const QString SerialModbusResources::STATUS_HEARTBEAT = "_SerialModbus_%d.Status.Heartbeat:_original.._value";
const QString SerialModbusResources::STATUS_BUS = "_SerialModbus_%d.Status.Bus:_original.._value";
const QString SerialModbusResources::STATUS_BUS_INVALID = "_SerialModbus_%d.Status.Bus:_original.._userbit1";

SerialModbusResources::SerialModbusResources() {
	dps = new QList<PVSSDataPoint*>();
}

SerialModbusResources::~SerialModbusResources() {
	if (!dps->isEmpty()) {
		qDeleteAll(*dps);
		dps->clear();
	}
	if (dps) {
		delete dps;
	}
}
//-------------------------------------------------------------------------------
// init is a wrapper around begin, readSection and end

void SerialModbusResources::init(int &argc, char *argv[])
{
  // Prepass of commandline arguments, e.g. get the arguments needed to
  // find the config file.
  begin(argc, argv);

	DEBUG_DRV_USR1("SerialModbus driver initialization !");

  // Read the config file
  while ( readSection() || generalSection() ) ;

  // Postpass of the commandline arguments, e.g. get the arguments that
  // will override entries in the config file
  end(argc, argv);
}

//-------------------------------------------------------------------------------

PVSSboolean SerialModbusResources::readSection()
{
	// Are we in our section ?
	if ( !isSection("SerialModbus") ) return PVSS_FALSE;

	// skip "[SerialModbus]"
	getNextEntry();

	// Now read the section until new section or end of file
	while ( (cfgState != CFG_SECT_START) && (cfgState != CFG_EOF) )	{
		// TODO whatever you have to read from the config file
		if      ( keyWord.icmp("tcpPort") == 0 ) cfgStream >> tcpPort;
		else if ( keyWord.icmp("propertyPath") == 0 ) cfgStream >> loggerPropertyPath;
		else if ( !commonKeyWord() ) {
		  ErrHdl::error(ErrClass::PRIO_WARNING,    // Not that bad
						ErrClass::ERR_PARAM,       // Error is with config file, not with driver
						ErrClass::ILLEGAL_KEYWORD, // Illegal keyword in res.
						keyWord);

		// signal Error, so we stop later
		cfgError = PVSS_TRUE;
		}

		// get next entry
		getNextEntry();
	}

	QString dir = QString::fromStdString(loggerPropertyPath.c_str())
		.replace("\"", "")
		.replace("%configPath%", getConfigDir().c_str());
	Logger::getInstance().setPath(dir);

  // So the loop will stop at the end of the file
  return cfgState != CFG_EOF;
}

//-------------------------------------------------------------------------------
// Interface to internal Datapoints
// Get the number of names we need the DpId for

int SerialModbusResources::getNumberOfDpNames()
{
	// TODO if you use internal DPs
	return SerialModbusResources::MAX_DPNAME_NUMBER * SerialModbusResources::DP_NUMBER_OF_ENTRIES;
}

const CharString& SerialModbusResources::getDpName4Query(int index) {

	// create dpname from format
	QString dpName;
	const QString *format;
	int pos = index / DP_NUMBER_OF_ENTRIES;
	int entry = index % DP_NUMBER_OF_ENTRIES;
	int usage; 

	// config
	if (entry == 0) {
		format = &ADDRESS_MODE;
		usage = DPUsage::ADDRESS_MODE;
	} else if (entry == 1) {
		format = &ADDRESS_PORT;
		usage = DPUsage::ADDRESS_PORT;
	} else if (entry == 2) {
		format = &ADDRESS_BAUDRATE;
		usage = DPUsage::ADDRESS_BAUDRATE;
	} else if (entry == 3) {
		format = &ADDRESS_DATABITS;
		usage = DPUsage::ADDRESS_DATABITS;
	} else if (entry == 4) {
		format = &ADDRESS_STOPBITS;
		usage = DPUsage::ADDRESS_STOPBITS;
	} else if (entry == 5) {
		format = &ADDRESS_PARITY;
		usage = DPUsage::ADDRESS_PARITY;
	} else if (entry == 6) {
		format = &ADDRESS_POLLING;
		usage = DPUsage::ADDRESS_POLLING;
	} else if (entry == 7) {
		format = &ADDRESS_BUS;
		usage = DPUsage::ADDRESS_BUS;
	} else if (entry == 8) {
		format = &ADDRESS_TIMEOUT;
		usage = DPUsage::ADDRESS_TIMEOUT;
	} else if (entry == 9) {
		format = &ADDRESS_RETRY;
		usage = DPUsage::ADDRESS_RETRY;
	} else if (entry == 10) {
		format = &ADDRESS_POLL_DELAY;
		usage = DPUsage::ADDRESS_POLL_DELAY;

	// status
	} else if (entry == 11) {
		format = &STATUS_STATUS_1;
		usage = DPUsage::STATUS_STATUS_1;
	} else if (entry == 12) {
		format = &STATUS_STATUS_2;
		usage = DPUsage::STATUS_STATUS_2;
	} else if (entry == 13) {
		format = &STATUS_HEARTBEAT;
		usage = DPUsage::STATUS_HEARTBEAT;
	} else if (entry == 14) {
		format = &STATUS_BUS;
		usage = DPUsage::STATUS_BUS;
	} else if (entry == 15) {
		format = &STATUS_BUS_INVALID;
		usage = DPUsage::STATUS_BUS_INVALID;
	}

	dpName.sprintf(format->toAscii().data(), (pos + 1));
	dps->append(new PVSSDataPoint(pos, usage, dpName));

	CharString* ret = new CharString(dpName.toAscii().data());
	return *ret;
}

//-------------------------------------------------------------------------------
int identifier = 0;
int count = 0;

PVSSboolean SerialModbusResources::setDpIdentifier(CharString &name, DpIdentifier &dpId) {

	if (dpId.isNull()) {
		dps->removeAt(identifier);
	} else {
		dps->at(identifier)->setDpId(dpId);
		
		DEBUG_DRV_USR2("setDpIdIdentifier " << name << " id " << dpId);
		identifier++;
	}
	count++;

	return PVSS_TRUE;
}

PVSSboolean SerialModbusResources::allIdsGot() {
	bool areAll = (count == this->getNumberOfDpNames());

	if (areAll) {
		DEBUG_DRV_USR2("load " << dps->length() << " datapoint(s) !");
	}

	return true;
}


int SerialModbusResources::getNumberOfIds2Connect() {
	return dps->length();
}

const DpIdentifier & SerialModbusResources::getId2Connect(int i) {
	DpIdentifier dpid = dps->at(i)->getDpId();
	DpIdentifier* ident = new DpIdentifier(dpid);
	return *ident;
}

PVSSDataPoint *SerialModbusResources::findDpById(const DpIdentifier &dpId) {
	foreach (PVSSDataPoint *dp, *dps) {
		if (dp->getDpId() == dpId) {
			return dp;
		}
	}
	return NULL;
}

InternalDpIdType SerialModbusResources::isInternalDpId(const DpIdentifier &dpId) {
	if (this->findDpById(dpId) != NULL) {
		return INTERNAL_DPID;
	} else {
		return NOT_INTERNAL_DPID;
	}
	return NOT_INTERNAL_DPID;
}

void SerialModbusResources::parseBus(MBusAbstract* mb, DynVar* bus) {
	Variable* varPtr;

	while ((varPtr = bus->getNext()) != 0) {
		int bus = (((IntegerVar*)varPtr)->getValue());
		mb->addBus(bus);
	}
}

static const int TIMEOUT_MIN = 1;
static const int TIMEOUT_MAX = 100000;
static const int TIMEOUT_DEFAULT = 1000;

static const int RETRY_MIN = 0;
static const int RETRY_MAX = 10;

static const int POLL_DELAY_MIN = 0;
static const int POLL_DELAY_MAX = 100000;

int frameValue(int value, int min, int max, int defaultValue) {
	if (value < min) return defaultValue;
	if (value > max) return max;
	return value;
}

void SerialModbusResources::answer4DpId(const DpIdentifier &dpId, Variable *varPtr) {
	if (INTERNAL_DPID == isInternalDpId (dpId)) {
		PVSSDataPoint *dp = this->findDpById(dpId);

		MBusAbstract *mb = Driver::getInstance().getMBus(dp->getNumber());

		// create server
		MBusAsciiMaster *master = (MBusAsciiMaster *)mb;
		if (master == NULL) {
			QString dpName = dp->getDpName().left(dp->getDpName().indexOf("."));

			master = new MBusAsciiMaster(dpName);
			master->setAgentID(dp->getNumber() + 1);
			Driver::getInstance().addMBus(dp->getNumber(), master);
		}

		switch (dp->getUsage()) {
			// config
			case DPUsage::ADDRESS_PORT:
				master->setPort(QString(((TextVar*)varPtr)->getValue()));
				break;
			case DPUsage::ADDRESS_BAUDRATE:
				master->setBaudRate(((IntegerVar*)varPtr)->getValue());
				break;
			case DPUsage::ADDRESS_DATABITS:
				master->setDataBits(((IntegerVar*)varPtr)->getValue());
				break;
			case DPUsage::ADDRESS_STOPBITS:
				master->setStopBits(((IntegerVar*)varPtr)->getValue());
				break;
			case DPUsage::ADDRESS_PARITY:
				master->setParity(((IntegerVar*)varPtr)->getValue());
				break;
			case DPUsage::ADDRESS_POLLING:
				master->setPolling(((IntegerVar*)varPtr)->getValue());
				break;
			case DPUsage::ADDRESS_BUS:
				parseBus(mb, (DynVar*)varPtr);
				break;
			case DPUsage::ADDRESS_TIMEOUT:
				master->setTimeout(
					frameValue(((IntegerVar*)varPtr)->getValue(), TIMEOUT_MIN, TIMEOUT_MAX, TIMEOUT_DEFAULT)
				);
				break;
			case DPUsage::ADDRESS_RETRY:
				master->setRetry(
					frameValue(((IntegerVar*)varPtr)->getValue(), RETRY_MIN, RETRY_MAX, RETRY_MIN)
				);
				break;
			case DPUsage::ADDRESS_POLL_DELAY:
				master->setPollDelay(
					frameValue(((IntegerVar*)varPtr)->getValue(), POLL_DELAY_MIN, POLL_DELAY_MAX, POLL_DELAY_MIN)
				);
				break;

			// status
			case DPUsage::STATUS_STATUS_1:
				master->setStatusId_1(dpId);
				break;
			case DPUsage::STATUS_STATUS_2:
				master->setStatusId_2(dpId);
				break;
			case DPUsage::STATUS_HEARTBEAT:
				master->setHeartbeatId(dpId);
				break;
			case DPUsage::STATUS_BUS:
				master->setBusStatusId(dpId);
				break;
			case DPUsage::STATUS_BUS_INVALID:
				master->setBusStatusInvalidId(dpId);
				master->start();
				break;
		}
	}

	delete varPtr;
}
