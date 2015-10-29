// Includes.
#include <Thread.h>
#include <ThreadController.h>

// Constants for the analog pin of witch sensor.
#define batTempPin A0			// A0: Battery Temperature Sensor
#define motorTempPin A1			// A1: Motor Temperature Sensor
#define batVoltPin A2			// A2: Battery Voltage Sensor
#define mpptCurPin A3			// A3: MPPT Current Sensor
#define motorCurPin A4			// A4: Motor Current Sensor
#define potPin A5			// A5: Potentiometer Voltage

unsigned int batTemp;
unsigned int motorTemp;
unsigned int batVolt;
unsigned long mpptCurxTime;	
unsigned long motorCurxTime;
unsigned int potVolt;
unsigned long initialTime1, initialTime2, actualTime1, actualTime2, oldTime1, oldTime2;
byte sensor = 0xA0;				// variable used for comunication

// ThreadController that will controll all threads.
ThreadController controll = ThreadController();

// Threads (as pointer).
Thread* measuresThread = new Thread();
Thread* sendDataThread = new Thread();

// Callbacks.
void measuresCallBack();
void sendDataCallBack();

void setup(){
	Serial.begin(9600);

        pinMode(2, OUTPUT);
        pinMode(3, OUTPUT);
        pinMode(4, OUTPUT);
	pinMode(5, OUTPUT);
        pinMode(6, OUTPUT);
        pinMode(7, OUTPUT);
        pinMode(8, OUTPUT);
        pinMode(9, OUTPUT);
        pinMode(12, OUTPUT);
        pinMode(13, OUTPUT);
        
        pinMode(A0, INPUT);
        pinMode(A1, INPUT);
        pinMode(A2, INPUT);
        pinMode(A3, INPUT);
        pinMode(A4, INPUT);
        pinMode(A5, INPUT);

	// Configure Threads.
	measuresThread->onRun(measuresCallBack);
	measuresThread->setInterval(100);
	sendDataThread->onRun(sendDataCallBack);
	sendDataThread->setInterval(100);

	// Adds both threads to the controller.
	controll.add(measuresThread);
	controll.add(sendDataThread);

	noInterrupts();//atomic block
	initialTime1 = micros();
	initialTime2 = micros();
	interrupts();
        
}

void loop(){
	controll.run();
	//A full
	noInterrupts();
	//multiplica as correntes pelo intervalo entre as aquisicoes
	actualTime1 = micros();
	mpptCurxTime += analogRead(mpptCurPin)*(actualTime1 - oldTime1);
	// Serial.print(actualTime1 - oldTime1); // debug
	oldTime1 = actualTime1;
	actualTime2 = micros();
	motorCurxTime += analogRead(motorCurPin)*(actualTime2 - oldTime2);
	oldTime2 = actualTime2;
	// Serial.print(actualTime1 - oldTime1); // debug
	interrupts();
	
}

void sendDataCallBack(){
	// Get command
	if (Serial.available()) {
		//noInterrupts();
		byte c = Serial.read();
		// if an ACK is received, send it back
		if (c == 0x01 ){
			Serial.write(c); // send ACK back   
			switch (sensor) {
				case 0xA0: // Battery Temperature Sensor
//					BluetoothSend(mapFloat((long)batTemp,0,1023,0,150));
                                        BluetoothSend(32.4f); //FAKE DATA FOR DEBUG
					sensor ++;
					break;
				case 0xA1: // Motor Temperature Sensor
//					BluetoothSend(mapFloat((long)motorTemp,0,1023,0,150));
                                        BluetoothSend(51.9f); //FAKE DATA FOR DEBUG
					sensor ++;
					break;
				case 0xA2: // Battery Voltage Sensor
//					BluetoothSend(mapFloat((long)batVolt,0,1023,0,48));
                                        BluetoothSend(25.8f); //FAKE DATA FOR DEBUG
					sensor ++;
					break;
				case 0xA3: // MPPT Current Sensor
					BluetoothSend(mapFloat((long)mpptCurxTime,0,1023,0,150));
//					BluetoothSend(0.1f); //FAKE DATA FOR DEBUG
					// Serial.print("\n Current1:  "); // debug
					// Serial.print(mapFloat( (float)mpptCurxTime / (micros() - initialTime1), 0, 1023, 0, 5)); // debug
					// Serial.print("\t DeltaTime:  "); // debug
					// Serial.print(micros() - initialTime1); // debug
					initialTime1 = micros();
					mpptCurxTime = 0;
					sensor ++;
					break;
				case 0xA4: // Motor Current Sensor
					BluetoothSend(mapFloat((long)motorCurxTime,0,1023,0,150));
//					BluetoothSend(200.0f); //FAKE DATA FOR DEBUG
					// Serial.print("\t\t Current2:"); // debug
					// Serial.print(mapFloat( (float)motorCurxTime / (micros() - initialTime2), 0, 1023, 0, 5)); // debug
					// Serial.print("\t DeltaTime:  "); // debug
					// Serial.print(micros() - initialTime2); // debug
					initialTime2 = micros();
					motorCurxTime = 0;
					sensor ++;
					break;
				case 0xA5: // Potentiometer Voltage
//					BluetoothSend(2.0f); //FAKE DATA FOR DEBUG
                                        BluetoothSend(mapFloat((long)batVolt,0,1023,0,5));
					sensor = 0xA0;
					break;
				default: 
					sensor = 0xA0;
					break;
			}
		}
	}
}

void measuresCallBack(){
	batTemp = analogRead(batTempPin);
	motorTemp = analogRead(motorTempPin);
	batVolt = analogRead(batVoltPin);
}

// Sends the float value as a 4 bytes little endian and than breaks the line.
void BluetoothSend(float value){
	byte * b = (byte *) &value;
	Serial.write(0x02); // STX (start of text)
		Serial.write(sensor);
	Serial.write(b,4); // 4 bytes Little Endian
	Serial.write(0x03); // ETX (end of text)
}

// A copy of "map" function working with float values.
float mapFloat(long value, int in_min, int in_max, int out_min, int out_max){
	return (float) ( (value - in_min) * (out_max - out_min) )/( (float) (in_max - in_min) ) + out_min;
}


