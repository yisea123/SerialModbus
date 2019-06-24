#include "MBusAbstract.h"
#include "Driver.h"

MBusAbstract::MBusAbstract(const QString &dpName) {
	this->running = true;
	this->bus = new QList<int>();
	this->dpName = dpName;
}

MBusAbstract::~MBusAbstract() {
	if (bus) delete (bus);
}

int MBusAbstract::getAgentID() {
	return this->agentID;
}

void MBusAbstract::setAgentID(int agentID) {
	this->agentID = agentID;
}

QString MBusAbstract::getPort() {
	return this->port;
}

void MBusAbstract::setPort(QString port) {
	this->port = port;
}

int MBusAbstract::getBaudRate(){
	return this->baudRate;
}

void MBusAbstract::setBaudRate(int baudRate) {
	this->baudRate = baudRate;
}

int MBusAbstract::getDataBits() {
	return this->dataBits;
}

void MBusAbstract::setDataBits(int dataBits) {
	this->dataBits = dataBits;
}

int MBusAbstract::getStopBits() {
	return this->stopBits;
}

void MBusAbstract::setStopBits(int stopBits) {
	this->stopBits = stopBits;
}

int MBusAbstract::getParity() {
	return this->parity;
}

void MBusAbstract::setParity(int parity) {
	this->parity = parity;
}

int MBusAbstract::getPolling() {
	return this->polling;
}

void MBusAbstract::setPolling(int polling) {
	this->polling = polling;
}

int MBusAbstract::getTimeout() {
	return this->timeout;
}

void MBusAbstract::setTimeout(int timeout) {
	this->timeout = timeout;
}

int MBusAbstract::getRetry() {
	return this->retry;
}

void MBusAbstract::setRetry(int retry) {
	this->retry = retry;
}

int MBusAbstract::getPollDelay() {
	return this->pollDelay;
}

void MBusAbstract::setPollDelay(int pollDelay) {
	this->pollDelay = pollDelay;
}

void MBusAbstract::addBus(int address) {
	bus->append(address);
}

void MBusAbstract::setAgentStatus(int status) {
	if (Resources::isRedActive()) {
		Driver::getInstance().getService()->setDpValue(getStatusId(), new UIntegerVar(status));
	} else {
		Driver::getInstance().getService()->setDpValue(getStatusId(), new UIntegerVar(Status::STANDBY));
	}
}

void MBusAbstract::setBusStatus(QList<int> &status) {

	if (Resources::isRedActive()) {
		DynVar *v = new DynVar(VariableType::UINTEGER_VAR);
		for (int i = 0; i<status.count(); i++) {
			v->append(new UIntegerVar(status.at(i)));
		}

		Driver::getInstance().getService()->setDpValue(getBusStatusId(), v);
		Driver::getInstance().getService()->setDpValue(getBusStatusInvalidId(), new BitVar(false));
	}
}

DpIdentifier MBusAbstract::getStatusId() {
	if (Resources::getReplica() == 1) {
		return this->statusId_1;
	} else {
		return this->statusId_2;
	}
}

void MBusAbstract::setStatusId_1(DpIdentifier dpId) {
	statusId_1 = dpId;
}

void MBusAbstract::setStatusId_2(DpIdentifier dpId) {
	statusId_2 = dpId;
}

DpIdentifier MBusAbstract::getHeartbeatId() {
	return heartbeatId;
}

void MBusAbstract::setHeartbeatId(DpIdentifier dpId) {
	heartbeatId = dpId;
}

void MBusAbstract::setHeartbeat() {
	Driver::getInstance().getService()->setDpValue(heartbeatId, new BitVar(PVSS_TRUE));
}

void MBusAbstract::setBusStatusId(DpIdentifier dpId) {
	statusBus = dpId;
}

DpIdentifier MBusAbstract::getBusStatusId() {
	return statusBus;
}

void MBusAbstract::setBusStatusInvalidId(DpIdentifier dpId) {
	statusBusInvalid = dpId;
}

DpIdentifier MBusAbstract::getBusStatusInvalidId() {
	return statusBusInvalid;
}
// property
bool MBusAbstract::isValid() {
	return this->polling >= 2000
		&& this->port != ""
		&& this->baudRate > 0;
}

// method
void MBusAbstract::sendInvalidCoils(int agent, int b) {
	for (int i = 0; i < COIL_NUMBER; i++) {
		Driver::getInstance().getService()->setStatus(agent, b, i + 1, DriverTypes::COILS, QString::number(0), true);
	}
}

void MBusAbstract::sendInvalidRegisters(int agent, int b) {
	for (int i = 0; i < REGISTER_NUMBER; i++) {
		Driver::getInstance().getService()->setStatus(agent, b, i + 1, DriverTypes::REGISTERS, QString::number(0), true);
	}
}

void MBusAbstract::stop() {
	this->running = false;

	// stopped status
	this->setAgentStatus(Status::STOPPED);
}