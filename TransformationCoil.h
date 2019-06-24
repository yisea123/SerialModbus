#pragma once

#include <Transformation.hxx>

// Our Transformation class for Text
// As the Transformation really depends on the format of data you send and
// receive in your protocol (see HWService), this template is just an
// example.
// Things you have to change are marked with TODO

class TransformationCoil : public Transformation
{
  public:
	  TransformationStatus(VariableType type) : varType(type) {}

    virtual TransformationType isA() const;
    virtual TransformationType isA(TransformationType type) const;

    virtual int itemSize() const;

    virtual VariableType getVariableType() const;

    virtual Transformation *clone() const;

    virtual PVSSboolean toPeriph(PVSSchar *dataPtr, PVSSuint len, const Variable &var, const PVSSuint subix) const;

    virtual VariablePtr toVar(const PVSSchar *data, const PVSSuint dlen, const PVSSuint subix) const;

  private:
	  VariableType varType;
};
