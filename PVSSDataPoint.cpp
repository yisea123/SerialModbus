#include "PVSSDataPoint.h"

PVSSDataPoint::PVSSDataPoint(int number, int usage, QString dpName) {
	this->number = number;
	this->usage = usage;
	this->dpName = dpName;
}

PVSSDataPoint::~PVSSDataPoint() {
}

int PVSSDataPoint::getNumber() {
	return this->number;
}

void PVSSDataPoint::setNumber(int number) {
	this->number = number;
}

int PVSSDataPoint::getUsage() {
	return this->usage;
}

void PVSSDataPoint::setUsage(int usage) {
	this->usage = usage;
}

const QString PVSSDataPoint::getDpName() {
	return this->dpName;
}

void PVSSDataPoint::setDpName(QString name) {
	this->dpName = name;
}

const DpIdentifier PVSSDataPoint::getDpId() {
	return this->dpId;
}

void PVSSDataPoint::setDpId(DpIdentifier dpId) {
	this->dpId = dpId;
}
