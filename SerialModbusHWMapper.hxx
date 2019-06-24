#ifndef SerialModbusHWMAPPER_H_
#define SerialModbusHWMAPPER_H_

#include <HWMapper.hxx>
#include <Global.h>
#include <Driver.h>

class SerialModbusHWMapper : public HWMapper
{
  public:
	void addParamAddress(const CharString& cs);
	void clrParamAddress(const CharString& cs);
    virtual PVSSboolean addDpPa(DpIdentifier &dpId, PeriphAddr *confPtr);
    virtual PVSSboolean clrDpPa(DpIdentifier &dpId, PeriphAddr *confPtr);
};

#endif
