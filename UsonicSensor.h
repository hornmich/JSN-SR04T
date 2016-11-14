/*
 * UsonicSensor.h
 *
 *  Created on: 14. 10. 2016
 *      Author: michal
 */

#ifndef JSN_SE04T_SENSOR_USONICSENSOR_H_
#define JSN_SE04T_SENSOR_USONICSENSOR_H_

#include "mbed.h"

typedef void (*UsonicSensorCallback)();

enum Status{
	Ready,
	Measuring,
	Failed,
};

class UsonicSensor {
public:
	enum Status{
		Ready,
		Measuring,
		Failed,
	};

	UsonicSensor(PinName trigger, PinName echo);
	void attach(UsonicSensorCallback callback);
	int getDistanceCm() const;
	int getEchoTimeUs() const;
	UsonicSensor::Status getStatus() const;
	void startMeasure();
	virtual ~UsonicSensor();

private:
	DigitalOut triggerPin;
	InterruptIn echoPin;
	Timeout triggerTime;
	Timer echoDuration;
	UsonicSensor::Status sensorStatus;
	int measuredTimeUs;
	UsonicSensorCallback measuringCompletedCb;

	void triggerFinishedCb();
	void echoStartedCb();
	void echoCompletedCb();
};

#endif /* JSN_SE04T_SENSOR_USONICSENSOR_H_ */
