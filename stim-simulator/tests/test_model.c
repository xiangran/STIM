#include "model/stim_model.h"

#include <assert.h>
#include <stdio.h>

int main(void)
{
    stim_model_t model;

    stim_model_init(&model);
    assert(model.channels[0].state == STIM_STATE_READY);
    assert(model.channels[2].state == STIM_STATE_RUNNING);
    assert(model.receivers[10].state == STIM_STATE_OFFLINE);

    assert(stim_model_channel_start(&model, 0U));
    assert(model.channels[0].state == STIM_STATE_RUNNING);
    stim_model_tick(&model);
    assert(model.channels[0].remaining_seconds == 1199U);
    assert(stim_model_channel_pause(&model, 0U));
    assert(model.channels[0].state == STIM_STATE_PAUSED);
    assert(stim_model_channel_pause(&model, 0U));
    assert(stim_model_channel_stop(&model, 0U));
    assert(model.channels[0].state == STIM_STATE_READY);

    assert(!stim_model_toggle_receiver_selection(&model, 10U));
    stim_model_select_all_online_receivers(&model, true);
    assert(model.receivers[9].selected);
    assert(!model.receivers[10].selected);

    stim_model_select_prescription(&model, 2U);
    stim_model_apply_low(&model);
    assert(model.receivers[9].state == STIM_STATE_READY);
    assert(model.receivers[9].prescription == stim_prescriptions[2]);

    puts("stim model tests passed");
    return 0;
}

