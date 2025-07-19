#include "Buttons.h"
#include "PageManager.h"
#include "WebApp.h"
#include "Encoder.h"
#include "ESPNOW.h"
#include "PID.h"
#include "PWM.h"
#include "MPU6050.h"
#include "EdgeAI.h"

PageManager LCD;
WebApp Server;
ESPNOW EspNow;
Encoder MyEncoder;
MPU6050 MPU6050_r(AD01);
MPU6050 MPU6050_l(AD00);
PWM MotorRvs(R_EN_PIN, R_PWM_PIN, R_PWM_CHANNEL);
PWM MotorFwd(L_EN_PIN, L_PWM_PIN, L_PWM_CHANNEL);
PID PID_ate(15, 0, 0);
PID PID_pse(15, 0.1, 0);
EdgeAI myAI;

#define WINDOW_SIZE 5

uint8_t level = 0, phase = 0, lastPhase = 0;
int curIdx = 0;
bool windowReady = false;
float input_window[5][7];
float levelSpeed[] = { 5, 10, 15, 20, 25 };
bool spPhase = false;
Mode mode = NULL_MODE;

TaskHandle_t xBrakeToggleHandle;
TaskHandle_t xLCDTaskHandle;
TaskHandle_t xServerTaskHandle;
TaskHandle_t xRunTaskHandle;

