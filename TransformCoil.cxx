// Our transformation class PVSS <--> Hardware

// local
#include <Global.h>

// pvss
#include <TransformCoil.hxx>
#include <ErrHdl.hxx>     // The Error handler Basics/Utilities
#include <TextVar.hxx>    // Our Variable type Basics/Variables
#include <BitVar.hxx>    // Our Variable type Basics/Variables

#include <stdio.h>

//----------------------------------------------------------------------------

TransformationType TransformCoil::isA() const
{
	return (TransformationType) DriverTypes::COILS;
}

//----------------------------------------------------------------------------

TransformationType TransformCoil::isA(TransformationType type) const
{
	if ( type == isA() )
		return type;
	else
		return Transformation::isA(type);
}

//----------------------------------------------------------------------------

Transformation *TransformCoil::clone() const
{
	return new TransformCoil;
}

//----------------------------------------------------------------------------
// Our item size. The max we will use is 256 Bytes.
// This is an arbitrary value! A Transformation for a long e.g. would return 4

int TransformCoil::itemSize() const
{
	return 1;
}

//----------------------------------------------------------------------------
// Our preferred Variable type. Data will be converted to this type
// before toPeriph is called.

VariableType TransformCoil::getVariableType() const
{
	return BIT_VAR;
}

//----------------------------------------------------------------------------
// Convert data from PVSS to Hardware.
// In this example the subindex is ignored

PVSSboolean TransformCoil::toPeriph(PVSSchar *buffer, PVSSuint len,
                                      const Variable &var, const PVSSuint subix) const
{
	// Be paranoic, check variable type
	if ( var.isA() != BIT_VAR )
	{
		// Throw error message
		ErrHdl::error(
		  ErrClass::PRIO_SEVERE,             // Data will be lost
		  ErrClass::ERR_PARAM,               // Wrong parametrization
		  ErrClass::UNEXPECTEDSTATE,         // Nothing else appropriate
		  "CoilTrans", "toPeriph",     // File and function name
		  "Wrong variable type for data"     // Unfortunately we don't know which DP
		);

		return PVSS_FALSE;
	}

	if (((BitVar &) var).getValue() == PVSS_TRUE) {
		buffer[0] = (PVSSchar)'1';
	} else {
		buffer[0] = (PVSSchar)'0';
	}

	return PVSS_TRUE;
}

//----------------------------------------------------------------------------
// Conversion from Hardware to PVSS

VariablePtr TransformCoil::toVar(const PVSSchar *buffer, const PVSSuint dlen,
                                   const PVSSuint /* subix */) const
{
	if (sizeof(buffer) > 0 && buffer[0] == '1') {
		return new BitVar(PVSS_TRUE);
	} else {
		return new BitVar(PVSS_FALSE);
	}
}

//----------------------------------------------------------------------------
