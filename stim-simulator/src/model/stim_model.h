#ifndef STIM_MODEL_H
#define STIM_MODEL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define STIM_CHANNEL_COUNT 4U
#define STIM_RECEIVER_COUNT 12U
#define STIM_PRESCRIPTION_COUNT 4U

typedef enum {
    STIM_STATE_UNCONFIGURED = 0,
    STIM_STATE_READY,
    STIM_STATE_RUNNING,
    STIM_STATE_PAUSED,
    STIM_STATE_OFFLINE
} stim_unit_state_t;

typedef enum {
    STIM_WAVE_SINE = 0,
    STIM_WAVE_SQUARE,
    STIM_WAVE_TRIANGLE
} stim_waveform_t;

typedef struct {
    char id;
    const char * prescription;
    uint32_t remaining_seconds;
    uint8_t intensity;
    stim_unit_state_t state;
    bool selected;
} stim_channel_t;

typedef struct {
    uint8_t id;
    const char * prescription;
    uint32_t remaining_seconds;
    uint8_t battery_percent;
    stim_unit_state_t state;
    bool selected;
    bool locked;
} stim_receiver_t;

typedef struct {
    stim_channel_t channels[STIM_CHANNEL_COUNT];
    stim_receiver_t receivers[STIM_RECEIVER_COUNT];
    size_t selected_prescription;
    stim_waveform_t medium_waveform;
    stim_waveform_t low_waveform;
    uint16_t medium_carrier_tenths_khz;
    uint16_t medium_modulation_hz;
    uint16_t medium_pulse_width_us;
    uint16_t medium_duration_min;
    uint16_t low_frequency_hz;
    uint16_t low_pulse_width_us;
    uint16_t low_duration_min;
    uint8_t low_config_count;
} stim_model_t;

extern const char * const stim_prescriptions[STIM_PRESCRIPTION_COUNT];

void stim_model_init(stim_model_t * model);
void stim_model_tick(stim_model_t * model);

bool stim_model_toggle_channel_selection(stim_model_t * model, size_t index);
bool stim_model_channel_start(stim_model_t * model, size_t index);
bool stim_model_channel_pause(stim_model_t * model, size_t index);
bool stim_model_channel_stop(stim_model_t * model, size_t index);
void stim_model_apply_medium(stim_model_t * model);

bool stim_model_toggle_receiver_selection(stim_model_t * model, size_t index);
void stim_model_select_all_online_receivers(stim_model_t * model, bool selected);
void stim_model_apply_low(stim_model_t * model);

void stim_model_select_prescription(stim_model_t * model, size_t index);

#endif

