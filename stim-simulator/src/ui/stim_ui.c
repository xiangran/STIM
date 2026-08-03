#include "ui/stim_ui.h"

#include "stim_fonts.h"

#include <stdint.h>
#include <stdio.h>

#define STIM_COLOR_BG 0xFBF8F3U
#define STIM_COLOR_CARD 0xFFFFFFU
#define STIM_COLOR_NAVY 0x5B8DEFU
#define STIM_COLOR_BLUE 0x5B8DEFU
#define STIM_COLOR_CYAN 0x16A6C9U
#define STIM_COLOR_TEAL 0x2BB3AEU
#define STIM_COLOR_CORAL 0xF2685CU
#define STIM_COLOR_AMBER 0xF0A857U
#define STIM_COLOR_TEXT 0x33302BU
#define STIM_COLOR_MUTED 0x8C8478U
#define STIM_COLOR_BORDER 0xF1E9DCU
#define STIM_COLOR_DISABLED 0xF2ECE0U
#define STIM_COLOR_SELECTED 0xEAF0FEU
#define STIM_COLOR_RUNNING_SOFT 0xE4F5F3U
#define STIM_COLOR_GRAD_START 0x22C1C3U
#define STIM_COLOR_GRAD_END 0x5B8DEFU
#define STIM_COLOR_SHADOW 0x8C6E46U

#define STIM_HEADER_HEIGHT 64
#define STIM_PAGE_TOP 64
#define STIM_PAGE_HEIGHT 736
#define STIM_PANEL_RADIUS 20
#define STIM_METRIC_RADIUS 12
#define STIM_TOUCH_MIN 44

typedef struct {
    lv_obj_t * card;
    lv_obj_t * header;
    lv_obj_t * title_label;
    lv_obj_t * state_label;
    lv_obj_t * prescription_label;
    lv_obj_t * time_label;
    lv_obj_t * intensity_label;
    lv_obj_t * start_button;
    lv_obj_t * pause_button;
    lv_obj_t * pause_label;
    lv_obj_t * stop_button;
} channel_view_t;

typedef struct {
    lv_obj_t * card;
    lv_obj_t * id_label;
    lv_obj_t * selection_label;
    lv_obj_t * link_label;
    lv_obj_t * prescription_label;
    lv_obj_t * time_label;
    lv_obj_t * state_label;
} receiver_view_t;

typedef struct {
    lv_obj_t * slider;
    lv_obj_t * value_label;
} parameter_view_t;

typedef struct {
    stim_model_t * model;
    stim_screen_t visible_screen;
    lv_obj_t * medium_tab;
    lv_obj_t * low_tab;
    lv_obj_t * medium_page;
    lv_obj_t * low_page;
    channel_view_t channels[STIM_CHANNEL_COUNT];
    receiver_view_t receivers[STIM_RECEIVER_COUNT];
    lv_obj_t * prescription_rows[2][STIM_PRESCRIPTION_COUNT];
    lv_obj_t * wave_buttons[2][3];
    parameter_view_t parameters[2][4];
} ui_context_t;

static ui_context_t ui;

static lv_color_t color(uint32_t value)
{
    return lv_color_hex(value);
}

