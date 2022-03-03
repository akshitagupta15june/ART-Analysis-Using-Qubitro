#include <QubitroMqttClient.h>
#include <ESP8266WiFi.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
// WiFi Client
#define SensorPin A0
WiFiClient wifiClient;
// Qubitro Client
QubitroMqttClient mqttClient(wifiClient);
// Device Parameters
char deviceID[] = "a4b6dd7f-be6e-4b92-b5c6-658f0aa65ed0";
char deviceToken[] = "rqlg14R0n3oIfi-Ieu8NzF46zRyggwUIdm2igfk$";
// WiFi Parameters
const char* ssid = "Zuluxia";
const char* password = "akshita1506";
const int MPU_addr=0x68;
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
int minVal=265;
int maxVal=402;
double x;
double y;
double z;
Adafruit_MPU6050 mpu;
void setup() {
// Initialize the serial port
  serial_init();
  // Initialize wireless connectivity
  wifi_init();
  // Initialize Qubitro
  qubitro_init();
Wire.begin();
Wire.beginTransmission(MPU_addr);
Wire.write(0x6B);
Wire.write(0);
Wire.endTransmission(true);
  while (!Serial)
    delay(10); 
  Serial.println("Adafruit MPU6050 test!");
  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }
  Serial.println("");
  delay(100);
}
void loop() {
Wire.beginTransmission(MPU_addr);
Wire.write(0x3B);
Wire.endTransmission(false);
Wire.requestFrom(MPU_addr,14,true);
AcX=Wire.read()<<8|Wire.read();
AcY=Wire.read()<<8|Wire.read();
AcZ=Wire.read()<<8|Wire.read();
int xAng = map(AcX,minVal,maxVal,-90,90);
int yAng = map(AcY,minVal,maxVal,-90,90);
int zAng = map(AcZ,minVal,maxVal,-90,90);
x= RAD_TO_DEG * (atan2(-yAng, -zAng)+PI);
y= RAD_TO_DEG * (atan2(-xAng, -zAng)+PI);
z= RAD_TO_DEG * (atan2(-yAng, -xAng)+PI);
Serial.print("AngleX= ");
Serial.println(x);
Serial.print("AngleY= ");
Serial.println(y);
Serial.print("AngleZ= ");
Serial.println(z);
  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  /* Print out the values */
  Serial.print("Acceleration X: ");
  Serial.print(a.acceleration.x);
  Serial.print(", Y: ");
  Serial.print(a.acceleration.y);
  Serial.print(", Z: ");
  Serial.print(a.acceleration.z);
  Serial.println(" m/s^2");
  Serial.print("Rotation X: ");
  Serial.print(g.gyro.x);
  Serial.print(", Y: ");
  Serial.print(g.gyro.y);
  Serial.print(", Z: ");
  Serial.print(g.gyro.z);
  Serial.println(" rad/s");
  Serial.print("Temperature: ");
  Serial.print(temp.temperature);
  Serial.println(" degC");
  Serial.println("");
  delay(500);
  String payload = "{\"AngleX\": " + String(x) + ",\"AngleY\":" + String(y) + ",\"AngleZ\":" + String(z) + ",\"Temperature: \":" + String(temp.temperature) +  ",\"Rotation X:\":" + String(g.gyro.x) +  ",\"Rotation Y:\":" + String(g.gyro.y) +  ",\"Rotation Z:\":" + String(g.gyro.z)+ ",\"Acceleration X :\":" + String(a.acceleration.x)+ ",\"Acceleration Y:\":" + String(a.acceleration.y)+ ",\"Acceleration Z:\":" + String(a.acceleration.z)+"}";
  mqttClient.poll();
  mqttClient.beginMessage(deviceID);
  mqttClient.print(payload);
  mqttClient.endMessage();
  // Delay
  delay(30 * 1000);
}
// Initialization code
void serial_init() {
  // Initiate serial port connection
  Serial.begin(115200);
  // Delay for stabilization
  delay(200);
}
void wifi_init() {
  // Set WiFi mode
  WiFi.mode(WIFI_STA);
  // Disconnect WiFi
  WiFi.disconnect();
  delay(100);
  // Initiate WiFi connection
  WiFi.begin(ssid, password);
  // Print connectivity status to the terminal
  Serial.print("Connecting to WiFi...");
  while(true)
  {
    delay(1000);
    Serial.print(".");
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("");
      Serial.println("WiFi Connected.");
      Serial.print("Local IP: ");
      Serial.println(WiFi.localIP());
      Serial.print("RSSI: ");
      Serial.println(WiFi.RSSI());
      break;
    }
  }
}
void qubitro_init() {
  char host[] = "broker.qubitro.com";
  int port = 1883;
  mqttClient.setId(deviceID);
  mqttClient.setDeviceIdToken(deviceID, deviceToken);
  Serial.println("Connecting to Qubitro...");
  if (!mqttClient.connect(host, port))
  {
    Serial.print("Connection failed. Error code: ");
    Serial.println(mqttClient.connectError());
    Serial.println("Visit docs.qubitro.com or create a new issue on github.com/qubitro");
  }
  Serial.println("Connected to Qubitro.");
  mqttClient.subscribe(deviceID);
}
