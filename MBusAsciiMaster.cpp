#include "MBusAsciiMaster.h"
#include <SerialModbusHWService.hxx>
#include "driver.h"

using namespace std;

// constructor
MBusAsciiMaster::MBusAsciiMaster(const QString &dpName):MBusAbstract(dpName) {
	this->mbusProtocol = new MbusAsciiMasterProtocol();
}

MBusAsciiMaster::~MBusAsciiMaster() {
	if (mbusProtocol) {
		tryToDisconnect();
		delete ((MbusAsciiMasterProtocol*)mbusProtocol);
	}
}

// modbus
bool MBusAsciiMaster::tryToConnect() {
	if (mbusProtocol && !mbusProtocol->isOpen()) {
		// configure
		mbusProtocol->setTimeout(this->getTimeout());
		mbusProtocol->setRetryCnt(this->getRetry());
		mbusProtocol->setPollDelay(this->getPollDelay());
		Logger::getInstance().debug(QString("Port configured %1 with [timeout:%2, retry:%3, pollDelay:%4]").arg(this->port).arg(QString::number(this->getTimeout())).arg(QString::number(this->getRetry())).arg(QString::number(this->getPollDelay())));

		int result = mbusProtocol->openProtocol(this->port.toAscii().data(), this->baudRate, this->dataBits, this->stopBits, this->parity);
		if (result != FTALK_SUCCESS) {
			// init status
			setAgentStatus(Status::DISCONNECTED);
			Logger::getInstance().error(QString("Error occured [dp:%1, port:%2] during connection [%3]! Com port should not be correctly configured.").arg(this->dpName).arg(this->port).arg(QString(getBusProtocolErrorText(result))));
			return false;
		} else {
			Logger::getInstance().debug(QString("Port %1 connected").arg(this->port));
		}
		return true;
	}
	//return false;
}

void MBusAsciiMaster::tryToDisconnect() {
	if (mbusProtocol && mbusProtocol->isOpen()) {
		// close
		mbusProtocol->closeProtocol();
		Logger::getInstance().debug(QString("Port %1 closed").arg(this->port));
	}
}

// thread
void MBusAsciiMaster::run() {

	Logger::getInstance().debug(QString("Starting MBus %1").arg(this->toString()));

	if (!isValid()) {
		setAgentStatus(Status::STOPPED);
		Logger::getInstance().error(QString("Invalid modbus configuration [dp:%1, port:%2]! Property missing on object %3.").arg(this->dpName).arg(this->port).arg(QString(this->toString())));
		return;
	}

	// init status
	setAgentStatus(Status::INITIALIZE);

	while(this->running) {
		qint64 from = QDateTime::currentMSecsSinceEpoch();

		if (Resources::isRedActive()) {
			this->poll();
		} else {
			tryToDisconnect();
		}

		// wait delta
		int delta = (int)(QDateTime::currentMSecsSinceEpoch() - from);
		Logger::getInstance().debug(QString("Polling [port:%1] time elapsed %2").arg(this->port).arg(QString::number(delta)));

		if (delta < this->polling) {
			this->msleep(this->polling - delta);
		}
	}
}

void MBusAsciiMaster::poll() {
	int result;
	QString sCoils, sRegisters;
	QList<int> busStatus;
	bool atLeastOneNotOK = false;
	bool atLeastOneOK = false;

	// connect
	bool ret = tryToConnect();
	if (!ret) return;

	qint64 from = 0;

	for (int busNumber = 0; busNumber < bus->count(); busNumber++) {

		// heartbeat management
		qint64 delta = QDateTime::currentMSecsSinceEpoch() - from;
		if (delta >= MBusAsciiMaster::HEARTBEAT_MINIMUM_REWRITE) {
			this->setHeartbeat();
			Logger::getInstance().debug(QString("Heartbeat [dp:%1, port:%2]]").arg(this->dpName).arg(this->port));
			from = QDateTime::currentMSecsSinceEpoch();
		}

		int b = (bus->at(busNumber));

		busStatus.append(Status::CONNECTED);
		int coils[COIL_NUMBER];
		short registers[REGISTER_NUMBER];

		// read coils
		result = mbusProtocol->readCoils(b, 1, coils, COIL_NUMBER);
		if (result != FTALK_SUCCESS) {
			// bus status
			busStatus[busNumber] = Status::DISCONNECTED;
			atLeastOneNotOK = true;
			Logger::getInstance().error(QString("Error occured [dp:%1, port:%2, add:%3] during coils read [%4]! Device should not be connected or bus does not exists.").arg(this->dpName).arg(this->port).arg(QString::number(b)).arg(QString(getBusProtocolErrorText(result))));
			sendInvalidCoils(this->agentID, b);
			sendInvalidRegisters(this->agentID, b);
			continue;
		}
		for (int i = 0; i < COIL_NUMBER; i++) {
			sCoils = sCoils % QString::number(coils[i]);
			Driver::getInstance().getService()->setStatus(this->agentID, b, i + 1, DriverTypes::COILS, QString::number(coils[i]), false);
		}
		Logger::getInstance().debug(QString("Coils read [dp:%1, port:%2, add:%3]:%4]").arg(this->dpName).arg(this->port).arg(QString::number(b)).arg(sCoils));

		// read registers
		result = mbusProtocol->readMultipleRegisters(b, 1, registers, REGISTER_NUMBER);
		if (result != FTALK_SUCCESS) {
			// bus status
			busStatus[busNumber] = Status::DISCONNECTED;
			atLeastOneNotOK = true;
			Logger::getInstance().error(QString("Error occured [dp:%1, port:%2, add:%3] during register read [%4]! Unexpected error.").arg(this->dpName).arg(this->port).arg(QString::number(b)).arg(QString(getBusProtocolErrorText(result))));
			sendInvalidRegisters(this->agentID, b);
			continue;
		}
		for (int i = 0; i < REGISTER_NUMBER; i++) {
			if (i > 0) sRegisters = sRegisters % ",";
			sRegisters = sRegisters % QString::number(registers[i]);
			Driver::getInstance().getService()->setStatus(this->agentID, b, i + 1, DriverTypes::REGISTERS, QString::number(registers[i]), false);
		}
		atLeastOneOK = true;
		Logger::getInstance().debug(QString("Registers read [dp:%1, port:%2, add:%3]:%4]").arg(this->dpName).arg(this->port).arg(QString::number(b)).arg(sRegisters));
	}

	// status
	setBusStatus(busStatus);
	if (!atLeastOneNotOK) {
		Logger::getInstance().debug(QString("Connected to %1").arg(this->port));
		setAgentStatus(Status::CONNECTED);
	} else if (atLeastOneOK) {
		Logger::getInstance().debug(QString("Connected (but degraded) to %1").arg(this->port));
		setAgentStatus(Status::DEGRADED);
	} else {
		Logger::getInstance().debug(QString("Disconnected from %1").arg(this->port));
		setAgentStatus(Status::DISCONNECTED);
	}
}

// operator
QString MBusAsciiMaster::toString() {
	return QString("MBusAsciiMaster [dp:%1, port:%2, baud rate:%3, params:%4,%5,%6, polling:%7]").arg(this->dpName).arg(this->port).arg(QString::number(this->baudRate)).arg(QString::number(this->dataBits)).arg(QString::number(this->stopBits)).arg(QString::number(this->parity)).arg(QString::number(this->polling));
}

