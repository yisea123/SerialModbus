#include <SerialModbusDrv.hxx>
#include <SerialModbusHWMapper.hxx>
#include <SerialModbusHWService.hxx>

//------------------------------------------------------------------------------------

void SerialModbusDrv::install_HWMapper()
{
  hwMapper = new SerialModbusHWMapper;
}

//--------------------------------------------------------------------------------

void SerialModbusDrv::install_HWService()
{
  hwService = new SerialModbusHWService;
}

//--------------------------------------------------------------------------------
