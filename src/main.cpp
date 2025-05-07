#include <Arduino.h>
// 카메라 라이브러리 추가
#include <esp_camera.h>
#include <WiFi.h>
#include <Secrets.h> // 비밀번호를 포함한 비밀 정보 파일
#include <CameraPins.h> // 핀 번호를 포함한 카메라 핀 설정 파일
#include "mbedtls/base64.h" // Base64 라이브러리 추가
#include <base64.h> // Base64 라이브러리 추가

//Wi-Fi 연결 정보
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

void Camera_init_config();
bool initWiFi();
void get_photo();

void setup() {
  // 시리얼 통신 설정
  Serial.begin(115200);
  delay(1000);
  // 카메라 초기화
  Camera_init_config();
  // Wi-Fi 연결 설정
  initWiFi();
  // 카메라 LED 핀 설정
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, HIGH); // LED 끄기
}

void loop() {
  // 카메라 프레임 캡처
  get_photo();
}

// init camera
void Camera_init_config(){
  camera_config_t config;
  // camera clock 
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  // camera hardware configuration
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  // configuration for streaming
  //config.frame_size = FRAMESIZE_UXGA; 
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_LATEST;
  // if PSRAM is present, init with UXGA resolution and higher JPEG quality
  // for larger pre-allocated frame buffer.
  if(psramFound()){
      config.frame_size = FRAMESIZE_UXGA;
      config.fb_location = CAMERA_FB_IN_PSRAM;
      config.jpeg_quality = 8;
      config.fb_count = 2;   
  }
  else {
      // Limit the frame size when PSRAM is not available
      config.frame_size = FRAMESIZE_QVGA;
      config.fb_location = CAMERA_FB_IN_DRAM;
      config.jpeg_quality = 12;
      config.fb_count = 1; 
  }

  // initialize camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
      Serial.printf("Camera init failed with error 0x%x", err);
      return;
  }
  else{
      Serial.printf("Camera init successfully!\n");
  }
  // configure camera sensor
  sensor_t * s = esp_camera_sensor_get();
  if(config.pixel_format == PIXFORMAT_JPEG){
      // drop down frame size for higher initial frame rate
      s->set_framesize(s, FRAMESIZE_QVGA);
      // image setup
      s->set_aec2(s,1);
      s->set_aec_value(s,168);
      s->set_agc_gain(s,5);
      s->set_hmirror(s,1);
      // register
      //s->set_reg(s, 0x111, 0x80, 0x80);
  }
}

// connect to wifi
bool initWiFi(){
  // web server as Station mode:
  // we can  request information from the internet 
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // if (WiFi.waitForConnectResult() != WL_CONNECTED) {
  //   //Serial.printf("WiFi Failed!\n");
  //   delay(500);
  //   Serial.print(".");
  // }
  // else{
  //   Serial.print("WiFi connected succesfully!");
  //   Serial.println(WiFi.localIP());
  // }
  return true;
}

void get_photo(){
  // 카메라 프레임 캡처
  camera_fb_t *fb = NULL;
  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("사진촬영 실패");
    esp_camera_fb_return(fb);
    return;
  }
  else {
    Serial.println("사진촬영 성공");
  }
  
  // JPEG 데이터 전송 (예: HTTP 서버에 업로드)
  // 여기에 JPEG 데이터를 전송하는 코드를 추가하세요.
  size_t size = fb->len; // JPEG 데이터 크기
  uint8_t *image = (uint8_t*)fb->buf; // JPEG 데이터 포인터(주소?)
  
  // Base64 인코딩
  int base64_size = (size +2 - ((size + 2) % 3)) / 3 * 4 + 1; // Base64 인코딩된 데이터 크기
  uint8_t *buffer = (uint8_t*)calloc(base64_size, sizeof(char)); // Base64 인코딩된 데이터 포인터
  size_t buffer_size = 0; // 변환된 사이즈를 저장할 변수

  //image의 데이터를 Base64로 인코딩
  int err = mbedtls_base64_encode(buffer, base64_size, &buffer_size, image, size);
  
  if (err != 0) { // Base64 인코딩 실패
    Serial.printf("Base64 인코딩 실패: %d\n", err);
  }
  else { // Base64 인코딩 성공
    String base64_code = "data:image/jpeg;base64,"; // Base64 인코딩된 데이터를 문자열로 변환
    base64_code = base64_code + String((char*)buffer); // Base64 인코딩된 데이터 추가
    
    Serial.println(base64_code); // Base64 인코딩된 데이터 출력
  }
  // 프레임 버퍼 해제
  esp_camera_fb_return(fb);
  
  //delay(1000); // 1초 대기
}