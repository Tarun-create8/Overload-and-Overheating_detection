#include "acs723.h"

// Initialize ACS723
void ACS723_Init(ACS723_HandleTypeDef *handle, ADC_HandleTypeDef *hadc, uint32_t channel) {
    handle->hadc = hadc;
    handle->channel = channel;
    handle->voltage = ACS723_DEFAULT_VOLTAGE;
    handle->max_adc = ACS723_DEFAULT_MAX_ADC;
    handle->mv_per_amp = ACS723_DEFAULT_MV_PER_AMP;
    handle->midpoint = handle->max_adc / 2;
}

// Read DC current
float ACS723_ReadCurrentDC(ACS723_HandleTypeDef *handle, uint16_t samples) {
    uint32_t adc_sum = 0;
    for (uint16_t i = 0; i < samples; i++) {
        HAL_ADC_Start(handle->hadc);
        if (HAL_ADC_PollForConversion(handle->hadc, HAL_MAX_DELAY) == HAL_OK) {
            adc_sum += HAL_ADC_GetValue(handle->hadc);
        }
        HAL_ADC_Stop(handle->hadc);
    }

    uint32_t adc_avg = adc_sum / samples;
    int16_t adc_diff = adc_avg - handle->midpoint;

    // Convert ADC difference to current
    float mv_diff = (adc_diff * handle->voltage * 1000.0f) / handle->max_adc;
    return mv_diff / handle->mv_per_amp;
}

// Calibrate midpoint
void ACS723_CalibrateMidpoint(ACS723_HandleTypeDef *handle, uint16_t samples) {
    uint32_t adc_sum = 0;

    for (uint16_t i = 0; i < samples; i++) {
        HAL_ADC_Start(handle->hadc);
        if (HAL_ADC_PollForConversion(handle->hadc, HAL_MAX_DELAY) == HAL_OK) {
            adc_sum += HAL_ADC_GetValue(handle->hadc);
        }
        HAL_ADC_Stop(handle->hadc);
    }

    handle->midpoint = adc_sum / samples;
}
