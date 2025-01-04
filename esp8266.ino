#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <DHT.h>

// Cấu hình Wi-Fi
#define WIFI_SSID "Victory"
#define WIFI_PASSWORD "012117764"

// Thông tin Firebase
#define FIREBASE_HOST "dht11-43ff2-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "oy5GxM6IzuC52aEGwJ5Sc4mAlpXUbpJ1wpnpb2lM"

// Cấu hình cảm biến DHT11
#define DHTPIN D5          // Chân dữ liệu của DHT11 
#define DHTTYPE DHT11      // Loại cảm biến: DHT11

DHT dht(DHTPIN, DHTTYPE);

// Khởi tạo Firebase và cấu hình
FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

void setup() {
  Serial.begin(115200);
  // Cấu hình chân cảm biến DHT11
  dht.begin();

  // Kết nối Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Đang kết nối Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nKết nối Wi-Fi thành công!");

  // Cấu hình Firebase
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;

  // Khởi động Firebase
  Firebase.begin(&config, &auth);

  // Kiểm tra kết nối Firebase
  if (Firebase.ready()) {
    Serial.println("Kết nối Firebase thành công!");
  } else {
    Serial.println("Kết nối Firebase thất bại!");
  }
}

void loop() {
  // Đọc nhiệt độ và độ ẩm từ DHT11
  float temperature = dht.readTemperature(); // Nhiệt độ (°C)
  float humidity = dht.readHumidity();       // Độ ẩm (%)

  // Kiểm tra nếu đọc dữ liệu bị lỗi
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Lỗi đọc dữ liệu từ DHT11!");
    return;
  }

  // Gửi dữ liệu lên Firebase
  String temperaturePath = "/sensors/temperature"; // Đường dẫn trong Firebase cho nhiệt độ
  String humidityPath = "/sensors/humidity";       // Đường dẫn trong Firebase cho độ ẩm

  if (Firebase.setFloat(firebaseData, temperaturePath, temperature)) {
    Serial.println("Gửi nhiệt độ thành công: " + String(temperature) + "°C");
  } else {
    Serial.println("Gửi nhiệt độ thất bại: " + firebaseData.errorReason());
  }

  if (Firebase.setFloat(firebaseData, humidityPath, humidity)) {
    Serial.println("Gửi độ ẩm thành công: " + String(humidity) + "%");
  } else {
    Serial.println("Gửi độ ẩm thất bại: " + firebaseData.errorReason());
  }
  Firebase.setString(firebaseData, "/test", "Hello Firebase!");
  // Chờ 5 giây trước khi đọc và gửi lại
  delay(5000);
}


