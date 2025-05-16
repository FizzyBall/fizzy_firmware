#include "motor.h"
#include "driver/ledc.h"
#include "driver/gpio.h"

#define MOTOR_PWM   GPIO_NUM_33

#define SAT_MAX (1.0)
#define SAT(x) (((x) > SAT_MAX) ? SAT_MAX : ((-(x) > SAT_MAX) ? (-SAT_MAX) : (x)))

static float m_speed_local = 0.0;


void motor_set_speed(float m_speed) {
    ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, (uint32_t) (9830.25 + (SAT(m_speed) * 4000)));
    ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);
    m_speed_local = SAT(m_speed);
}

float motor_get_speed(void) {
    return m_speed_local;
}

void motor_init(void) {
    //init for PWM signal, needed for all versions
    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_16_BIT,
        .freq_hz = 100,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    ledc_timer_config(&ledc_timer);
    ledc_channel_config_t ledc_channel = {
        .channel = LEDC_CHANNEL_0,
        .duty = 0,
        .gpio_num = MOTOR_PWM,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_TIMER_0,
    };
    ledc_channel_config(&ledc_channel);
    // set motor to zero position
    motor_set_speed(0.0);
 }