void brakeToggleTask(void* parameter) {
  vTaskSuspend(NULL);

  while (1) {
    PWM::onBTS();
    vTaskDelay(pdMS_TO_TICKS(10));

    PWM::offBTS();
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void lcdTask(void* pvParameters) {
  while (1) {
    if (LCD.modeRunning()) {
      mode = LCD.getMode();
      if (mode == FREE) {
        level = 0;
        curIdx = 0;
        windowReady = false;
      }

      else if (mode == ACTIVE) {
        level = LCD.getDetailMode().charAt(LCD.getDetailMode().length() - 1) - '0';
        Serial.printf("Level: %d \n", level);
        vTaskResume(xBrakeToggleHandle);
      }

      else if (mode == PASSIVE) {
        level = LCD.getDetailMode().charAt(LCD.getDetailMode().length() - 1) - '0';
        Serial.printf("Level: %d \n", level);
        PID_pse.setpoint = levelSpeed[level - 1];
      }

      vTaskResume(xRunTaskHandle);
      vTaskSuspend(xLCDTaskHandle);
    }

    else if (LCD.regisProcessing()) {
      Server.start();
      vTaskResume(xServerTaskHandle);
      vTaskSuspend(xLCDTaskHandle);
    }

    if (Buttons::checkChange) {
      Buttons::checkChange = false;
      LCD.goNextOpt();
    } else if (Buttons::checkConfirm) {
      Buttons::checkConfirm = false;
      LCD.goNextPage();
    } else if (Buttons::checkStop) {
      Buttons::checkStop = false;
      if (LCD.modeRunning())
        LCD.goPausePage();
      else
        LCD.goMenuPage();
    }

    LCD.showOpt();

    vTaskDelay(pdMS_TO_TICKS(1));
  }
}

void serverTask(void* pvParameters) {
  vTaskSuspend(NULL);

  while (1) {
    if (Buttons::checkStop) {
      Server.stop();
      vTaskResume(xLCDTaskHandle);
      vTaskSuspend(xServerTaskHandle);
    }

    Server.handleClient();

    if (Server.login) {
      Server.login = false;
      LCD.regisID(Server.userID, Server.today);

      vTaskResume(xLCDTaskHandle);
      Server.stop();
      vTaskSuspend(xServerTaskHandle);
    }

    vTaskDelay(pdMS_TO_TICKS(1));
  }
}

void runTask(void* pvParameters) {
  vTaskSuspend(NULL);

  TickType_t xLastWakeTime = xTaskGetTickCount();

  float rpm = 0.0, moment = 0.0, errThres = 0.5;
  int16_t PWM_signal = 0;

  while (1) {
    if (Buttons::checkStop) {

      if (mode == ACTIVE) {
        vTaskSuspend(xBrakeToggleHandle);
      } else if (mode == PASSIVE) {
        PID_pse.setpoint = 0;
      } else if (mode == FREE) {
        PID_ate.setpoint = 0;
      }

      PWM::offBTS();

      vTaskResume(xLCDTaskHandle);
      vTaskSuspend(xRunTaskHandle);
    }

    rpm = (float)(MyEncoder.encoderPulse) / 50.0 / PULSE_PER_RND * 60000.0 / TRANS_RATIO;
    MyEncoder.encoderPulse = 0;

    switch (mode) {
      case PASSIVE:
        if (fabs(rpm - PID_pse.setpoint) <= errThres) rpm = PID_pse.setpoint;
        Serial.println(rpm);

        PWM_signal = PID_pse.compute(rpm);
        if (PWM_signal >= 0) {
          MotorFwd.setPWM(1, PWM_signal);
          MotorRvs.setPWM(1, 0);
        } else {
          MotorFwd.setPWM(1, 0);
          MotorRvs.setPWM(1, -PWM_signal);
        }

        break;

      case ACTIVE:
      case FREE:
        EspNow.sendRequest();

        if (EspNow.receivedFlag) {
          moment = EspNow.receivedVal;
          EspNow.receivedFlag = false;
        }

        if (mode == ACTIVE) {
          Serial.println(moment);
          if (int(rpm) != 0) {
            PID_ate.setpoint = -rpm * level * 0.1;
            PWM_signal = -PID_ate.compute(moment) * 5;
            MotorRvs.setPWM(1, PWM_signal);
          } else MotorRvs.setPWM(1, 0);
        }

        else if (mode == FREE) {
          MPU6050_r.readAccel();
          MPU6050_r.readGyro();
          MPU6050_l.readAccel();
          MPU6050_l.readGyro();

          for (int i = 0; i < WINDOW_SIZE - 1; i++) {
            for (int j = 0; j < 7; j++) {
              input_window[i][j] = input_window[i + 1][j];
            }
          }

          input_window[WINDOW_SIZE - 1][0] = MPU6050_r.accelY;
          input_window[WINDOW_SIZE - 1][1] = MPU6050_r.accelZ;
          input_window[WINDOW_SIZE - 1][2] = MPU6050_r.gyroX;
          input_window[WINDOW_SIZE - 1][3] = MPU6050_l.accelY;
          input_window[WINDOW_SIZE - 1][4] = MPU6050_l.accelZ;
          input_window[WINDOW_SIZE - 1][5] = MPU6050_l.gyroX;
          input_window[WINDOW_SIZE - 1][6] = moment;

          Serial.printf("%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\n",
                        input_window[WINDOW_SIZE - 1][0],
                        input_window[WINDOW_SIZE - 1][1],
                        input_window[WINDOW_SIZE - 1][2],
                        input_window[WINDOW_SIZE - 1][3],
                        input_window[WINDOW_SIZE - 1][4],
                        input_window[WINDOW_SIZE - 1][5],
                        input_window[WINDOW_SIZE - 1][6]);

          if (curIdx < WINDOW_SIZE) curIdx++;
          if (curIdx >= WINDOW_SIZE) windowReady = true;

          if (windowReady) {
            myAI.setInput(input_window);
            myAI.compute();
            phase = myAI.getOutput();
            Serial.println(phase);
            Serial.println(lastPhase);
          }

          if (phase == 3 && lastPhase == 1) phase = 2;

          spPhase = (phase == 2 || phase == 4);

          if (int(rpm) != 0) {
            if (spPhase) {
              MotorFwd.setPWM(1, 356);
              MotorRvs.setPWM(1, 0);
            } else {
              MotorFwd.setPWM(1, 256);
              MotorRvs.setPWM(1, 0);
            }
          } else {
            MotorFwd.setPWM(1, 156);
            MotorRvs.setPWM(1, 0);
          }

          lastPhase = phase;
        }
        break;
    }

    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(50));
  }
}

void setup() {
  Serial.begin(115200);

  CountTime::begin();
  Buttons::begin();
  LCD.begin();

  WiFi.mode(WIFI_OFF);
  Server.begin();

  MyEncoder.setInterrupt();

  MPU6050_r.begin();
  MPU6050_l.begin();

  MotorRvs.initPWM();
  MotorFwd.initPWM();

  myAI.init();

  xTaskCreatePinnedToCore(
    brakeToggleTask,
    "Brake toggle task",
    1024,
    NULL,
    2,
    &xBrakeToggleHandle,
    tskNO_AFFINITY);

  xTaskCreatePinnedToCore(
    lcdTask,
    "LCD Task",
    4096,
    NULL,
    4,
    &xLCDTaskHandle,
    tskNO_AFFINITY);

  xTaskCreatePinnedToCore(
    serverTask,
    "Server Task",
    9216,
    NULL,
    3,
    &xServerTaskHandle,
    tskNO_AFFINITY);

  xTaskCreatePinnedToCore(
    runTask,
    "Run Task",
    9216,
    NULL,
    3,
    &xRunTaskHandle,
    tskNO_AFFINITY);
}

void loop() {
}
