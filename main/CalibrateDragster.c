#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_rom_gpio.h"

adc_oneshot_unit_handle_t adc1_handle;

// Canal ADC dos sensores
#define S1_CHANNEL ADC_CHANNEL_6
#define S2_CHANNEL ADC_CHANNEL_7
#define S3_CHANNEL ADC_CHANNEL_4
#define S4_CHANNEL ADC_CHANNEL_5

void app_main(void)
{
    // Inicializar ADC
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_1
    };
    adc_oneshot_new_unit(&init_config, &adc1_handle);

    adc_oneshot_chan_cfg_t config = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten = ADC_ATTEN_DB_12
    };

    adc_oneshot_config_channel(adc1_handle, S1_CHANNEL, &config);
    adc_oneshot_config_channel(adc1_handle, S2_CHANNEL, &config);
    adc_oneshot_config_channel(adc1_handle, S3_CHANNEL, &config);
    adc_oneshot_config_channel(adc1_handle, S4_CHANNEL, &config);

    // Variáveis
    int s1_raw, s2_raw, s3_raw, s4_raw;
    int min_s1 = 4095, max_s1 = 0;
    int min_s2 = 4095, max_s2 = 0;
    int min_s3 = 4095, max_s3 = 0;
    int min_s4 = 4095, max_s4 = 0;

    printf("=== CALIBRACAO SENSORES ===\n");
    printf("Move o Dragster lentamente sobre a linha e o fundo...\n");

    uint32_t inicio = xTaskGetTickCount();
    const int DURACAO_CALIB_MS = 10000;   // 3 segundos

    // -----------------------
    // CICLO DE CALIBRAÇÃO
    // -----------------------
    while (xTaskGetTickCount() - inicio < pdMS_TO_TICKS(DURACAO_CALIB_MS))
    {
        adc_oneshot_read(adc1_handle, S1_CHANNEL, &s1_raw);
        adc_oneshot_read(adc1_handle, S2_CHANNEL, &s2_raw);
        adc_oneshot_read(adc1_handle, S3_CHANNEL, &s3_raw);
        adc_oneshot_read(adc1_handle, S4_CHANNEL, &s4_raw);

        if(s1_raw < min_s1) min_s1 = s1_raw;
        if(s1_raw > max_s1) max_s1 = s1_raw;

        if(s2_raw < min_s2) min_s2 = s2_raw;
        if(s2_raw > max_s2) max_s2 = s2_raw;

        if(s3_raw < min_s3) min_s3 = s3_raw;
        if(s3_raw > max_s3) max_s3 = s3_raw;

        if(s4_raw < min_s4) min_s4 = s4_raw;
        if(s4_raw > max_s4) max_s4 = s4_raw;

        printf("S1 raw=%4d min=%4d max=%4d | S2 raw=%4d min=%4d max=%4d\n",
               s1_raw, min_s1, max_s1, s2_raw, min_s2, max_s2);
        printf("S3 raw=%4d min=%4d max=%4d | S4 raw=%4d min=%4d max=%4d\n\n",
               s3_raw, min_s3, max_s3, s4_raw, min_s4, max_s4);

        vTaskDelay(pdMS_TO_TICKS(100));
    }

    // -----------------------
    // RESULTADOS FINAIS
    // -----------------------
    printf("\n=== RESULTADOS FINAIS DA CALIBRACAO ===\n");
    printf("S1: min=%4d  max=%4d\n", min_s1, max_s1);
    printf("S2: min=%4d  max=%4d\n", min_s2, max_s2);
    printf("S3: min=%4d  max=%4d\n", min_s3, max_s3);
    printf("S4: min=%4d  max=%4d\n", min_s4, max_s4);
    printf("=======================================\n");

    printf("Calibração concluída! Reinicia para correr o programa.\n");

    // Impede que continue (opcional)
    while(1) vTaskDelay(pdMS_TO_TICKS(1000));
}