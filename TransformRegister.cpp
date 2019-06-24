// Our transformation class PVSS <--> Hardware

// local
#include <Global.h>

// pvss
#include <TransformRegister.hxx>
#include <ErrHdl.hxx>     // The Error handler Basics/Utilities
#include <TextVar.hxx>    // Our Variable type Basics/Variables
#include <IntegerVar.hxx>    // Our Variable type Basics/Variables

#include <stdio.h>

//----------------------------------------------------------------------------

TransformationType TransformRegister::isA() const
{
	return (TransformationType) DriverTypes::REGISTERS;
}

//----------------------------------------------------------------------------

TransformationType TransformRegister::isA(TransformationType type) const
{
	if ( type == isA() )
		return type;
	else
		return Transformation::isA(type);
}

//----------------------------------------------------------------------------

Transformation *TransformRegister::clone() const
{
	return new TransformRegister;
}

//----------------------------------------------------------------------------
// Our item size. The max we will use is 256 Bytes.
// This is an arbitrary value! A Transformation for a long e.g. would return 4

int TransformRegister::itemSize() const
{
	return sizeof(PVSSlong);
}

//----------------------------------------------------------------------------
// Our preferred Variable type. Data will be converted to this type
// before toPeriph is called.

VariableType TransformRegister::getVariableType() const
{
	return INTEGER_VAR;
}

//----------------------------------------------------------------------------
// Convert data from PVSS to Hardware.
// In this example the subindex is ignored

PVSSboolean TransformRegister::toPeriph(PVSSchar *dataPtr, PVSSuint len, const Variable &var, const PVSSuint subix) const {
    return PVSS_FALSE;
}

//----------------------------------------------------------------------------
// Conversion from Hardware to PVSS

VariablePtr TransformRegister::toVar(const PVSSchar *data, const PVSSuint dlen, const PVSSuint /* subix */) const {
    if (dlen >= sizeof(PVSSlong)) {
		QString d = QString(data);
		return new IntegerVar(d.toInt());
	}
	
	return PVSS_FALSE;
}

//----------------------------------------------------------------------------
