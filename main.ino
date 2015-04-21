// Includes.
#include <Thread.h>
#include <ThreadController.h>

// Temporary variables for the float mapped values.
float temperature1;					// internalTempMeasure
float temperature2;					// motorTempMeasure
float voltage1;						// batteryVoltageMeasure
float current1;						// panelsCurrentMeasure
float current2;						// motorCurrentMeasure

// Constants for the analog pin of witch sensor.
const int temperature1Pin = A0;		// A0: internalTempMeasure
const int temperature2Pin = A1;		// A1: motorTempMeasure
const int voltage1Pin = A2;			// A2: batteryVoltageMeasure
const int current1Pin = A3;			// A3: panelsCurrentMeasure
const int current2Pin = A4;			// A4: motorCurrentMeasure

// ThreadController that will controll all threads.
ThreadController controll = ThreadController();

// Threads (as pointer).
Thread* temperature1Thread = new Thread();
Thread* temperature2Thread = new Thread();
Thread* voltage1Thread = new Thread();
Thread* current1Thread = new Thread();
Thread* current2Thread = new Thread();
Thread* sendDataThread = new Thread();


// Callbacks.

void temperature1CallBack();
void temperature2CallBack();
void voltage1CallBack();
void current1CallBack();
void current2CallBack();
void senddataCallBack();
void protocol();
void ProcessConfirmation(float measure_float);
boolean ReadByteArrayAndCompare(byte byteSent[],int array_size);
boolean ByteArrayCompare(byte a[],byte b[],int array_size);
void BluetoothSend(float value);
void BluetoothSend_debug(float value);
float mapFloat(float value, float in_min, float in_max, float out_min, float out_max);

void temperature1CallBack(){
	// Serial.print("debug: temperature1CallBack   - time: " + String(millis()) + "\n" );
	temperature1 = mapFloat(analogRead(temperature1Pin), 0, 1023, 2, 150);
}
void temperature2CallBack(){
	temperature2 = mapFloat(analogRead(temperature2Pin), 0, 1023, 2, 150);
}
void voltage1CallBack(){
	voltage1 = mapFloat(analogRead(voltage1Pin), 0, 1023, 0, 47);
}
void current1CallBack(){
	current1 = mapFloat(analogRead(current1Pin), 0, 1023, 0, 150);
}
void current2CallBack(){
	current2 = mapFloat(analogRead(current2Pin), 0, 1023, 0, 150);
}
void senddataCallBack(){
	//protocol();
}

void setup(){
	Serial.begin(9600);

	// Configure Threads.
	temperature1Thread->onRun(temperature1CallBack);
	temperature1Thread->setInterval(1500);
	temperature2Thread->onRun(temperature2CallBack);
	temperature2Thread->setInterval(1500);
	voltage1Thread->onRun(voltage1CallBack);
	voltage1Thread->setInterval(250);
	current1Thread->onRun(current1CallBack);
	current1Thread->setInterval(250);
	current2Thread->onRun(current2CallBack);
	current2Thread->setInterval(250);
	sendDataThread->onRun(senddataCallBack);
	sendDataThread->setInterval(2000);

	// Adds both threads to the controller.
	controll.add(temperature1Thread);
	controll.add(temperature2Thread);
	controll.add(voltage1Thread);
	controll.add(current1Thread);
	controll.add(current2Thread);
	controll.add(sendDataThread);
}

void loop(){
	// run ThreadController
	// this will check every thread inside ThreadController,
	// if it should run. If yes, he will run it;
	controll.run();

	// Rest of code
	protocol();
}

// Waits for a request (through serial) of a measure and than answer the measure.
void protocol(){
    // Get command
    if (Serial.available()) {
    	//noInterrupts();
    	byte c = Serial.read();
		// if an ACK is received, send it back
		if (c == 0x01 ){
			Serial.write(c); // send ACK back
			while(!Serial.available());
			// ENQ
			c = Serial.read();
			if (c == 0x05){
				// Serial.write(c); // debug
				while(!Serial.available());
				c = Serial.read();
				switch (c) {
					case 0xA0: // internalTempMeasure
						//BluetoothSend(temperature1);
						BluetoothSend(123.2f);
						// ProcessConfirmation(123.2f);
						break;
					case 0xA1: // motorTempMeasure
						BluetoothSend(502.1f);
						// ProcessConfirmation(502.1f);
						break;
					case 0xA2: // batteryVoltageMeasure
						//BluetoothSend(voltage1);
						BluetoothSend(321.1f);
						break;
					case 0xA3: // panelsCurrentMeasure
						//BluetoothSend(current1);
						BluetoothSend(654.1f);
						break;
					case 0xA4: // motorCurrentMeasure
						//BluetoothSend(current2);
						BluetoothSend(099.1f);
						break;
					default: 
						Serial.write(0x25); // mensagem de erro: NAK
						break;
				}
			}
		}
		//interrupts();
	}
}

void ProcessConfirmation(float measure_float) {
	// procedimento
	// 1: recria mensagem enviada
	byte * measure_byteArray = (byte *) &measure_float;
	// recreate exactly in the order that it was sent
	byte msgSent[6] = {0x02, measure_byteArray[0], measure_byteArray[1], measure_byteArray[2], measure_byteArray[3], 0x03};

	// byte msgSent[6] = {0x02, 0x00, 0x00, 0xF6, 0x42, 0x03};

	// 2: recebe mensagem
	// 3: compara as duas mensagens
	// 4: responde ACK ou NAK
	if(ReadByteArrayAndCompare(msgSent, 6)){
		Serial.write(0x01); // ACK
	}
	else{
		Serial.write(0x25); // NAK
	}
}

//
// Compare an array of specified size (array_size) to N (array_size) bytes readed;
// Compare witch byteReaded to witch byte from byteSent[]
boolean ReadByteArrayAndCompare(byte byteSent[],int array_size) { //
	for (int i = 0; i < array_size; ++i){
		while(!Serial.available());
		byte byteReaded = Serial.read();
		if (byteSent[i] != byteReaded) {
			return(false);
		}
	}
	return(true);
}

// Compare two arrays with specifed size
boolean ByteArrayCompare(byte a[],byte b[],int array_size) {
	for (int i = 0; i < array_size; ++i){
		if (a[i] != b[i]) {
			return(false);
		}
	}
	return(true);
}

// Sends the float value as a 4 bytes little endian and than breaks the line.
void BluetoothSend(float value){
	byte * b = (byte *) &value;
	Serial.write(0x02); // STX (start of text)
	Serial.write(b,4); // 4 bytes Little Endian
	Serial.write(0x03); // ETX (end of text)
}

// Sends the float value as a string and than breaks the line.
void BluetoothSend_debug(float value){
	Serial.print(value);
	Serial.print("\n");
}

// A copy of "map" function working with float values.
float mapFloat(float value, float in_min, float in_max, float out_min, float out_max)
{
	return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}






