#pragma once

// local
#include "Global.h"

// pvss
#include <CharString.hxx>
#include <DpIdentifier.hxx>

class PVSSDataPoint {

public:
	// constructor
	PVSSDataPoint(int, int, QString);
	~PVSSDataPoint();

	// properties
	int getNumber();
	void setNumber(int number);

	int getUsage();
	void setUsage(int usage);

	const QString getDpName();
	void setDpName(QString);

	const DpIdentifier getDpId();
	void setDpId(DpIdentifier);

private:
	// attributs
	int number;
	int usage;
	QString dpName;
	DpIdentifier dpId;

};