static void no_scroll(lv_obj_t * object)
{
    lv_obj_remove_flag(object, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_scrollbar_mode(object, LV_SCROLLBAR_MODE_OFF);
}

static lv_obj_t * make_plain(lv_obj_t * parent)
{
    lv_obj_t * object = lv_obj_create(parent);

    lv_obj_set_style_bg_opa(object, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(object, 0, 0);
    lv_obj_set_style_radius(object, 0, 0);
    lv_obj_set_style_pad_all(object, 0, 0);
    no_scroll(object);
    return object;
}

static lv_obj_t * make_panel(lv_obj_t * parent)
{
    lv_obj_t * panel = lv_obj_create(parent);

    lv_obj_set_style_bg_color(panel, color(STIM_COLOR_CARD), 0);
    lv_obj_set_style_bg_opa(panel, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(panel, color(STIM_COLOR_BORDER), 0);
    lv_obj_set_style_border_width(panel, 1, 0);
    lv_obj_set_style_radius(panel, STIM_PANEL_RADIUS, 0);
    lv_obj_set_style_pad_all(panel, 0, 0);
    lv_obj_set_style_shadow_width(panel, 20, 0);
    lv_obj_set_style_shadow_offset_y(panel, 6, 0);
    lv_obj_set_style_shadow_spread(panel, 0, 0);
    lv_obj_set_style_shadow_color(panel, color(STIM_COLOR_SHADOW), 0);
    lv_obj_set_style_shadow_opa(panel, LV_OPA_10, 0);
    no_scroll(panel);
    return panel;
}

static lv_obj_t * make_label(lv_obj_t * parent,
                             const char * text,
                             const lv_font_t * font,
                             uint32_t text_color)
{
    lv_obj_t * label = lv_label_create(parent);

    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_style_text_color(label, color(text_color), 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_DOT);
    return label;
}

static void apply_button_style(lv_obj_t * button, bool primary)
{
    lv_obj_set_style_radius(button, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(button, primary ? 0 : 1, 0);
    lv_obj_set_style_border_color(button, color(STIM_COLOR_BORDER), 0);
    lv_obj_set_style_bg_color(button, color(primary ? STIM_COLOR_GRAD_START : STIM_COLOR_CARD), 0);
    if(primary) {
        lv_obj_set_style_bg_grad_color(button, color(STIM_COLOR_GRAD_END), 0);
        lv_obj_set_style_bg_grad_dir(button, LV_GRAD_DIR_HOR, 0);
    }
    lv_obj_set_style_bg_opa(button, LV_OPA_80, LV_STATE_PRESSED);
    lv_obj_set_style_pad_all(button, 6, 0);
    lv_obj_set_style_shadow_width(button, 0, 0);
    no_scroll(button);
}

static lv_obj_t * make_button(lv_obj_t * parent,
                              const char * text,
                              bool primary,
                              lv_obj_t ** label_out)
{
    lv_obj_t * button = lv_button_create(parent);
    lv_obj_t * label;

    apply_button_style(button, primary);
    lv_obj_set_style_text_color(button, color(primary ? 0xFFFFFFU : STIM_COLOR_TEXT), 0);

    label = make_label(button, text, &stim_font_16, primary ? 0xFFFFFFU : STIM_COLOR_TEXT);
    lv_obj_center(label);
    if(label_out != NULL) {
        *label_out = label;
    }
    return button;
}

static lv_obj_t * make_section_header(lv_obj_t * parent, const char * title)
{
    lv_obj_t * header = make_plain(parent);

    lv_obj_set_size(header, LV_PCT(100), 44);
    lv_obj_set_style_pad_left(header, 16, 0);
    lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    (void)make_label(header, title, &stim_font_20, STIM_COLOR_TEXT);
    return header;
}

static void set_button_disabled(lv_obj_t * button, bool disabled)
{
    if(disabled) {
        lv_obj_add_state(button, LV_STATE_DISABLED);
    }
    else {
        lv_obj_remove_state(button, LV_STATE_DISABLED);
    }
}

static void format_time(char * buffer, size_t length, uint32_t seconds)
{
    (void)snprintf(buffer, length, "%02lu:%02lu",
                   (unsigned long)(seconds / 60U),
                   (unsigned long)(seconds % 60U));
}

static const char * unit_state_text(stim_unit_state_t state)
{
    switch(state) {
        case STIM_STATE_READY:
            return "就绪";
        case STIM_STATE_RUNNING:
            return "治疗中";
        case STIM_STATE_PAUSED:
            return "已暂停";
        case STIM_STATE_OFFLINE:
            return "离线";
        case STIM_STATE_UNCONFIGURED:
        default:
            return "未配置";
    }
}

static void refresh_channel(size_t index)
{
    stim_channel_t * channel = &ui.model->channels[index];
    channel_view_t * view = &ui.channels[index];
    char buffer[32];
    uint32_t header_color = STIM_COLOR_NAVY;
    bool configured = channel->state != STIM_STATE_UNCONFIGURED;
    bool active = (channel->state == STIM_STATE_RUNNING) || (channel->state == STIM_STATE_PAUSED);

    if(channel->state == STIM_STATE_RUNNING) {
        header_color = STIM_COLOR_TEAL;
    }
    else if(channel->state == STIM_STATE_PAUSED) {
        header_color = STIM_COLOR_AMBER;
    }
    else if(channel->selected) {
        header_color = STIM_COLOR_BLUE;
    }

    lv_obj_set_style_bg_color(view->header, color(header_color), 0);
    lv_obj_set_style_border_width(view->card, channel->selected ? 2 : 1, 0);
    lv_obj_set_style_border_color(view->card,
                                  color(channel->selected ? STIM_COLOR_BLUE : STIM_COLOR_BORDER),
                                  0);
    lv_obj_set_style_bg_color(view->card,
                              color(channel->selected ? STIM_COLOR_SELECTED : STIM_COLOR_CARD),
                              0);

    (void)snprintf(buffer, sizeof(buffer), "%c通道", channel->id);
    lv_label_set_text(view->title_label, buffer);
    lv_label_set_text(view->state_label, unit_state_text(channel->state));
    lv_label_set_text(view->prescription_label,
                      configured && (channel->prescription != NULL) ? channel->prescription : "未配置");

    if(configured) {
        format_time(buffer, sizeof(buffer), channel->remaining_seconds);
        lv_label_set_text(view->time_label, buffer);
        (void)snprintf(buffer, sizeof(buffer), "%02u", channel->intensity);
        lv_label_set_text(view->intensity_label, buffer);
    }
    else {
        lv_label_set_text(view->time_label, "--:--");
        lv_label_set_text(view->intensity_label, "--");
    }

    set_button_disabled(view->start_button,
                        (channel->state != STIM_STATE_READY) && (channel->state != STIM_STATE_PAUSED));
    set_button_disabled(view->pause_button, !active);
    set_button_disabled(view->stop_button, !active);
    lv_label_set_text(view->pause_label, channel->state == STIM_STATE_PAUSED ? "继续" : "暂停");
}

static void refresh_medium(void)
{
    size_t index;

    for(index = 0U; index < STIM_CHANNEL_COUNT; ++index) {
        refresh_channel(index);
    }
}

static void refresh_receiver(size_t index)
{
    stim_receiver_t * receiver = &ui.model->receivers[index];
    receiver_view_t * view = &ui.receivers[index];
    char buffer[48];
    uint32_t border_color = STIM_COLOR_BORDER;
    uint32_t background = STIM_COLOR_CARD;
    bool configured = (receiver->state != STIM_STATE_UNCONFIGURED) &&
                      (receiver->state != STIM_STATE_OFFLINE);

    if(receiver->state == STIM_STATE_RUNNING) {
        border_color = STIM_COLOR_TEAL;
        background = 0xEAFBF9U;
    }
    else if(receiver->selected) {
        border_color = STIM_COLOR_BLUE;
        background = STIM_COLOR_SELECTED;
    }
    else if(receiver->state == STIM_STATE_OFFLINE) {
        background = STIM_COLOR_DISABLED;
    }

    lv_obj_set_style_border_color(view->card, color(border_color), 0);
    lv_obj_set_style_border_width(view->card, receiver->selected ? 2 : 1, 0);
    lv_obj_set_style_bg_color(view->card, color(background), 0);
    lv_obj_set_style_text_opa(view->card,
                              receiver->state == STIM_STATE_OFFLINE ? LV_OPA_60 : LV_OPA_COVER,
                              0);

    (void)snprintf(buffer, sizeof(buffer), "%02u号", receiver->id);
    lv_label_set_text(view->id_label, buffer);
    lv_label_set_text(view->selection_label, receiver->selected ? "已选" : "");

    if(receiver->state == STIM_STATE_OFFLINE) {
        lv_label_set_text(view->link_label, "无线 --  电量 --");
        lv_label_set_text(view->prescription_label, "离线");
        lv_label_set_text(view->time_label, "--:--");
    }
    else {
        (void)snprintf(buffer, sizeof(buffer), "无线在线  电量 %u%%", receiver->battery_percent);
        lv_label_set_text(view->link_label, buffer);
        lv_label_set_text(view->prescription_label,
                          configured && (receiver->prescription != NULL) ?
                          receiver->prescription : "待配置");
        if(configured) {
            format_time(buffer, sizeof(buffer), receiver->remaining_seconds);
            lv_label_set_text(view->time_label, buffer);
        }
        else {
            lv_label_set_text(view->time_label, "--:--");
        }
    }

    if(receiver->locked && configured) {
        lv_label_set_text(view->state_label, "已配置  锁定");
    }
    else {
        lv_label_set_text(view->state_label, unit_state_text(receiver->state));
    }
}

static void refresh_low(void)
{
    size_t index;

    for(index = 0U; index < STIM_RECEIVER_COUNT; ++index) {
        refresh_receiver(index);
    }
}

static int32_t parameter_min(stim_screen_t screen, size_t parameter)
{
    static const int32_t medium_min[4] = {35, 35, 50, 1};
    static const int32_t low_min[4] = {35, 50, 1, 1};
    return screen == STIM_SCREEN_MEDIUM ? medium_min[parameter] : low_min[parameter];
}

static int32_t parameter_max(stim_screen_t screen, size_t parameter)
{
    static const int32_t medium_max[4] = {100, 150, 500, 180};
    static const int32_t low_max[4] = {150, 500, 180, 12};
    return screen == STIM_SCREEN_MEDIUM ? medium_max[parameter] : low_max[parameter];
}

static int32_t parameter_step(stim_screen_t screen, size_t parameter)
{
    static const int32_t medium_step[4] = {5, 5, 10, 1};
    static const int32_t low_step[4] = {5, 10, 1, 1};
    return screen == STIM_SCREEN_MEDIUM ? medium_step[parameter] : low_step[parameter];
}

static int32_t get_parameter(stim_screen_t screen, size_t parameter)
{
    if(screen == STIM_SCREEN_MEDIUM) {
        switch(parameter) {
            case 0U: return ui.model->medium_carrier_tenths_khz;
            case 1U: return ui.model->medium_modulation_hz;
            case 2U: return ui.model->medium_pulse_width_us;
            default: return ui.model->medium_duration_min;
        }
    }

    switch(parameter) {
        case 0U: return ui.model->low_frequency_hz;
        case 1U: return ui.model->low_pulse_width_us;
        case 2U: return ui.model->low_duration_min;
        default: return ui.model->low_config_count;
    }
}

static void set_parameter(stim_screen_t screen, size_t parameter, int32_t value)
{
    if(value < parameter_min(screen, parameter)) {
        value = parameter_min(screen, parameter);
    }
    if(value > parameter_max(screen, parameter)) {
        value = parameter_max(screen, parameter);
    }

    if(screen == STIM_SCREEN_MEDIUM) {
        switch(parameter) {
            case 0U: ui.model->medium_carrier_tenths_khz = (uint16_t)value; break;
            case 1U: ui.model->medium_modulation_hz = (uint16_t)value; break;
            case 2U: ui.model->medium_pulse_width_us = (uint16_t)value; break;
            default: ui.model->medium_duration_min = (uint16_t)value; break;
        }
    }
    else {
        switch(parameter) {
            case 0U: ui.model->low_frequency_hz = (uint16_t)value; break;
            case 1U: ui.model->low_pulse_width_us = (uint16_t)value; break;
            case 2U: ui.model->low_duration_min = (uint16_t)value; break;
            default: ui.model->low_config_count = (uint8_t)value; break;
        }
    }
}

static void format_parameter(stim_screen_t screen, size_t parameter, char * buffer, size_t length)
{
    int32_t value = get_parameter(screen, parameter);

    if((screen == STIM_SCREEN_MEDIUM) && (parameter == 0U)) {
        (void)snprintf(buffer, length, "%ld.%ld kHz", (long)(value / 10), (long)(value % 10));
    }
    else if(((screen == STIM_SCREEN_MEDIUM) && (parameter == 1U)) ||
            ((screen == STIM_SCREEN_LOW) && (parameter == 0U))) {
        (void)snprintf(buffer, length, "%ld Hz", (long)value);
    }
    else if(((screen == STIM_SCREEN_MEDIUM) && (parameter == 2U)) ||
            ((screen == STIM_SCREEN_LOW) && (parameter == 1U))) {
        (void)snprintf(buffer, length, "%ld us", (long)value);
    }
    else if(((screen == STIM_SCREEN_MEDIUM) && (parameter == 3U)) ||
            ((screen == STIM_SCREEN_LOW) && (parameter == 2U))) {
        (void)snprintf(buffer, length, "%ld min", (long)value);
    }
    else {
        (void)snprintf(buffer, length, "%ld 台", (long)value);
    }
}

static void refresh_parameters(stim_screen_t screen)
{
    size_t index;
    char buffer[32];
    stim_waveform_t waveform = screen == STIM_SCREEN_MEDIUM ?
                               ui.model->medium_waveform : ui.model->low_waveform;

    for(index = 0U; index < 4U; ++index) {
        int32_t value = get_parameter(screen, index);
        lv_slider_set_value(ui.parameters[screen][index].slider, value, LV_ANIM_OFF);
        format_parameter(screen, index, buffer, sizeof(buffer));
        lv_label_set_text(ui.parameters[screen][index].value_label, buffer);
    }

    for(index = 0U; index < 3U; ++index) {
        bool selected = index == (size_t)waveform;
        lv_obj_set_style_bg_color(ui.wave_buttons[screen][index],
                                  color(selected ? STIM_COLOR_BLUE : STIM_COLOR_DISABLED), 0);
        lv_obj_set_style_text_color(ui.wave_buttons[screen][index],
                                    color(selected ? 0xFFFFFFU : STIM_COLOR_TEXT), 0);
        lv_obj_t * label = lv_obj_get_child(ui.wave_buttons[screen][index], 0);
        lv_obj_set_style_text_color(label, color(selected ? 0xFFFFFFU : STIM_COLOR_TEXT), 0);
    }
}

static void refresh_prescriptions(void)
{
    size_t screen;
    size_t index;

    for(screen = 0U; screen < 2U; ++screen) {
        for(index = 0U; index < STIM_PRESCRIPTION_COUNT; ++index) {
            bool selected = index == ui.model->selected_prescription;
            lv_obj_set_style_bg_color(ui.prescription_rows[screen][index],
                                      color(selected ? STIM_COLOR_SELECTED : STIM_COLOR_CARD), 0);
            lv_obj_set_style_border_color(ui.prescription_rows[screen][index],
                                          color(selected ? STIM_COLOR_BLUE : STIM_COLOR_BORDER), 0);
        }
    }
}

static void refresh_all(void)
{
    refresh_medium();
    refresh_low();
    refresh_parameters(STIM_SCREEN_MEDIUM);
    refresh_parameters(STIM_SCREEN_LOW);
    refresh_prescriptions();
}

static void modal_show(const char * title, const char * text)
{
    lv_obj_t * message = lv_msgbox_create(NULL);

    lv_obj_set_width(message, 520);
    lv_obj_set_style_text_font(message, &stim_font_16, 0);
    lv_obj_set_style_bg_color(message, color(STIM_COLOR_CARD), 0);
    lv_msgbox_add_title(message, title);
    lv_msgbox_add_text(message, text);
    lv_msgbox_add_close_button(message);
}

static void tab_event(lv_event_t * event)
{
    stim_screen_t screen = (stim_screen_t)(uintptr_t)lv_event_get_user_data(event);
    stim_ui_show_screen(screen);
}

static void settings_event(lv_event_t * event)
{
    (void)event;
    modal_show("系统设置", "模拟器设置页将在下一迭代接入。当前参数仅用于界面演示。");
}

static void channel_card_event(lv_event_t * event)
{
    size_t index = (size_t)(uintptr_t)lv_event_get_user_data(event);
    (void)stim_model_toggle_channel_selection(ui.model, index);
    refresh_medium();
}

static void channel_control_event(lv_event_t * event)
{
    uintptr_t code = (uintptr_t)lv_event_get_user_data(event);
    size_t index = (size_t)(code >> 4U);
    uint8_t action = (uint8_t)(code & 0x0FU);

    if(action == 0U) {
        (void)stim_model_channel_start(ui.model, index);
    }
    else if(action == 1U) {
        (void)stim_model_channel_pause(ui.model, index);
    }
    else {
        (void)stim_model_channel_stop(ui.model, index);
    }
    refresh_medium();
}

static void receiver_event(lv_event_t * event)
{
    size_t index = (size_t)(uintptr_t)lv_event_get_user_data(event);
    (void)stim_model_toggle_receiver_selection(ui.model, index);
    refresh_low();
}

static void select_all_event(lv_event_t * event)
{
    size_t index;
    bool all_selected = true;

    (void)event;
    for(index = 0U; index < STIM_RECEIVER_COUNT; ++index) {
        const stim_receiver_t * receiver = &ui.model->receivers[index];
        if((receiver->state != STIM_STATE_OFFLINE) && !receiver->selected) {
            all_selected = false;
            break;
        }
    }
    stim_model_select_all_online_receivers(ui.model, !all_selected);
    refresh_low();
}

static void prescription_event(lv_event_t * event)
{
    size_t index = (size_t)(uintptr_t)lv_event_get_user_data(event);
    stim_model_select_prescription(ui.model, index);
    refresh_prescriptions();
}

static void wave_event(lv_event_t * event)
{
    uintptr_t code = (uintptr_t)lv_event_get_user_data(event);
    stim_screen_t screen = (stim_screen_t)(code >> 4U);
    stim_waveform_t waveform = (stim_waveform_t)(code & 0x0FU);

    if(screen == STIM_SCREEN_MEDIUM) {
        ui.model->medium_waveform = waveform;
    }
    else {
        ui.model->low_waveform = waveform;
    }
    refresh_parameters(screen);
}

static void parameter_button_event(lv_event_t * event)
{
    uintptr_t code = (uintptr_t)lv_event_get_user_data(event);
    stim_screen_t screen = (stim_screen_t)((code >> 8U) & 0x01U);
    size_t parameter = (size_t)((code >> 4U) & 0x0FU);
    bool increment = (code & 0x01U) != 0U;
    int32_t value = get_parameter(screen, parameter);
    int32_t step = parameter_step(screen, parameter);

    set_parameter(screen, parameter, value + (increment ? step : -step));
    refresh_parameters(screen);
}

static void parameter_slider_event(lv_event_t * event)
{
    uintptr_t code = (uintptr_t)lv_event_get_user_data(event);
    stim_screen_t screen = (stim_screen_t)((code >> 4U) & 0x01U);
    size_t parameter = (size_t)(code & 0x0FU);
    lv_obj_t * slider = lv_event_get_target_obj(event);

    set_parameter(screen, parameter, lv_slider_get_value(slider));
    refresh_parameters(screen);
}

static void apply_medium_event(lv_event_t * event)
{
    (void)event;
    stim_model_apply_medium(ui.model);
    refresh_medium();
}

static void apply_low_event(lv_event_t * event)
{
    (void)event;
    stim_model_apply_low(ui.model);
    refresh_low();
}

static void placement_event(lv_event_t * event)
{
    (void)event;
    modal_show("贴敷位置", "演示入口：肩颈、腰部和膝关节贴敷引导页。");
}

static void timer_event(lv_timer_t * timer)
{
    (void)timer;
    stim_model_tick(ui.model);
    refresh_medium();
    refresh_low();
}

static void create_header(lv_obj_t * screen)
{
    lv_obj_t * header = make_plain(screen);
    lv_obj_t * spacer;
    lv_obj_t * tabs;
    lv_obj_t * right;
    lv_obj_t * settings;

    lv_obj_set_size(header, LV_PCT(100), STIM_HEADER_HEIGHT);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(header, color(STIM_COLOR_CARD), 0);
    lv_obj_set_style_bg_opa(header, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(header, color(STIM_COLOR_BORDER), 0);
    lv_obj_set_style_border_width(header, 0, 0);
    lv_obj_set_style_border_side(header, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_pad_hor(header, 16, 0);
    lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    spacer = make_plain(header);
    lv_obj_set_size(spacer, 205, 1);

    tabs = make_plain(header);
    lv_obj_set_size(tabs, 520, LV_PCT(100));
    lv_obj_set_flex_flow(tabs, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(tabs, 8, 0);

    ui.medium_tab = make_button(tabs, "中频治疗", false, NULL);
    lv_obj_set_height(ui.medium_tab, LV_PCT(100));
    lv_obj_set_flex_grow(ui.medium_tab, 1);
    lv_obj_set_style_text_font(ui.medium_tab, &stim_font_24, 0);
    lv_obj_add_event_cb(ui.medium_tab, tab_event, LV_EVENT_CLICKED, (void *)(uintptr_t)STIM_SCREEN_MEDIUM);
    lv_obj_set_style_text_font(lv_obj_get_child(ui.medium_tab, 0), &stim_font_24, 0);

    ui.low_tab = make_button(tabs, "低频治疗", false, NULL);
    lv_obj_set_height(ui.low_tab, LV_PCT(100));
    lv_obj_set_flex_grow(ui.low_tab, 1);
    lv_obj_add_event_cb(ui.low_tab, tab_event, LV_EVENT_CLICKED, (void *)(uintptr_t)STIM_SCREEN_LOW);
    lv_obj_set_style_text_font(lv_obj_get_child(ui.low_tab, 0), &stim_font_24, 0);

    right = make_plain(header);
    lv_obj_set_size(right, 205, LV_PCT(100));
    lv_obj_set_flex_flow(right, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(right, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(right, 16, 0);
    (void)make_label(right, "14:28", &lv_font_montserrat_24, STIM_COLOR_TEXT);

    settings = make_button(right, "设置", false, NULL);
    lv_obj_set_size(settings, 72, 44);
    lv_obj_add_event_cb(settings, settings_event, LV_EVENT_CLICKED, NULL);
}

static void create_metric(lv_obj_t * parent,
                          const char * caption,
                          lv_obj_t ** value_label,
                          bool time_value)
{
    lv_obj_t * box = make_panel(parent);

    lv_obj_set_height(box, LV_PCT(100));
    lv_obj_set_flex_grow(box, 1);
    lv_obj_set_style_radius(box, 8, 0);
    lv_obj_set_style_pad_all(box, 8, 0);
    lv_obj_set_flex_flow(box, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(box, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    (void)make_label(box, caption, &stim_font_16, STIM_COLOR_MUTED);
    *value_label = make_label(box, time_value ? "--:--" : "--",
                              &lv_font_montserrat_28, STIM_COLOR_NAVY);
}

static void create_channel_card(lv_obj_t * parent, size_t index)
{
    channel_view_t * view = &ui.channels[index];
    lv_obj_t * body;
    lv_obj_t * metrics;
    lv_obj_t * controls;
    uintptr_t base = (uintptr_t)(index << 4U);

    view->card = make_panel(parent);
    lv_obj_set_width(view->card, 1);
    lv_obj_set_height(view->card, LV_PCT(100));
    lv_obj_set_flex_grow(view->card, 1);
    lv_obj_set_flex_flow(view->card, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_flag(view->card, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(view->card, channel_card_event, LV_EVENT_CLICKED, (void *)(uintptr_t)index);

    view->header = make_plain(view->card);
    lv_obj_set_size(view->header, LV_PCT(100), 48);
    lv_obj_set_style_bg_opa(view->header, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(view->header, STIM_PANEL_RADIUS, 0);
    lv_obj_set_style_pad_hor(view->header, 12, 0);
    lv_obj_set_flex_flow(view->header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(view->header, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    view->title_label = make_label(view->header, "A通道", &stim_font_20, 0xFFFFFFU);
    view->state_label = make_label(view->header, "就绪", &stim_font_16, 0xFFFFFFU);

    body = make_plain(view->card);
    lv_obj_set_width(body, LV_PCT(100));
    lv_obj_set_flex_grow(body, 1);
    lv_obj_set_style_pad_all(body, 10, 0);
    lv_obj_set_style_pad_row(body, 8, 0);
    lv_obj_set_flex_flow(body, LV_FLEX_FLOW_COLUMN);

    view->prescription_label = make_label(body, "未配置", &stim_font_20, STIM_COLOR_TEXT);
    lv_obj_set_width(view->prescription_label, LV_PCT(100));

    metrics = make_plain(body);
    lv_obj_set_size(metrics, LV_PCT(100), 82);
    lv_obj_set_flex_flow(metrics, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(metrics, 8, 0);
    create_metric(metrics, "剩余时间", &view->time_label, true);
    create_metric(metrics, "强度", &view->intensity_label, false);

    controls = make_plain(body);
    lv_obj_set_size(controls, LV_PCT(100), 50);
    lv_obj_set_flex_flow(controls, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(controls, 6, 0);

    view->start_button = make_button(controls, "开始", false, NULL);
    lv_obj_set_height(view->start_button, LV_PCT(100));
    lv_obj_set_flex_grow(view->start_button, 1);
    lv_obj_add_event_cb(view->start_button, channel_control_event, LV_EVENT_CLICKED,
                        (void *)(base | 0U));

    view->pause_button = make_button(controls, "暂停", false, &view->pause_label);
    lv_obj_set_height(view->pause_button, LV_PCT(100));
    lv_obj_set_flex_grow(view->pause_button, 1);
    lv_obj_add_event_cb(view->pause_button, channel_control_event, LV_EVENT_CLICKED,
                        (void *)(base | 1U));

    view->stop_button = make_button(controls, "停止", false, NULL);
    lv_obj_set_height(view->stop_button, LV_PCT(100));
    lv_obj_set_flex_grow(view->stop_button, 1);
    lv_obj_set_style_border_color(view->stop_button, color(STIM_COLOR_CORAL), 0);
    lv_obj_add_event_cb(view->stop_button, channel_control_event, LV_EVENT_CLICKED,
                        (void *)(base | 2U));
}

static lv_obj_t * create_prescription_row(lv_obj_t * parent, size_t screen, size_t index, bool compact)
{
    lv_obj_t * row = make_plain(parent);
    lv_obj_t * badge;
    char number[8];

    lv_obj_set_size(row, LV_PCT(100), compact ? 42 : 48);
    lv_obj_set_style_bg_color(row, color(STIM_COLOR_CARD), 0);
    lv_obj_set_style_bg_opa(row, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(row, color(STIM_COLOR_BORDER), 0);
    lv_obj_set_style_border_width(row, 1, 0);
    lv_obj_set_style_radius(row, 8, 0);
    lv_obj_set_style_pad_hor(row, 8, 0);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(row, 12, 0);
    lv_obj_add_flag(row, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(row, prescription_event, LV_EVENT_CLICKED, (void *)(uintptr_t)index);

    badge = make_plain(row);
    lv_obj_set_size(badge, 48, compact ? 30 : 34);
    lv_obj_set_style_bg_color(badge, color(STIM_COLOR_BLUE), 0);
    lv_obj_set_style_bg_opa(badge, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(badge, 6, 0);
    (void)snprintf(number, sizeof(number), "%02u", (unsigned)(index + 1U));
    lv_obj_t * badge_label = make_label(badge, number, &lv_font_montserrat_20, 0xFFFFFFU);
    lv_obj_center(badge_label);

    lv_obj_t * text = make_label(row, stim_prescriptions[index], &stim_font_16, STIM_COLOR_TEXT);
    lv_obj_set_flex_grow(text, 1);
    ui.prescription_rows[screen][index] = row;
    return row;
}

static lv_obj_t * create_prescription_panel(lv_obj_t * parent, stim_screen_t screen, bool compact)
{
    lv_obj_t * panel = make_panel(parent);
    lv_obj_t * content;
    size_t index;

    lv_obj_set_width(panel, 420);
    lv_obj_set_height(panel, LV_PCT(100));
    lv_obj_set_flex_flow(panel, LV_FLEX_FLOW_COLUMN);
    (void)make_section_header(panel, "处方");

    content = make_plain(panel);
    lv_obj_set_width(content, LV_PCT(100));
    lv_obj_set_flex_grow(content, 1);
    lv_obj_set_style_pad_all(content, compact ? 8 : 10, 0);
    lv_obj_set_style_pad_row(content, compact ? 6 : 8, 0);
    lv_obj_set_flex_flow(content, LV_FLEX_FLOW_COLUMN);

    for(index = 0U; index < STIM_PRESCRIPTION_COUNT; ++index) {
        (void)create_prescription_row(content, (size_t)screen, index, compact);
    }
    return panel;
}

static lv_obj_t * create_wave_row(lv_obj_t * parent, stim_screen_t screen, bool compact)
{
    static const char * const wave_names[3] = {"正弦波", "方波", "三角波"};
    lv_obj_t * row = make_plain(parent);
    size_t index;

    lv_obj_set_size(row, LV_PCT(100), compact ? 42 : 48);
    lv_obj_set_style_pad_hor(row, 12, 0);
    lv_obj_set_style_pad_column(row, 8, 0);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t * title = make_label(row, "波形", &stim_font_16, STIM_COLOR_MUTED);
    lv_obj_set_width(title, compact ? 110 : 120);

    for(index = 0U; index < 3U; ++index) {
        uintptr_t code = ((uintptr_t)screen << 4U) | index;
        ui.wave_buttons[screen][index] = make_button(row, wave_names[index], false, NULL);
        lv_obj_set_height(ui.wave_buttons[screen][index], compact ? 34 : 38);
        lv_obj_set_flex_grow(ui.wave_buttons[screen][index], 1);
        lv_obj_add_event_cb(ui.wave_buttons[screen][index], wave_event, LV_EVENT_CLICKED,
                            (void *)code);
    }
    return row;
}

static void create_parameter_row(lv_obj_t * parent,
                                 stim_screen_t screen,
                                 size_t parameter,
                                 const char * title,
                                 bool compact)
{
    lv_obj_t * row = make_plain(parent);
    lv_obj_t * title_label;
    lv_obj_t * minus;
    lv_obj_t * plus;
    uintptr_t base = ((uintptr_t)screen << 8U) | ((uintptr_t)parameter << 4U);
    uintptr_t slider_code = ((uintptr_t)screen << 4U) | (uintptr_t)parameter;

    lv_obj_set_size(row, LV_PCT(100), compact ? 40 : 46);
    lv_obj_set_style_pad_hor(row, 12, 0);
    lv_obj_set_style_pad_column(row, 8, 0);
    lv_obj_set_style_border_color(row, color(STIM_COLOR_BORDER), 0);
    lv_obj_set_style_border_width(row, 0, 0);
    lv_obj_set_style_border_side(row, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    title_label = make_label(row, title, &stim_font_16, STIM_COLOR_MUTED);
    lv_obj_set_width(title_label, compact ? 110 : 120);

    minus = make_button(row, "-", false, NULL);
    lv_obj_set_size(minus, compact ? 36 : 40, compact ? 32 : 36);
    lv_obj_set_style_text_font(lv_obj_get_child(minus, 0), &lv_font_montserrat_20, 0);
    lv_obj_add_event_cb(minus, parameter_button_event, LV_EVENT_CLICKED, (void *)base);

    ui.parameters[screen][parameter].slider = lv_slider_create(row);
    lv_obj_set_height(ui.parameters[screen][parameter].slider, 12);
    lv_obj_set_flex_grow(ui.parameters[screen][parameter].slider, 1);
    lv_slider_set_range(ui.parameters[screen][parameter].slider,
                        parameter_min(screen, parameter), parameter_max(screen, parameter));
    lv_obj_set_style_bg_color(ui.parameters[screen][parameter].slider,
                              color(STIM_COLOR_DISABLED), LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui.parameters[screen][parameter].slider,
                              color(STIM_COLOR_BLUE), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(ui.parameters[screen][parameter].slider,
                              color(STIM_COLOR_BLUE), LV_PART_KNOB);
    lv_obj_set_style_pad_all(ui.parameters[screen][parameter].slider, 4, LV_PART_KNOB);
    lv_obj_add_event_cb(ui.parameters[screen][parameter].slider, parameter_slider_event,
                        LV_EVENT_VALUE_CHANGED, (void *)slider_code);

    ui.parameters[screen][parameter].value_label =
        make_label(row, "--", &stim_font_16, STIM_COLOR_NAVY);
    lv_obj_set_width(ui.parameters[screen][parameter].value_label, compact ? 90 : 100);
    lv_obj_set_style_text_align(ui.parameters[screen][parameter].value_label, LV_TEXT_ALIGN_RIGHT, 0);

    plus = make_button(row, "+", false, NULL);
    lv_obj_set_size(plus, compact ? 36 : 40, compact ? 32 : 36);
    lv_obj_set_style_text_font(lv_obj_get_child(plus, 0), &lv_font_montserrat_20, 0);
    lv_obj_add_event_cb(plus, parameter_button_event, LV_EVENT_CLICKED, (void *)(base | 1U));
}

static lv_obj_t * create_parameter_panel(lv_obj_t * parent, stim_screen_t screen, bool compact)
{
    static const char * const medium_titles[4] = {"载波频率", "调制频率", "脉宽", "治疗时间"};
    static const char * const low_titles[4] = {"频率", "脉宽", "治疗时间", "配置数量"};
    const char * const * titles = screen == STIM_SCREEN_MEDIUM ? medium_titles : low_titles;
    lv_obj_t * panel = make_panel(parent);
    lv_obj_t * action_row;
    lv_obj_t * apply;
    size_t index;

    lv_obj_set_width(panel, 1);
    lv_obj_set_height(panel, LV_PCT(100));
    lv_obj_set_flex_grow(panel, 1);
    lv_obj_set_flex_flow(panel, LV_FLEX_FLOW_COLUMN);
    (void)make_section_header(panel, screen == STIM_SCREEN_MEDIUM ? "处方参数" : "批量配置");
    (void)create_wave_row(panel, screen, compact);

    for(index = 0U; index < 4U; ++index) {
        create_parameter_row(panel, screen, index, titles[index], compact);
    }

    action_row = make_plain(panel);
    lv_obj_set_width(action_row, LV_PCT(100));
    lv_obj_set_flex_grow(action_row, 1);
    lv_obj_set_style_pad_all(action_row, compact ? 8 : 10, 0);
    lv_obj_set_style_pad_column(action_row, 12, 0);
    lv_obj_set_flex_flow(action_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(action_row, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    if(screen == STIM_SCREEN_LOW) {
        lv_obj_t * placement = make_button(action_row, "查看贴敷位置", false, NULL);
        lv_obj_set_size(placement, 250, compact ? 42 : 48);
        lv_obj_add_event_cb(placement, placement_event, LV_EVENT_CLICKED, NULL);
    }

    apply = make_button(action_row,
                        screen == STIM_SCREEN_MEDIUM ? "应用到所选通道" : "配置到所选单元",
                        true, NULL);
    lv_obj_set_height(apply, compact ? 42 : 48);
    lv_obj_set_flex_grow(apply, 1);
    lv_obj_add_event_cb(apply,
                        screen == STIM_SCREEN_MEDIUM ? apply_medium_event : apply_low_event,
                        LV_EVENT_CLICKED, NULL);
    return panel;
}

static void create_medium_page(lv_obj_t * screen)
{
    lv_obj_t * top;
    lv_obj_t * bottom;
    size_t index;

    ui.medium_page = make_plain(screen);
    lv_obj_set_size(ui.medium_page, LV_PCT(100), STIM_PAGE_HEIGHT);
    lv_obj_set_pos(ui.medium_page, 0, STIM_PAGE_TOP);
    lv_obj_set_style_bg_color(ui.medium_page, color(STIM_COLOR_BG), 0);
    lv_obj_set_style_bg_opa(ui.medium_page, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(ui.medium_page, 16, 0);
    lv_obj_set_style_pad_row(ui.medium_page, 12, 0);
    lv_obj_set_flex_flow(ui.medium_page, LV_FLEX_FLOW_COLUMN);

    top = make_plain(ui.medium_page);
    lv_obj_set_size(top, LV_PCT(100), 286);
    lv_obj_set_flex_flow(top, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(top, 12, 0);
    for(index = 0U; index < STIM_CHANNEL_COUNT; ++index) {
        create_channel_card(top, index);
    }

    bottom = make_plain(ui.medium_page);
    lv_obj_set_width(bottom, LV_PCT(100));
    lv_obj_set_flex_grow(bottom, 1);
    lv_obj_set_flex_flow(bottom, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(bottom, 12, 0);
    (void)create_prescription_panel(bottom, STIM_SCREEN_MEDIUM, false);
    (void)create_parameter_panel(bottom, STIM_SCREEN_MEDIUM, false);
}

static void create_receiver_card(lv_obj_t * parent, size_t index)
{
    receiver_view_t * view = &ui.receivers[index];
    lv_obj_t * title_row;

    view->card = make_panel(parent);
    lv_obj_set_size(view->card, 197, 120);
    lv_obj_set_style_pad_all(view->card, 4, 0);
    lv_obj_set_style_pad_row(view->card, 1, 0);
    lv_obj_set_flex_flow(view->card, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_flag(view->card, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(view->card, receiver_event, LV_EVENT_CLICKED, (void *)(uintptr_t)index);

    title_row = make_plain(view->card);
    lv_obj_set_size(title_row, LV_PCT(100), 22);
    lv_obj_set_flex_flow(title_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(title_row, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    view->id_label = make_label(title_row, "01号", &stim_font_20, STIM_COLOR_NAVY);
    view->selection_label = make_label(title_row, "", &stim_font_16, STIM_COLOR_BLUE);

    view->link_label = make_label(view->card, "无线在线  电量 90%", &stim_font_16, STIM_COLOR_MUTED);
    lv_obj_set_width(view->link_label, LV_PCT(100));
    view->prescription_label = make_label(view->card, "待配置", &stim_font_16, STIM_COLOR_TEXT);
    lv_obj_set_width(view->prescription_label, LV_PCT(100));
    view->time_label = make_label(view->card, "--:--", &lv_font_montserrat_20, STIM_COLOR_NAVY);
    view->state_label = make_label(view->card, "待配置", &stim_font_16, STIM_COLOR_MUTED);
    lv_obj_set_width(view->state_label, LV_PCT(100));
}

static lv_obj_t * create_receiver_panel(lv_obj_t * parent)
{
    lv_obj_t * panel = make_panel(parent);
    lv_obj_t * header;
    lv_obj_t * units;
    lv_obj_t * select_all;
    size_t index;

    lv_obj_set_size(panel, LV_PCT(100), 322);
    lv_obj_set_flex_flow(panel, LV_FLEX_FLOW_COLUMN);

    header = make_plain(panel);
    lv_obj_set_size(header, LV_PCT(100), 48);
    lv_obj_set_style_bg_color(header, color(STIM_COLOR_NAVY), 0);
    lv_obj_set_style_bg_opa(header, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(header, STIM_PANEL_RADIUS, 0);
    lv_obj_set_style_pad_hor(header, 16, 0);
    lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(header, 18, 0);
    (void)make_label(header, "治疗单元", &stim_font_20, 0xFFFFFFU);
    lv_obj_t * summary = make_label(header, "在线 10 / 12", &stim_font_16, 0xFFFFFFU);
    lv_obj_set_flex_grow(summary, 1);
    select_all = make_button(header, "全选", false, NULL);
    lv_obj_set_size(select_all, 88, 36);
    lv_obj_add_event_cb(select_all, select_all_event, LV_EVENT_CLICKED, NULL);

    units = make_plain(panel);
    lv_obj_set_width(units, LV_PCT(100));
    lv_obj_set_flex_grow(units, 1);
    lv_obj_set_style_pad_all(units, 12, 0);
    lv_obj_set_style_pad_row(units, 8, 0);
    lv_obj_set_style_pad_column(units, 8, 0);
    lv_obj_set_flex_flow(units, LV_FLEX_FLOW_ROW_WRAP);

    for(index = 0U; index < STIM_RECEIVER_COUNT; ++index) {
        create_receiver_card(units, index);
    }
    return panel;
}

static void create_low_page(lv_obj_t * screen)
{
    lv_obj_t * bottom;

    ui.low_page = make_plain(screen);
    lv_obj_set_size(ui.low_page, LV_PCT(100), STIM_PAGE_HEIGHT);
    lv_obj_set_pos(ui.low_page, 0, STIM_PAGE_TOP);
    lv_obj_set_style_bg_color(ui.low_page, color(STIM_COLOR_BG), 0);
    lv_obj_set_style_bg_opa(ui.low_page, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(ui.low_page, 16, 0);
    lv_obj_set_style_pad_row(ui.low_page, 12, 0);
    lv_obj_set_flex_flow(ui.low_page, LV_FLEX_FLOW_COLUMN);

    (void)create_receiver_panel(ui.low_page);

    bottom = make_plain(ui.low_page);
    lv_obj_set_width(bottom, LV_PCT(100));
    lv_obj_set_flex_grow(bottom, 1);
    lv_obj_set_flex_flow(bottom, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(bottom, 12, 0);
    (void)create_prescription_panel(bottom, STIM_SCREEN_LOW, true);
    (void)create_parameter_panel(bottom, STIM_SCREEN_LOW, true);
}

void stim_ui_create(stim_model_t * model)
{
    lv_obj_t * screen = lv_screen_active();

    ui = (ui_context_t) {0};
    ui.model = model;
    ui.visible_screen = STIM_SCREEN_MEDIUM;

    lv_obj_set_style_bg_color(screen, color(STIM_COLOR_BG), 0);
    lv_obj_set_style_text_font(screen, &stim_font_16, 0);
    lv_obj_set_style_text_color(screen, color(STIM_COLOR_TEXT), 0);
    lv_obj_set_style_pad_all(screen, 0, 0);
    no_scroll(screen);

    create_header(screen);
    create_medium_page(screen);
    create_low_page(screen);
    (void)lv_timer_create(timer_event, 1000, NULL);
    refresh_all();
}

void stim_ui_show_screen(stim_screen_t screen)
{
    bool medium = screen == STIM_SCREEN_MEDIUM;

    ui.visible_screen = screen;
    if(medium) {
        lv_obj_remove_flag(ui.medium_page, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(ui.low_page, LV_OBJ_FLAG_HIDDEN);
    }
    else {
        lv_obj_add_flag(ui.medium_page, LV_OBJ_FLAG_HIDDEN);
        lv_obj_remove_flag(ui.low_page, LV_OBJ_FLAG_HIDDEN);
    }

    lv_obj_set_style_bg_color(ui.medium_tab, color(medium ? STIM_COLOR_NAVY : STIM_COLOR_CARD), 0);
    lv_obj_set_style_border_width(ui.medium_tab, 0, 0);
    lv_obj_set_style_text_color(ui.medium_tab, color(medium ? 0xFFFFFFU : STIM_COLOR_TEXT), 0);
    lv_obj_set_style_text_color(lv_obj_get_child(ui.medium_tab, 0),
                                color(medium ? 0xFFFFFFU : STIM_COLOR_TEXT), 0);

    lv_obj_set_style_bg_color(ui.low_tab, color(medium ? STIM_COLOR_CARD : STIM_COLOR_NAVY), 0);
    lv_obj_set_style_border_width(ui.low_tab, 0, 0);
    lv_obj_set_style_text_color(ui.low_tab, color(medium ? STIM_COLOR_TEXT : 0xFFFFFFU), 0);
    lv_obj_set_style_text_color(lv_obj_get_child(ui.low_tab, 0),
                                color(medium ? STIM_COLOR_TEXT : 0xFFFFFFU), 0);
}
