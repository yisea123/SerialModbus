#include "Driver.h"

using namespace std;

Driver::Driver(void) {
	mbus = new QMap<int, MBusAbstract *>();
	paramAddress = new QMap<QString, QString>();
	queues = new QMap<QString, QQueue<PVSSchar>*>();
}

Driver::~Driver(void) {
	foreach (MBusAbstract *mb, *mbus) {
		((QThread*)mb)->terminate();
	}

	qDeleteAll(*mbus);
	if (mbus) {
		mbus = NULL;
		delete mbus;
	}

	if (paramAddress) {
		paramAddress = NULL;
		delete paramAddress;
	}

	qDeleteAll(*queues);
	if (queues) {
		queues = NULL;
		delete queues;
	}

}

Driver &Driver::getInstance() {
	static Driver instance;
	return instance;
}

SerialModbusHWService *Driver::getService() {
	return service;
}

void Driver::addMBus(int i, MBusAbstract *mb) {
	this->mbus->insert(i, mb);
}

bool Driver::hasParameter(const QString& fullName) {
	return fullName.contains("$");
}

QString Driver::extractAddress(const QString& fullName) {
	if (fullName.contains("$")) {
		return fullName.left(fullName.indexOf("$"));
	} else {
		return fullName;
	}
}

int Driver::extractTime(const QString& fullName) {
	return fullName.right(fullName.size() - fullName.indexOf("$") - 1).toInt();
}

void Driver::manageParamAddress(const QString& key, const QString& value) {
	if (value == 0 && paramAddress->contains(key)) { // exists and no cycle -> remove it
		paramAddress->remove(key);
	} else { // cycle -> create / update it
		paramAddress->insert(key, value);
	}
}

QString Driver::getParamAddress(const QString& address) {
	if (paramAddress->contains(address)) {
		QString value = paramAddress->value(address);
		return (paramAddress->value(address));
	}

	return address;
}

bool Driver::applyValueAgainstSmoothing(const QString& address, int time, const PVSSchar& value) {
	// lazy initialization
	if (!queues->contains(address)) {
		queues->insert(address, new QQueue<PVSSchar>());
	}

	QQueue<PVSSchar>* queue = queues->value(address);
	queue->append(value);
	if (queue->size() > time) {
		for (int i = 0; i < queue->size() - time; i++) {
			queue->removeFirst();
		}
	}

	for (int i = 0; i < queue->size(); i++) {
		if (queue->at(i) != value) return false;
	}

	return true;
}

MBusAbstract* Driver::getMBus(int i) {
	return this->mbus->value(i, NULL);
}

void Driver::registerService(SerialModbusHWService *service) {
	*this->service = *service;
}

void Driver::stop() {
	foreach (MBusAbstract *mb, *mbus) {
		mb->stop();
	}
}