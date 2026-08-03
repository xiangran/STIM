#include "model/stim_model.h"

#include <assert.h>

const char * const stim_prescriptions[STIM_PRESCRIPTION_COUNT] = {
    "肩颈放松",
    "腰部镇痛",
    "膝关节康复",
    "自定义处方"
};

static void tick_unit(stim_unit_state_t * state, uint32_t * remaining_seconds)
{
    if((*state != STIM_STATE_RUNNING) || (*remaining_seconds == 0U)) {
        return;
    }

    --(*remaining_seconds);
    if(*remaining_seconds == 0U) {
        *state = STIM_STATE_READY;
    }
}

void stim_model_init(stim_model_t * model)
{
    size_t index;

    assert(model != NULL);

    *model = (stim_model_t) {0};
    model->selected_prescription = 0U;
    model->medium_waveform = STIM_WAVE_SINE;
    model->low_waveform = STIM_WAVE_SINE;
    model->medium_carrier_tenths_khz = 50U;
    model->medium_modulation_hz = 80U;
    model->medium_pulse_width_us = 250U;
    model->medium_duration_min = 20U;
    model->low_frequency_hz = 80U;
    model->low_pulse_width_us = 250U;
    model->low_duration_min = 20U;
    model->low_config_count = 4U;

    for(index = 0U; index < STIM_CHANNEL_COUNT; ++index) {
        model->channels[index].id = (char)('A' + (char)index);
        model->channels[index].state = STIM_STATE_UNCONFIGURED;
    }

    model->channels[0].prescription = stim_prescriptions[0];
    model->channels[0].remaining_seconds = 20U * 60U;
    model->channels[0].intensity = 12U;
    model->channels[0].state = STIM_STATE_READY;
    model->channels[0].selected = true;

    model->channels[2].prescription = stim_prescriptions[1];
    model->channels[2].remaining_seconds = (12U * 60U) + 36U;
    model->channels[2].intensity = 8U;
    model->channels[2].state = STIM_STATE_RUNNING;

    for(index = 0U; index < STIM_RECEIVER_COUNT; ++index) {
        stim_receiver_t * receiver = &model->receivers[index];
        receiver->id = (uint8_t)(index + 1U);
        receiver->battery_percent = (uint8_t)(92U - (index * 3U));
        receiver->state = STIM_STATE_UNCONFIGURED;
    }

    for(index = 0U; index < 4U; ++index) {
        stim_receiver_t * receiver = &model->receivers[index];
        receiver->prescription = stim_prescriptions[0];
        receiver->remaining_seconds = 20U * 60U;
        receiver->state = STIM_STATE_READY;
        receiver->selected = true;
        receiver->locked = true;
    }

    model->receivers[4].prescription = stim_prescriptions[1];
    model->receivers[4].remaining_seconds = (12U * 60U) + 36U;
    model->receivers[4].state = STIM_STATE_RUNNING;

    model->receivers[10].battery_percent = 0U;
    model->receivers[10].state = STIM_STATE_OFFLINE;
    model->receivers[11].battery_percent = 0U;
    model->receivers[11].state = STIM_STATE_OFFLINE;
}

void stim_model_tick(stim_model_t * model)
{
    size_t index;

    assert(model != NULL);
    for(index = 0U; index < STIM_CHANNEL_COUNT; ++index) {
        tick_unit(&model->channels[index].state, &model->channels[index].remaining_seconds);
    }
    for(index = 0U; index < STIM_RECEIVER_COUNT; ++index) {
        tick_unit(&model->receivers[index].state, &model->receivers[index].remaining_seconds);
    }
}

bool stim_model_toggle_channel_selection(stim_model_t * model, size_t index)
{
    assert(model != NULL);
    if(index >= STIM_CHANNEL_COUNT) {
        return false;
    }
    model->channels[index].selected = !model->channels[index].selected;
    return true;
}

bool stim_model_channel_start(stim_model_t * model, size_t index)
{
    stim_channel_t * channel;

    assert(model != NULL);
    if(index >= STIM_CHANNEL_COUNT) {
        return false;
    }
    channel = &model->channels[index];
    if((channel->state != STIM_STATE_READY) && (channel->state != STIM_STATE_PAUSED)) {
        return false;
    }
    channel->state = STIM_STATE_RUNNING;
    return true;
}

bool stim_model_channel_pause(stim_model_t * model, size_t index)
{
    stim_channel_t * channel;

    assert(model != NULL);
    if(index >= STIM_CHANNEL_COUNT) {
        return false;
    }
    channel = &model->channels[index];
    if(channel->state == STIM_STATE_RUNNING) {
        channel->state = STIM_STATE_PAUSED;
        return true;
    }
    if(channel->state == STIM_STATE_PAUSED) {
        channel->state = STIM_STATE_RUNNING;
        return true;
    }
    return false;
}

bool stim_model_channel_stop(stim_model_t * model, size_t index)
{
    stim_channel_t * channel;

    assert(model != NULL);
    if(index >= STIM_CHANNEL_COUNT) {
        return false;
    }
    channel = &model->channels[index];
    if((channel->state != STIM_STATE_RUNNING) && (channel->state != STIM_STATE_PAUSED)) {
        return false;
    }
    channel->state = STIM_STATE_READY;
    channel->remaining_seconds = (uint32_t)model->medium_duration_min * 60U;
    return true;
}

void stim_model_apply_medium(stim_model_t * model)
{
    size_t index;

    assert(model != NULL);
    for(index = 0U; index < STIM_CHANNEL_COUNT; ++index) {
        stim_channel_t * channel = &model->channels[index];
        if(!channel->selected || (channel->state == STIM_STATE_RUNNING)) {
            continue;
        }
        channel->prescription = stim_prescriptions[model->selected_prescription];
        channel->remaining_seconds = (uint32_t)model->medium_duration_min * 60U;
        if(channel->intensity == 0U) {
            channel->intensity = 8U;
        }
        channel->state = STIM_STATE_READY;
    }
}

bool stim_model_toggle_receiver_selection(stim_model_t * model, size_t index)
{
    stim_receiver_t * receiver;

    assert(model != NULL);
    if(index >= STIM_RECEIVER_COUNT) {
        return false;
    }
    receiver = &model->receivers[index];
    if(receiver->state == STIM_STATE_OFFLINE) {
        return false;
    }
    receiver->selected = !receiver->selected;
    return true;
}

void stim_model_select_all_online_receivers(stim_model_t * model, bool selected)
{
    size_t index;

    assert(model != NULL);
    for(index = 0U; index < STIM_RECEIVER_COUNT; ++index) {
        if(model->receivers[index].state != STIM_STATE_OFFLINE) {
            model->receivers[index].selected = selected;
        }
    }
}

void stim_model_apply_low(stim_model_t * model)
{
    size_t index;

    assert(model != NULL);
    for(index = 0U; index < STIM_RECEIVER_COUNT; ++index) {
        stim_receiver_t * receiver = &model->receivers[index];
        if(!receiver->selected || (receiver->state == STIM_STATE_OFFLINE) ||
           (receiver->state == STIM_STATE_RUNNING)) {
            continue;
        }
        receiver->prescription = stim_prescriptions[model->selected_prescription];
        receiver->remaining_seconds = (uint32_t)model->low_duration_min * 60U;
        receiver->state = STIM_STATE_READY;
    }
}

void stim_model_select_prescription(stim_model_t * model, size_t index)
{
    assert(model != NULL);
    if(index < STIM_PRESCRIPTION_COUNT) {
        model->selected_prescription = index;
    }
}

