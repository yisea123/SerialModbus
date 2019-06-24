#include <SerialModbusHWMapper.hxx>
#include <TransformCoil.hxx>
#include <TransformRegister.hxx>

#include <ErrHdl.hxx>         // error handler

//--------------------------------------------------------------------------------
// We get new configs here. Create a new HW-Object on arrival and insert it.

void SerialModbusHWMapper::addParamAddress(const CharString& cs) {
	QString fullName = QString(cs);
	QString address = Driver::getInstance().extractAddress(fullName);
	Driver::getInstance().manageParamAddress(address, fullName);
}

void SerialModbusHWMapper::clrParamAddress(const CharString& cs) {
	// TODO
}

PVSSboolean SerialModbusHWMapper::addDpPa(DpIdentifier &dpId, PeriphAddr *confPtr)
{
	// Removed because of too much logs
	//Logger::getInstance().debug(QString("addDpPa called for %1 of type %2").arg(QString(confPtr->getName())).arg(QString::number(confPtr->getTransformationType())));

	switch (confPtr->getTransformationType()) {
		case DriverTypes::COILS:
			confPtr->setTransform(new TransformCoil());
			break;
		case DriverTypes::REGISTERS:
			confPtr->setTransform(new TransformRegister());
			break;
		default :
			Logger::getInstance().error(QString("Illegal transformation type %1").arg(QString::number((int)confPtr->getTransformationType())));

			// But add the config !
			return HWMapper::addDpPa(dpId, confPtr);
	}

	// First add the config, then the HW-Object
	if ( !HWMapper::addDpPa(dpId, confPtr) )  // FAILED !!
		return PVSS_FALSE;

	HWObject *hwObj = new HWObject;
	// Set Address and Subindex
	hwObj->setAddress(confPtr->getName());       // Resolve the HW-Address, too

	// Set the data type.
	hwObj->setType(confPtr->getTransform()->isA());

	// Set the len needed for data from _all_ subindices of this PVSS-Address.
	// Because we will deal with subix 0 only this is the Transformation::itemSize
	hwObj->setDlen(confPtr->getTransform()->itemSize());

	addParamAddress(confPtr->getName());

	// Add it to the list
	addHWObject(hwObj);

	return PVSS_TRUE;
}

//--------------------------------------------------------------------------------

PVSSboolean SerialModbusHWMapper::clrDpPa(DpIdentifier &dpId, PeriphAddr *confPtr)
{
	Logger::getInstance().debug(QString("clrDpPa called for %1").arg(QString(confPtr->getName())));

  // Find our HWObject via a template
  HWObject adrObj;
  adrObj.setAddress(confPtr->getName());

  // Lookup HW-Object via the Name, not via the HW-Address
  // The class type isn't important here
  HWObject *hwObj = findHWAddr(&adrObj);

  if ( hwObj )
  {
    // Object exists, remove it from the list and delete it.
    clrHWObject(hwObj);
    delete hwObj;
  }

  // Call function of base class to remove config
  return HWMapper::clrDpPa(dpId, confPtr);
}

//--------------------------------------------------------------------------------
