#pragma once

#include <Transformation.hxx>

// Our Transformation class for Text
// As the Transformation really depends on the format of data you send and
// receive in your protocol (see HWService), this template is just an
// example.
// Things you have to change are marked with TODO

class TransformCoil : public Transformation
{
  public:
    TransformationType isA() const;
    TransformationType isA(TransformationType type) const;

    // (max) size of one item. This is needed by DrvManager to
    // create the buffer used in toPeriph and by the Low-Level-Compare
    // For our Text-Transformation we set it arbitrarly to 256 Bytes
    int itemSize() const;

    // The type of Variable we are expecting here
    VariableType getVariableType() const;

    // Clone of our class
    Transformation *clone() const;

    // Conversion from PVSS to Hardware
    PVSSboolean toPeriph(PVSSchar *dataPtr, PVSSuint len, const Variable &var, const PVSSuint subix) const;

    // Conversion from Hardware to PVSS
    VariablePtr toVar(const PVSSchar *data, const PVSSuint dlen, const PVSSuint subix) const;

};
