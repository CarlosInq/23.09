#define LED  2
#define BOTAO 0
#include <WiFi.h>
//#include "queue.h"

TaskHandle_t th_chave;
TaskHandle_t th_serial;

extern "C" {
  uint8_t temprature_sens_read();
}

//hw_timer_t *Timer0_Cfg = NULL;

void UART_RX_IRQ() {
  // enqueue(t_serial);
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  vTaskNotifyGiveFromISR( th_serial, &xHigherPriorityTaskWoken );
  portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void IRAM_ATTR isr() {
  //enqueue(t_ch_boot);
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  vTaskNotifyGiveFromISR( th_chave, &xHigherPriorityTaskWoken );
  portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void IRAM_ATTR Timer0_ISR()
{
  //enqueue(t_timer);
}

static void task_hall(void *pvParameters) {

  const TickType_t xFrequency = 2000 / portTICK_PERIOD_MS;

  TickType_t     xLastWakeTime = xTaskGetTickCount();

  while (1) {
    vTaskDelayUntil( &xLastWakeTime, xFrequency );
    Serial.print("sensor hall =");
    Serial.println(hallRead());
  }
  vTaskDelete(NULL);
}

static void task_temperatura(void *pvParameters) {
  //setup
  //loop
  const TickType_t xFrequency = 5000 / portTICK_PERIOD_MS;
  TickType_t xLastWakeTime = xTaskGetTickCount();
  while (1) {
    vTaskDelayUntil( &xLastWakeTime, xFrequency );
    int valor = temprature_sens_read();
    if (valor != 128) { //invalido se 128
      Serial.println((valor - 32) / 1.8);
    }
  }
  vTaskDelete(NULL);
}

static void task_chave(void *pvParameters) {

  pinMode(BOTAO, INPUT);
  attachInterrupt(0, isr, FALLING);

  while (1) {
    ulTaskNotifyTake( pdTRUE, portMAX_DELAY );
    Serial.println("Botao acionado !!!");


  }
  vTaskDelete(NULL);
}

static void task_serial(void *pvParameters) {

  pinMode(LED, OUTPUT);

  Serial.onReceive(UART_RX_IRQ);

  while (1) {
    ulTaskNotifyTake( pdTRUE, portMAX_DELAY );

    switch (Serial.read()) {
      case 'l':
        digitalWrite(LED, HIGH);
        break;
      case'd':
        digitalWrite(LED, LOW);
        break;
    }
    if (Serial.available()) {
      xTaskNotifyGive(th_serial);
    }
  }
  vTaskDelete(NULL);
}

void setup() {

  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  //pinMode(LED, OUTPUT);
  //pinMode(BOTAO, INPUT);

  //Serial.onReceive(UART_RX_IRQ);

  //attachInterrupt(0, isr, FALLING);

  //Timer0_Cfg = timerBegin(0, 80, true);
  //timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR, true);
  //timerAlarmWrite(Timer0_Cfg, 1000000, true);
  //timerAlarmEnable(Timer0_Cfg);

  xTaskCreate(task_hall, "task hal", 4096, NULL, 1, NULL);
  xTaskCreate(task_temperatura, "task temperatura", 4096, NULL, 5, NULL);
  xTaskCreate(task_chave, "task chave", 4096, NULL, 8, &th_chave);
  xTaskCreate(task_serial, "task serial", 4096, NULL, 6, &th_serial);

}

void loop() {

}
