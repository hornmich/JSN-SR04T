/*
 * UsonicSensor.cpp
 *
 *  Created on: 14. 10. 2016
 *      Author: michal
 */

#include <jsn-se04t_sensor/UsonicSensor.h>

UsonicSensor::UsonicSensor(PinName trigger, PinName echo) :
triggerPin(trigger), echoPin(echo)
{
	echoPin.rise(this, &UsonicSensor::echoStartedCb);
	echoPin.fall(this, &UsonicSensor::echoCompletedCb);
	measuredTimeUs = 0;
	sensorStatus = UsonicSensor::Ready;
	measuringCompletedCb = NULL;

}

void UsonicSensor::attach(UsonicSensorCallback callback) {
	measuringCompletedCb = callback;
}

int UsonicSensor::getDistanceCm() const {
	int distanceCm;
	if (measuredTimeUs >= 30000) {
		distanceCm = -1;
	}
	else {
		distanceCm = measuredTimeUs >> 6;
	}

	return distanceCm;
}

int UsonicSensor::getEchoTimeUs() const {
	return measuredTimeUs;
}

UsonicSensor::Status UsonicSensor::getStatus() const {
	return sensorStatus;
}

void UsonicSensor::startMeasure() {
	sensorStatus = UsonicSensor::Measuring;
	measuredTimeUs = 0;
	triggerPin = 1;
	triggerTime.attach_us(this, &UsonicSensor::triggerFinishedCb, 10);
}

UsonicSensor::~UsonicSensor() {

}

void UsonicSensor::triggerFinishedCb() {
	triggerPin = 0;
}

void UsonicSensor::echoStartedCb() {
	echoDuration.start();
}

void UsonicSensor::echoCompletedCb() {
	echoDuration.stop();
	measuredTimeUs = echoDuration.read_us();
	echoDuration.reset();
	sensorStatus = UsonicSensor::Ready;
	if (measuringCompletedCb != NULL) {
		measuringCompletedCb();
	}
}
