#ifndef SerialModbusDRV_H_
#define SerialModbusDRV_H_

#include <DrvManager.hxx>

class SerialModbusDrv : public DrvManager
{
  public:
    virtual void install_HWMapper();
    virtual void install_HWService();
};

#endif
