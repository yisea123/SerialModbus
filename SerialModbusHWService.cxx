#include <SerialModbusHWService.hxx>

#include "Driver.h"

#include <DrvManager.hxx>
#include <HWMapper.hxx>
#include <PVSSMacros.hxx>

// qt
#include <QtCore\QString>

const char* SerialModbusHWService::DATA_FORMAT = "%d_%d_%d.%d";

//--------------------------------------------------------------------------------
// called after connect to data
QQueue<Message *>* messages;
QQueue<DpValue *>* dpValues;
QMutex* mutex;

SerialModbusHWService::SerialModbusHWService() {
	messages = new QQueue<Message *>();
	dpValues = new QQueue<DpValue *>();
	mutex = new QMutex(QMutex::NonRecursive);
}

SerialModbusHWService::~SerialModbusHWService() {
	qDeleteAll(*messages);
	qDeleteAll(*dpValues);
	if (messages) {
		delete messages;
	}
	if (dpValues) {
		delete dpValues;
	}
	delete mutex;
}

PVSSboolean SerialModbusHWService::initialize(int argc, char *argv[])
{
  // use this function to initialize internals
  // if you don't need it, you can safely remove the whole method
  // TODO

  // To stop driver return PVSS_FALSE
  return PVSS_TRUE;
}

//--------------------------------------------------------------------------------
// called after connect to event

PVSSboolean SerialModbusHWService::start()
{
	Logger::getInstance().info(QString("SerialModbus v%1.%2 driver starting with reduNr %3.").arg(Global::V_MAJOR).arg(Global::V_MINOR).arg(Resources::getReplicaNr()));
	Logger::getInstance().info(QString("to debug driver, use USR_1 (-dbg 25) or USR_2 (-dbg 26) flags."));
	return PVSS_TRUE;
}

//--------------------------------------------------------------------------------

void SerialModbusHWService::stop()
{
	Logger::getInstance().info(QString("Driver stopping"));
	Driver::getInstance().stop();
}

//--------------------------------------------------------------------------------
HWObject* SerialModbusHWService::storeToDB(QString address, PVSSchar *value, bool invalid) {

	address = Driver::getInstance().getParamAddress(address);

	HWObject *obj = new HWObject();

//	obj->setType((TransformationType)DriverTypes::COILS);

	//DEBUG_MSG("address: " << address.toStdString().data());
	obj->setAddress(address.toStdString().data());
	obj->setDlen(ARRAY_SIZE(value));
	obj->setData(value);

	if (invalid) {
		obj->setSbit(DriverBits::DRV_INVALID);
		obj->setUserByte(0, 1);
	} else {
		obj->setUserByte(0, 0);
	}
	
	HWMapper* mapper = (HWMapper*)DrvManager::getHWMapperPtr();
	if ( mapper ) {
		HWObject* addrObj = mapper->findHWObject(obj);
		if ( addrObj ) {
			try {

				if (Driver::getInstance().hasParameter(address)) {
					int time = Driver::getInstance().extractTime(address);

					if (Driver::getInstance().applyValueAgainstSmoothing(address, time, *value)) {
						DrvManager::getSelfPtr()->toDp(obj, addrObj);
					} else {
						Logger::getInstance().debug(QString("Cycle delay smooth the value [%1] on %2 (%3) !").arg(QString(value)).arg(QString(addrObj->getAddress())).arg(QString(time)));
					}
				} else {
					DrvManager::getSelfPtr()->toDp(obj, addrObj);
				}
				// optional, to send it now !!!!
				//DrvManager::getSelfPtr()->sendData2Dp();
				//Logger::getInstance().debug(QString("Message stored to DB %1 [(%2_%3)%4]").arg(QString(addrObj->getAddress())).arg(QString(addrObj->getType())).arg(QString(sizeof(*value))).arg(QString(value)));
			} catch (std::exception& ex) {
				Logger::getInstance().error(QString("Exception occures %1 !").arg(ex.what()));

			} catch (...) {
				Logger::getInstance().error(QString("Unexpected exception !"));
			}
		} else {
			Logger::getInstance().warning(QString("Unknown address received: %1 !").arg(address));
		}
	}

	delete obj;
	return 0;
}

void SerialModbusHWService::workProc()
{
	// messages
	mutex->lock();

	while (messages->length() > 0) {

		struct Message *msg = messages->dequeue();

		storeToDB(msg->address, msg->value.toAscii().data(), msg->invalid);
		delete msg;

	}

	// dpValues
	while (dpValues->length() > 0) {

		struct DpValue *dpValue = dpValues->dequeue();

		Manager::dpSet(dpValue->dpId, *(dpValue->value));
		delete dpValue->value;
		delete dpValue;
	}

	mutex->unlock();
}

//--------------------------------------------------------------------------------
// we get data from PVSS and shall send it to the periphery

PVSSboolean SerialModbusHWService::writeData(HWObject *objPtr)
{

	Logger::getInstance().debug(QString("SerialModbusHWService::writeData to [%1] message [%2]").arg(QString(objPtr->getAddress())).arg(QString(objPtr->getData())));

  // TODO somehow send the data to your device

  // sending was successful
  return PVSS_TRUE;
}

void SerialModbusHWService::setStatus(int agent, int addr, int reference, DriverTypes type, QString value, bool invalid) {
	QString address;
	address.sprintf(DATA_FORMAT, agent, type, addr, reference);

	struct Message *msg;
	msg = new Message();
	msg->address = address;
	msg->type = type;
	msg->value = value;
	msg->invalid = invalid;

	// thread safe
	mutex->lock();

	// add to message queue
	messages->enqueue(msg);

	mutex->unlock();

}

//--------------------------------------------------------------------------------
void SerialModbusHWService::setDpValue(DpIdentifier dpId, Variable* value) {

	struct DpValue *dpValue;
	dpValue = new DpValue();
	dpValue->dpId = dpId;
	dpValue->value = value;

	// thread safe
	mutex->lock();

	// add to message queue
	dpValues->enqueue(dpValue);

	mutex->unlock();

}
