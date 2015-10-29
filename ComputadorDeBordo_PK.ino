// Defines for the analog pin of which sensor.
#define batTemperaturePin	  A0	// A0: Battery Temperature Sensor
#define motorTemperaturePin A1	// A1: Motor Temperature Sensor
#define batVoltagePin 		  A2	// A2: Battery Voltage Sensor
#define mpptCurrentPin 		  A3	// A3: MPPT Current Sensor
#define motorCurrentPin 	  A4	// A4: Motor Current Sensor
#define potPin 				      A5	// A5: Potentiometer Voltage

// Defines for control pins
#define reversePin    2
#define turnOnPin     3
#define turnOffPin    4
#define pumpPilot     5
#define pumpMotor     6
#define solarPanel1	  7
#define solarPanel2   8
#define solarPanel3  13
#define auxiliar     12
#define pwmOutput     9

// Global variables
int sensor = 0xA0;
// Prototype
void BluetoothSend(int);
void BluetoothFake(void);

// Setup
void setup()
{
	// Arduino comunication
	Serial.begin(9600);

	// Pins configuration
	/*pinMode(reversePin, OUTPUT);
	pinMode(turnOnPin, OUTPUT);
	pinMode(turnOffPin, OUTPUT);
	pinMode(pumpPilot, OUTPUT);
	pinMode(pumpMotor, OUTPUT);
	pinMode(solarPanel1, OUTPUT);
	pinMode(solarPanel2, OUTPUT);
	pinMode(solarPanel3, OUTPUT);
	pinMode(auxiliar, OUTPUT);
	pinMode(pwmOutput, OUTPUT);
 */
	
	// Analog pins configuration
	//pinMode(batTemperaturePin, INPUT);
	//pinMode(motorTemperaturePin, INPUT);
	//pinMode(batVoltagePin, INPUT);
	//pinMode(mpptCurrentPin, INPUT);
	//pinMode(motorCurrentPin, INPUT);
	//pinMode(potPin, INPUT);
}

void loop()
{
	int batTemperature;
	int motorTemperature;
	int batVoltage;
	int mpptCurrent; 
	int motorCurrent;
	int potVoltage;
	
	while(1)
	{
		// Read sensors
		batTemperature = analogRead(batTemperaturePin);
		motorTemperature = analogRead(motorTemperaturePin);
		batVoltage = analogRead(batVoltagePin);
		mpptCurrent = analogRead(mpptCurrentPin);
		motorCurrent = analogRead(motorCurrentPin);
		potVoltage = analogRead(potPin);
    
 		// Send data to tablet
		if (Serial.available())
		{
			byte c = Serial.read();
			// if an 0x01 is received, send a value
			if (c == 0x01)
			{
				Serial.write(c); // send ACK back   
				switch (sensor) 
				{
				case 0xA0: // Battery Temperature Sensor
					BluetoothSend(batTemperature);
					//BluetoothFake(); //FAKE DATA FOR DEBUG
					sensor ++;
					break;
				  
				case 0xA1: // Motor Temperature Sensor
					BluetoothSend(motorTemperature);
					//BluetoothFake(); //FAKE DATA FOR DEBUG
					sensor ++;
					break;
					
				case 0xA2: // Battery Voltage Sensor
					BluetoothSend(batVoltage);
					//BluetoothFake(); //FAKE DATA FOR DEBUG
					sensor ++;
					break;
					
				case 0xA3: // MPPT Current Sensor
					BluetoothSend(mpptCurrent);
					//BluetoothFake(); //FAKE DATA FOR DEBUG
					sensor ++;
					break;
					
				case 0xA4: // Motor Current Sensor
					BluetoothSend(motorCurrent);
					//BluetoothFake(); //FAKE DATA FOR DEBUG
					sensor ++;
					break;
					
				case 0xA5: // Potentiometer Voltage
					BluetoothSend(potVoltage);
					//BluetoothFake(); //FAKE DATA FOR DEBUG
					sensor = 0xA0;
					break;
				
				default: 
					sensor = 0xA0;
					break;
				}
			}
		}
	}
}

// Sends the value from adc as 2 bytes big endian (Big Endian por causa do JAVA de Android que trabalha em big endian)
void BluetoothSend(int value)
{
  Serial.write(0x02); // Start of transmission
  Serial.write(sensor);
  Serial.write(highByte(value));
  Serial.write(lowByte(value));
  Serial.write(0x03); // Start of transmission
}

// Fake data bluetooth
void BluetoothFake(void)
{
  Serial.write(0x02); // Start of transmission
  Serial.write(sensor);
  if(sensor == 0xA0)
  {
    // 30 Graus - Temperatura eletronica - 0x00CC
    Serial.write(0x00);
    Serial.write(0xCC);
  }
  if(sensor == 0xA1)
  {
    // 50 Graus - Temperatura motor - 0x0155
    Serial.write(0x01);
    Serial.write(0x55);
  }
  if(sensor == 0xA2)
  {
    // 24V - Tensao bateria - 0x0200
    Serial.write(0x02);
    Serial.write(0x00);
  }
  if(sensor == 0xA3)
  {
    // 30A Corrente MPPT - 0xCC
    Serial.write(0x00); 
    Serial.write(0xCC);
  }
  if(sensor == 0xA4)
  {
    // 45A Corrente motor - 0x0132
    Serial.write(0x01);
    Serial.write(0x32);
  }
  if(sensor == 0xA5)
  {
    // Potenciometro em 2V = 50% PWM 0x0199
    Serial.write(0x01);
    Serial.write(0x99);
  }
  
  Serial.write(0x03); // End of transmission 
}
