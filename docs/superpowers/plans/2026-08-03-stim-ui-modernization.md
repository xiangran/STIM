# STIM Simulator UI Modernization Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Restyle the existing LVGL PC simulator (`stim-simulator/src/ui/stim_ui.c`) from its current flat/navy look to the approved "柔和健康 (Soft Health-tech)" visual direction — warm palette, pill shapes, soft shadows, gradient accents, a handful of built-in LVGL icons — without changing any page structure, state machine, or treatment parameter.

**Architecture:** This is a single-file, in-place restyle. No new source files, no new build targets, no new fonts. Every task edits `stim-simulator/src/ui/stim_ui.c` (macros, shared style helpers, then each screen area top-to-bottom: header/tabs → channel cards → receiver tiles → prescription/parameter panels), then rebuilds and regenerates the reference screenshots.

**Tech Stack:** C11, LVGL v9.5.0 (fetched by CMake into `stim-simulator/out/build/macos-debug/_deps/lvgl-src`), SDL2, CMake presets, ctest.

## Global Constraints

- Do not change `src/model/*`, page structure, tab count, card counts, or any treatment parameter default/range. This is a visual-only change (see `output/ui-design/stim-ui-design-spec.md`, section "v2 现代化视觉方案").
- No new font assets. Icons use LVGL's built-in `LV_SYMBOL_*` glyphs rendered via the already-enabled `lv_font_montserrat_14/16/20/24/28/32` (see `stim-simulator/lv_conf.h:17-22`). The custom Chinese subset fonts `stim_font_16/20/24` (`assets/fonts/stim_fonts.h`) do **not** contain symbol glyphs — never put an `LV_SYMBOL_*` string into a label styled with `stim_font_*`; always give the icon its own label using a `lv_font_montserrat_*` font.
- The build enables `-Wall -Wextra -Wpedantic -Werror` (`CMakeLists.txt:69-70`), which turns an unused `static` function into a build failure. Any new helper function must be introduced in the same task where it is first called — never add a helper "for later."
- Minimum touch target for anything tappable is 44×44 px (`STIM_TOUCH_MIN`, capacitive touchscreen target hardware). Existing controls smaller than that must be bumped to at least 44 px as part of the task that touches them.
- **Adapted verification, not classic TDD:** this codebase's only automated tests (`tests/test_model.c`, run via `ctest`) cover the state-machine model, which this plan does not touch — they must stay green as a regression guard, but there is no meaningful unit-testable assertion for LVGL color/radius/shadow values. Each task's "test" step is therefore: (1) clean rebuild succeeds under `-Werror`, (2) `ctest` still passes unchanged, (3) a fresh screenshot is generated and visually reviewed against the described change. Do not invent fake assertions to force-fit the usual test-first template.
- Build/verify commands (macOS): run from `stim-simulator/`:
  ```bash
  cmake --build --preset macos-debug -j
  ctest --preset macos-debug
  ./out/build/macos-debug/bin/stim-simulator --screen medium --screenshot /tmp/medium.bmp
  ./out/build/macos-debug/bin/stim-simulator --screen low --screenshot /tmp/low.bmp
  ```
  (`--screen`/`--screenshot` flags are defined in `src/main.c:18-52`; screenshots are BMP only — PNG conversion is a manual step, handled in Task 6.)

---

### Task 1: Design tokens + `make_panel` / `make_button` / `make_section_header`

**Files:**
- Modify: `stim-simulator/src/ui/stim_ui.c:8-25` (color/size macros), `:95-107` (`make_panel`), `:123-147` (`make_button`), `:149-163` (`make_section_header`)
- Test: none (visual-only; see Global Constraints) — verify via build + `ctest` + screenshot

**Interfaces:**
- Consumes: nothing new (modifies existing functions in place)
- Produces: same macro names as before, new values; two new macros (`STIM_METRIC_RADIUS`, `STIM_TOUCH_MIN`) and four brand-new tokens (`STIM_COLOR_RUNNING_SOFT`, `STIM_COLOR_GRAD_START`, `STIM_COLOR_GRAD_END`, `STIM_COLOR_SHADOW`) that later tasks rely on by name. A new static helper `apply_button_style(lv_obj_t * button, bool primary)` holds the pill/gradient/press-opacity/shadow setup shared by `make_button` and Task 3's `make_icon_text_button` (Task 3 reuses it instead of duplicating it — see Task 3 Step 1). `make_panel(parent)` now also applies a soft shadow. `make_button(parent, text, primary, label_out)` keeps its exact signature but is now a pill (`LV_RADIUS_CIRCLE`), uses a horizontal gradient fill when `primary`, a neutral border when not, and dims via `bg_opa` on press instead of swapping to a hardcoded navy.

- [ ] **Step 1: Replace the color/size macro block**

Replace `stim-simulator/src/ui/stim_ui.c:8-25`:

```c
#define STIM_COLOR_BG 0xF4F7FAU
#define STIM_COLOR_CARD 0xFFFFFFU
#define STIM_COLOR_NAVY 0x0B4F8AU
#define STIM_COLOR_BLUE 0x0A61B8U
#define STIM_COLOR_CYAN 0x16A6C9U
#define STIM_COLOR_TEAL 0x159A99U
#define STIM_COLOR_CORAL 0xE45F5FU
#define STIM_COLOR_AMBER 0xC98518U
#define STIM_COLOR_TEXT 0x17324DU
#define STIM_COLOR_MUTED 0x708399U
#define STIM_COLOR_BORDER 0xD8E1EAU
#define STIM_COLOR_DISABLED 0xEEF2F6U
#define STIM_COLOR_SELECTED 0xEAF4FFU

#define STIM_HEADER_HEIGHT 64
#define STIM_PAGE_TOP 64
#define STIM_PAGE_HEIGHT 736
#define STIM_PANEL_RADIUS 12
```

with:

```c
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
```

`STIM_COLOR_NAVY` and `STIM_COLOR_BLUE` are intentionally the same accent blue now — the two-tone navy/blue distinction from the old palette collapses into one accent color used for both "selected" state and accent numerals. `STIM_COLOR_CYAN` stays unused (it was already dead in the original file — confirmed via grep, not introduced by this change).

- [ ] **Step 2: Rebuild and confirm it still compiles (macros alone don't change behavior yet)**

Run: `cmake --build --preset macos-debug -j` from `stim-simulator/`
Expected: build succeeds (colors changed, nothing else yet since call sites aren't touched until later steps in this task)

- [ ] **Step 3: Add a soft shadow to `make_panel`**

Replace `stim-simulator/src/ui/stim_ui.c:95-107`:

```c
static lv_obj_t * make_panel(lv_obj_t * parent)
{
    lv_obj_t * panel = lv_obj_create(parent);

    lv_obj_set_style_bg_color(panel, color(STIM_COLOR_CARD), 0);
    lv_obj_set_style_bg_opa(panel, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(panel, color(STIM_COLOR_BORDER), 0);
    lv_obj_set_style_border_width(panel, 1, 0);
    lv_obj_set_style_radius(panel, STIM_PANEL_RADIUS, 0);
    lv_obj_set_style_pad_all(panel, 0, 0);
    no_scroll(panel);
    return panel;
}
```

with:

```c
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
```

- [ ] **Step 4: Turn `make_button` into a pill with gradient primary fill**

Replace `stim-simulator/src/ui/stim_ui.c:123-147`:

```c
static lv_obj_t * make_button(lv_obj_t * parent,
                              const char * text,
                              bool primary,
                              lv_obj_t ** label_out)
{
    lv_obj_t * button = lv_button_create(parent);
    lv_obj_t * label;

    lv_obj_set_style_radius(button, 8, 0);
    lv_obj_set_style_border_width(button, primary ? 0 : 1, 0);
    lv_obj_set_style_border_color(button, color(STIM_COLOR_BLUE), 0);
    lv_obj_set_style_bg_color(button, color(primary ? STIM_COLOR_BLUE : STIM_COLOR_CARD), 0);
    lv_obj_set_style_bg_color(button, color(0x074E96U), LV_STATE_PRESSED);
    lv_obj_set_style_text_color(button, color(primary ? 0xFFFFFFU : STIM_COLOR_NAVY), 0);
    lv_obj_set_style_pad_all(button, 6, 0);
    lv_obj_set_style_shadow_width(button, 0, 0);
    no_scroll(button);

    label = make_label(button, text, &stim_font_16, primary ? 0xFFFFFFU : STIM_COLOR_NAVY);
    lv_obj_center(label);
    if(label_out != NULL) {
        *label_out = label;
    }
    return button;
}
```

with:

```c
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
```

`apply_button_style` is a new shared helper — it holds every style call that Task 3's `make_icon_text_button` also needs, so that function calls it too instead of repeating the same nine lines (see Task 3 Step 1). Two deliberate behavior changes here, not typos: the non-primary button border goes from accent-blue to neutral `STIM_COLOR_BORDER` (secondary actions should look quiet), and its label goes from `STIM_COLOR_NAVY` (now repurposed as accent blue) to `STIM_COLOR_TEXT` (so ordinary buttons read as dark text, not blue).

- [ ] **Step 5: Strip the solid-color bar out of `make_section_header`**

Replace `stim-simulator/src/ui/stim_ui.c:149-163`:

```c
static lv_obj_t * make_section_header(lv_obj_t * parent, const char * title)
{
    lv_obj_t * header = make_plain(parent);

    lv_obj_set_size(header, LV_PCT(100), 48);
    lv_obj_set_style_bg_color(header, color(STIM_COLOR_NAVY), 0);
    lv_obj_set_style_bg_opa(header, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(header, STIM_PANEL_RADIUS, 0);
    lv_obj_set_style_pad_left(header, 16, 0);
    lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    (void)make_label(header, title, &stim_font_20, 0xFFFFFFU);
    return header;
}
```

with:

```c
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
```

This is used by the "处方" / "处方参数" / "批量配置" panel headings (`create_prescription_panel`, `create_parameter_panel`) — they automatically become plain dark headings on the white panel instead of a navy bar with no other code changes required.

- [ ] **Step 6: Rebuild, run tests, generate a checkpoint screenshot**

Run:
```bash
cd stim-simulator
cmake --build --preset macos-debug -j
ctest --preset macos-debug
./out/build/macos-debug/bin/stim-simulator --screen medium --screenshot /tmp/task1-medium.bmp
```
Expected: build succeeds under `-Werror`, `ctest` passes (model tests untouched), and the screenshot shows a warm off-white background, larger-radius shadowed cards, pill-shaped buttons, and the "处方"/"处方参数" section titles as plain dark text (no navy bar) — even though channel/receiver headers still show their old navy/teal/amber bar until Task 3/4.

- [ ] **Step 7: Commit**

```bash
git add stim-simulator/src/ui/stim_ui.c
git commit -m "style: introduce soft health-tech design tokens and pill buttons"
```

---

### Task 2: Header / tab bar — pill segmented control + gear icon

**Files:**
- Modify: `stim-simulator/src/ui/stim_ui.c` — `create_header` (originally `:618-668`), `stim_ui_show_screen` (originally `:1091-1116`)
- Test: none (visual-only) — verify via build + `ctest` + screenshot

**Interfaces:**
- Consumes: `make_button`, `STIM_COLOR_*`, `STIM_TOUCH_MIN` from Task 1
- Produces: new static helper `set_tab_active(lv_obj_t * tab, bool active)`, called only from `stim_ui_show_screen` (defined immediately above it, same task)

- [ ] **Step 1: Rebuild the tab row as a pill segmented control and iconify the settings button**

In `create_header`, replace the tabs block:

```c
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
```

with:

```c
    tabs = make_plain(header);
    lv_obj_set_size(tabs, 400, 48);
    lv_obj_set_style_bg_color(tabs, color(STIM_COLOR_DISABLED), 0);
    lv_obj_set_style_bg_opa(tabs, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(tabs, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_pad_all(tabs, 4, 0);
    lv_obj_set_flex_flow(tabs, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(tabs, 4, 0);

    ui.medium_tab = make_button(tabs, "中频治疗", false, NULL);
    lv_obj_set_height(ui.medium_tab, LV_PCT(100));
    lv_obj_set_flex_grow(ui.medium_tab, 1);
    lv_obj_set_style_border_width(ui.medium_tab, 0, 0);
    lv_obj_set_style_text_font(lv_obj_get_child(ui.medium_tab, 0), &stim_font_20, 0);
    lv_obj_add_event_cb(ui.medium_tab, tab_event, LV_EVENT_CLICKED, (void *)(uintptr_t)STIM_SCREEN_MEDIUM);

    ui.low_tab = make_button(tabs, "低频治疗", false, NULL);
    lv_obj_set_height(ui.low_tab, LV_PCT(100));
    lv_obj_set_flex_grow(ui.low_tab, 1);
    lv_obj_set_style_border_width(ui.low_tab, 0, 0);
    lv_obj_set_style_text_font(lv_obj_get_child(ui.low_tab, 0), &stim_font_20, 0);
    lv_obj_add_event_cb(ui.low_tab, tab_event, LV_EVENT_CLICKED, (void *)(uintptr_t)STIM_SCREEN_LOW);
```

(Tab label size drops from 24px to the existing 20px font so text sits comfortably in the shorter 48px pill — still one of the three font sizes already shipped, no new font asset.)

Then replace the settings button creation:

```c
    settings = make_button(right, "设置", false, NULL);
    lv_obj_set_size(settings, 72, 44);
    lv_obj_add_event_cb(settings, settings_event, LV_EVENT_CLICKED, NULL);
```

with:

```c
    settings = make_button(right, LV_SYMBOL_SETTINGS, false, NULL);
    lv_obj_set_size(settings, STIM_TOUCH_MIN, STIM_TOUCH_MIN);
    lv_obj_set_style_text_font(lv_obj_get_child(settings, 0), &lv_font_montserrat_20, 0);
    lv_obj_add_event_cb(settings, settings_event, LV_EVENT_CLICKED, NULL);
```

(`make_button` creates the label with `stim_font_16` first, which has no symbol glyph — overriding the label's font to `lv_font_montserrat_20` right after creation, before any frame is drawn, is the same pattern the original code already used for the tab labels above, so this is a proven-safe sequence, not a new risk.)

- [ ] **Step 2: Replace the manual navy/white tab recoloring in `stim_ui_show_screen` with a gradient helper**

Add this new function directly above `stim_ui_show_screen` (originally around `:1091`), and replace the body of `stim_ui_show_screen`:

```c
static void set_tab_active(lv_obj_t * tab, bool active)
{
    lv_obj_t * label = lv_obj_get_child(tab, 0);

    lv_obj_set_style_bg_grad_dir(tab, LV_GRAD_DIR_NONE, 0);
    if(active) {
        lv_obj_set_style_bg_color(tab, color(STIM_COLOR_GRAD_START), 0);
        lv_obj_set_style_bg_grad_color(tab, color(STIM_COLOR_GRAD_END), 0);
        lv_obj_set_style_bg_grad_dir(tab, LV_GRAD_DIR_HOR, 0);
        lv_obj_set_style_bg_opa(tab, LV_OPA_COVER, 0);
    }
    else {
        lv_obj_set_style_bg_opa(tab, LV_OPA_TRANSP, 0);
    }
    lv_obj_set_style_text_color(tab, color(active ? 0xFFFFFFU : STIM_COLOR_MUTED), 0);
    lv_obj_set_style_text_color(label, color(active ? 0xFFFFFFU : STIM_COLOR_MUTED), 0);
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

    set_tab_active(ui.medium_tab, medium);
    set_tab_active(ui.low_tab, !medium);
}
```

- [ ] **Step 3: Rebuild, run tests, screenshot both pages**

Run:
```bash
cd stim-simulator
cmake --build --preset macos-debug -j
ctest --preset macos-debug
./out/build/macos-debug/bin/stim-simulator --screen medium --screenshot /tmp/task2-medium.bmp
./out/build/macos-debug/bin/stim-simulator --screen low --screenshot /tmp/task2-low.bmp
```
Expected: build succeeds, `ctest` passes, both screenshots show the pill-shaped segmented tab control with a teal-to-blue gradient fill on the active tab, and a circular gear-icon settings button.

- [ ] **Step 4: Commit**

```bash
git add stim-simulator/src/ui/stim_ui.c
git commit -m "style: pill segmented tab control and icon settings button"
```

---

### Task 3: Channel cards (中频 A–D) — badge header, glow border, icon buttons

**Files:**
- Modify: `stim-simulator/src/ui/stim_ui.c` — `channel_view_t` unchanged, `refresh_channel` (orig `:199-249`), `create_metric` (orig `:670-686`), `create_channel_card` (orig `:688-755`)
- Test: none (visual-only) — verify via build + `ctest` + screenshot

**Interfaces:**
- Consumes: `make_panel`, `make_button`, `make_plain`, `make_label`, `apply_button_style`, `STIM_COLOR_*`, `STIM_METRIC_RADIUS`, `STIM_TOUCH_MIN` from Task 1
- Produces: two new static helpers used starting in this task and reused by Task 4: `style_badge(lv_obj_t * label, uint32_t fg, uint32_t bg)` and `make_icon_text_button(lv_obj_t * parent, const char * icon, const char * text, bool primary, lv_obj_t ** label_out)`. Both must be defined and called within this task (the build fails on an unused `static` function otherwise).

- [ ] **Step 1: Add `style_badge` and `make_icon_text_button` right after `make_button` (orig `:147`)**

```c
static void style_badge(lv_obj_t * label, uint32_t fg, uint32_t bg)
{
    lv_obj_set_style_text_color(label, color(fg), 0);
    lv_obj_set_style_bg_color(label, color(bg), 0);
    lv_obj_set_style_bg_opa(label, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(label, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_pad_hor(label, 8, 0);
    lv_obj_set_style_pad_ver(label, 3, 0);
}

static lv_obj_t * make_icon_text_button(lv_obj_t * parent,
                                        const char * icon,
                                        const char * text,
                                        bool primary,
                                        lv_obj_t ** label_out)
{
    lv_obj_t * button = lv_button_create(parent);
    lv_obj_t * row;
    lv_obj_t * text_label;

    apply_button_style(button, primary);

    row = make_plain(button);
    lv_obj_set_size(row, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_center(row);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(row, 5, 0);

    (void)make_label(row, icon, &lv_font_montserrat_16, primary ? 0xFFFFFFU : STIM_COLOR_TEXT);
    text_label = make_label(row, text, &stim_font_16, primary ? 0xFFFFFFU : STIM_COLOR_TEXT);

    if(label_out != NULL) {
        *label_out = text_label;
    }
    return button;
}
```

(`style_badge` gives any label a colored pill background — used for channel/receiver state text. `make_icon_text_button` reuses Task 1's `apply_button_style` for the button chrome, then builds its own icon+text row — it can't reuse `make_button` itself because the icon glyph needs its own `lv_font_montserrat_16`-styled label; the Chinese `stim_font_16` used for the text has no symbol glyphs, so the two can never share one label.)

- [ ] **Step 2: Restyle `create_metric` as a flat tinted chip instead of a nested shadowed panel**

Replace `create_metric` (orig `:670-686`):

```c
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
```

with:

```c
static void create_metric(lv_obj_t * parent,
                          const char * caption,
                          lv_obj_t ** value_label,
                          bool time_value)
{
    lv_obj_t * box = make_plain(parent);

    lv_obj_set_height(box, LV_PCT(100));
    lv_obj_set_flex_grow(box, 1);
    lv_obj_set_style_bg_color(box, color(STIM_COLOR_DISABLED), 0);
    lv_obj_set_style_bg_opa(box, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(box, STIM_METRIC_RADIUS, 0);
    lv_obj_set_style_pad_all(box, 8, 0);
    lv_obj_set_flex_flow(box, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(box, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    (void)make_label(box, caption, &stim_font_16, STIM_COLOR_MUTED);
    *value_label = make_label(box, time_value ? "--:--" : "--",
                              &lv_font_montserrat_28, STIM_COLOR_NAVY);
}
```

(Switching from `make_panel` to `make_plain` + a flat tint avoids a shadowed-box-inside-a-shadowed-card look; `STIM_COLOR_NAVY` here is now the accent blue from Task 1, so the big number reads as an accent color, unchanged call site.)

- [ ] **Step 3: Remove the solid header bar and add icon buttons in `create_channel_card`**

Replace the header block inside `create_channel_card` (orig `:704-713`):

```c
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
```

with:

```c
    view->header = make_plain(view->card);
    lv_obj_set_size(view->header, LV_PCT(100), 48);
    lv_obj_set_style_pad_hor(view->header, 14, 0);
    lv_obj_set_flex_flow(view->header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(view->header, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    view->title_label = make_label(view->header, "A通道", &stim_font_20, STIM_COLOR_TEXT);
    view->state_label = make_label(view->header, "就绪", &stim_font_16, STIM_COLOR_MUTED);
    style_badge(view->state_label, STIM_COLOR_MUTED, STIM_COLOR_DISABLED);
```

Then replace the three control buttons (orig `:737-754`):

```c
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
```

with:

```c
    view->start_button = make_icon_text_button(controls, LV_SYMBOL_PLAY, "开始", false, NULL);
    lv_obj_set_height(view->start_button, LV_PCT(100));
    lv_obj_set_flex_grow(view->start_button, 1);
    lv_obj_add_event_cb(view->start_button, channel_control_event, LV_EVENT_CLICKED,
                        (void *)(base | 0U));

    view->pause_button = make_icon_text_button(controls, LV_SYMBOL_PAUSE, "暂停", false, &view->pause_label);
    lv_obj_set_height(view->pause_button, LV_PCT(100));
    lv_obj_set_flex_grow(view->pause_button, 1);
    lv_obj_add_event_cb(view->pause_button, channel_control_event, LV_EVENT_CLICKED,
                        (void *)(base | 1U));

    view->stop_button = make_icon_text_button(controls, LV_SYMBOL_STOP, "停止", false, NULL);
    lv_obj_set_height(view->stop_button, LV_PCT(100));
    lv_obj_set_flex_grow(view->stop_button, 1);
    lv_obj_set_style_border_color(view->stop_button, color(STIM_COLOR_CORAL), 0);
    lv_obj_add_event_cb(view->stop_button, channel_control_event, LV_EVENT_CLICKED,
                        (void *)(base | 2U));
```

(`view->pause_label` still receives the text label from `make_icon_text_button`'s `label_out`, so `refresh_channel`'s existing `lv_label_set_text(view->pause_label, ...)` toggle between "暂停"/"继续" keeps working untouched. Known simplification: the pause icon stays "⏸" even when the button reads "继续" — swapping it to "▶" would need plumbing an icon-label reference through `channel_view_t` too, which this pass intentionally skips.)

- [ ] **Step 4: Update `refresh_channel` to paint the badge and glow border instead of a solid header color**

Replace `refresh_channel` (orig `:199-249`):

```c
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
```

with:

```c
static void refresh_channel(size_t index)
{
    stim_channel_t * channel = &ui.model->channels[index];
    channel_view_t * view = &ui.channels[index];
    char buffer[32];
    uint32_t badge_fg = STIM_COLOR_MUTED;
    uint32_t badge_bg = STIM_COLOR_DISABLED;
    uint32_t border_color = STIM_COLOR_BORDER;
    bool configured = channel->state != STIM_STATE_UNCONFIGURED;
    bool active = (channel->state == STIM_STATE_RUNNING) || (channel->state == STIM_STATE_PAUSED);

    if(channel->state == STIM_STATE_RUNNING) {
        badge_fg = STIM_COLOR_TEAL;
        badge_bg = STIM_COLOR_RUNNING_SOFT;
        border_color = STIM_COLOR_TEAL;
    }
    else if(channel->state == STIM_STATE_PAUSED) {
        badge_fg = STIM_COLOR_AMBER;
        badge_bg = STIM_COLOR_SELECTED;
        border_color = STIM_COLOR_AMBER;
    }
    else if(channel->selected) {
        badge_fg = STIM_COLOR_BLUE;
        badge_bg = STIM_COLOR_SELECTED;
        border_color = STIM_COLOR_BLUE;
    }

    style_badge(view->state_label, badge_fg, badge_bg);
    lv_obj_set_style_border_width(view->card, (channel->selected || active) ? 2 : 1, 0);
    lv_obj_set_style_border_color(view->card, color(border_color), 0);
    lv_obj_set_style_bg_color(view->card,
                              color((channel->selected && !active) ? STIM_COLOR_SELECTED : STIM_COLOR_CARD),
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
```

(Same state → color mapping as before — running=teal, paused=amber, selected=blue — just expressed as a badge fill + card border instead of a solid header bar. No text/value/enable logic changed.)

- [ ] **Step 5: Rebuild, run tests, screenshot the medium page**

```bash
cd stim-simulator
cmake --build --preset macos-debug -j
ctest --preset macos-debug
./out/build/macos-debug/bin/stim-simulator --screen medium --screenshot /tmp/task3-medium.bmp
```
Expected: build succeeds, `ctest` passes, channel cards A–D show a plain white header with a colored state badge on the right, no solid color bar, and start/pause/stop buttons show a play/pause/stop glyph next to the Chinese label.

- [ ] **Step 6: Commit**

```bash
git add stim-simulator/src/ui/stim_ui.c
git commit -m "style: badge-based channel card headers with icon controls"
```

---

### Task 4: Receiver tiles (低频 12-grid) + receiver panel header

**Files:**
- Modify: `stim-simulator/src/ui/stim_ui.c` — `receiver_view_t` struct (orig `:41-49`, add one field), `refresh_receiver` (orig `:260-319`), `create_receiver_card` (orig `:975-1003`), `create_receiver_panel` header block (orig `:1005-1030`)
- Test: none (visual-only) — verify via build + `ctest` + screenshot

**Interfaces:**
- Consumes: `style_badge` (Task 3), `STIM_COLOR_*`, `STIM_METRIC_RADIUS`, `STIM_TOUCH_MIN` from Task 1/3
- Produces: `receiver_view_t` gains a `link_icon` field used only inside this task's functions

- [ ] **Step 1: Add a `link_icon` field to `receiver_view_t`**

Replace (orig `:41-49`):

```c
typedef struct {
    lv_obj_t * card;
    lv_obj_t * id_label;
    lv_obj_t * selection_label;
    lv_obj_t * link_label;
    lv_obj_t * prescription_label;
    lv_obj_t * time_label;
    lv_obj_t * state_label;
} receiver_view_t;
```

with:

```c
typedef struct {
    lv_obj_t * card;
    lv_obj_t * id_label;
    lv_obj_t * selection_label;
    lv_obj_t * link_icon;
    lv_obj_t * link_label;
    lv_obj_t * prescription_label;
    lv_obj_t * time_label;
    lv_obj_t * state_label;
} receiver_view_t;
```

- [ ] **Step 2: Give each receiver tile a wifi icon and a badge state, in `create_receiver_card`**

Replace `create_receiver_card` (orig `:975-1003`):

```c
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
```

with:

```c
static void create_receiver_card(lv_obj_t * parent, size_t index)
{
    receiver_view_t * view = &ui.receivers[index];
    lv_obj_t * title_row;
    lv_obj_t * link_row;

    view->card = make_panel(parent);
    lv_obj_set_size(view->card, 197, 120);
    lv_obj_set_style_radius(view->card, STIM_METRIC_RADIUS, 0);
    lv_obj_set_style_pad_all(view->card, 8, 0);
    lv_obj_set_style_pad_row(view->card, 2, 0);
    lv_obj_set_flex_flow(view->card, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_flag(view->card, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(view->card, receiver_event, LV_EVENT_CLICKED, (void *)(uintptr_t)index);

    title_row = make_plain(view->card);
    lv_obj_set_size(title_row, LV_PCT(100), 22);
    lv_obj_set_flex_flow(title_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(title_row, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    view->id_label = make_label(title_row, "01号", &stim_font_20, STIM_COLOR_TEXT);
    view->selection_label = make_label(title_row, "", &stim_font_16, STIM_COLOR_BLUE);

    link_row = make_plain(view->card);
    lv_obj_set_size(link_row, LV_PCT(100), 16);
    lv_obj_set_flex_flow(link_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(link_row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(link_row, 4, 0);
    view->link_icon = make_label(link_row, LV_SYMBOL_WIFI, &lv_font_montserrat_14, STIM_COLOR_MUTED);
    view->link_label = make_label(link_row, "无线在线  电量 90%", &stim_font_16, STIM_COLOR_MUTED);

    view->prescription_label = make_label(view->card, "待配置", &stim_font_16, STIM_COLOR_TEXT);
    lv_obj_set_width(view->prescription_label, LV_PCT(100));
    view->time_label = make_label(view->card, "--:--", &lv_font_montserrat_20, STIM_COLOR_NAVY);
    view->state_label = make_label(view->card, "待配置", &stim_font_16, STIM_COLOR_MUTED);
    style_badge(view->state_label, STIM_COLOR_MUTED, STIM_COLOR_DISABLED);
}
```

(Dropped the `lv_obj_set_width(view->state_label, LV_PCT(100));` from the original — a full-width badge pill would stretch oddly; a badge should hug its text.)

- [ ] **Step 3: Update `refresh_receiver` to paint the badge, the wifi icon color, and use the new `STIM_COLOR_RUNNING_SOFT` token**

Replace `refresh_receiver` (orig `:260-319`):

```c
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
```

with:

```c
static void refresh_receiver(size_t index)
{
    stim_receiver_t * receiver = &ui.model->receivers[index];
    receiver_view_t * view = &ui.receivers[index];
    char buffer[48];
    uint32_t border_color = STIM_COLOR_BORDER;
    uint32_t background = STIM_COLOR_CARD;
    uint32_t badge_fg = STIM_COLOR_MUTED;
    uint32_t badge_bg = STIM_COLOR_DISABLED;
    bool configured = (receiver->state != STIM_STATE_UNCONFIGURED) &&
                      (receiver->state != STIM_STATE_OFFLINE);

    if(receiver->state == STIM_STATE_RUNNING) {
        border_color = STIM_COLOR_TEAL;
        background = STIM_COLOR_RUNNING_SOFT;
        badge_fg = STIM_COLOR_TEAL;
        badge_bg = STIM_COLOR_RUNNING_SOFT;
    }
    else if(receiver->selected) {
        border_color = STIM_COLOR_BLUE;
        background = STIM_COLOR_SELECTED;
        badge_fg = STIM_COLOR_BLUE;
        badge_bg = STIM_COLOR_SELECTED;
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
    style_badge(view->state_label, badge_fg, badge_bg);
    lv_obj_set_style_text_color(view->link_icon,
                                color(receiver->state == STIM_STATE_OFFLINE ? STIM_COLOR_MUTED : STIM_COLOR_TEAL),
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
        lv_label_set_text(view->state_label, "已配置 锁定");
    }
    else {
        lv_label_set_text(view->state_label, unit_state_text(receiver->state));
    }
}
```

- [ ] **Step 4: Destyle the "治疗单元" panel header bar and bump "全选" to touch size, in `create_receiver_panel`**

Replace the header block (orig `:1005-1030`, keep everything from `units = make_plain(panel);` onward unchanged):

```c
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
```

with:

```c
    header = make_plain(panel);
    lv_obj_set_size(header, LV_PCT(100), 48);
    lv_obj_set_style_pad_hor(header, 16, 0);
    lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(header, 18, 0);
    (void)make_label(header, "治疗单元", &stim_font_20, STIM_COLOR_TEXT);
    lv_obj_t * summary = make_label(header, "在线 10 / 12", &stim_font_16, STIM_COLOR_MUTED);
    lv_obj_set_flex_grow(summary, 1);
    select_all = make_button(header, "全选", false, NULL);
    lv_obj_set_size(select_all, 88, STIM_TOUCH_MIN);
    lv_obj_add_event_cb(select_all, select_all_event, LV_EVENT_CLICKED, NULL);
```

- [ ] **Step 5: Rebuild, run tests, screenshot the low-frequency page**

```bash
cd stim-simulator
cmake --build --preset macos-debug -j
ctest --preset macos-debug
./out/build/macos-debug/bin/stim-simulator --screen low --screenshot /tmp/task4-low.bmp
```
Expected: build succeeds, `ctest` passes, all 12 receiver tiles show a small wifi glyph next to the link text, a colored state badge instead of plain muted text, and the "治疗单元" panel header is plain text (no navy bar); "全选" button is visibly ≥44px tall.

- [ ] **Step 6: Commit**

```bash
git add stim-simulator/src/ui/stim_ui.c
git commit -m "style: badge-based receiver tiles with wifi icon and touch-sized controls"
```

---

### Task 5: Prescription rows, wave buttons, parameter sliders/steppers, apply/placement buttons

**Files:**
- Modify: `stim-simulator/src/ui/stim_ui.c` — `create_prescription_row` (orig `:757-789`), `create_wave_row` (orig `:815-839`), `refresh_parameters` wave-button loop (orig `:435-444`), `create_parameter_row` (orig `:841-895`), `create_parameter_panel` action row (orig `:918-939`)
- Test: none (visual-only) — verify via build + `ctest` + screenshot

**Interfaces:**
- Consumes: `make_button`, `STIM_COLOR_*`, `STIM_TOUCH_MIN` from Task 1 (no new helpers needed in this task)

- [ ] **Step 1: Round the prescription row and make its number badge a gradient circle, in `create_prescription_row`**

Replace (orig `:757-789`):

```c
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
```

with:

```c
static lv_obj_t * create_prescription_row(lv_obj_t * parent, size_t screen, size_t index, bool compact)
{
    lv_obj_t * row = make_plain(parent);
    lv_obj_t * badge;
    char number[8];

    lv_obj_set_size(row, LV_PCT(100), compact ? STIM_TOUCH_MIN : 50);
    lv_obj_set_style_bg_color(row, color(STIM_COLOR_CARD), 0);
    lv_obj_set_style_bg_opa(row, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(row, color(STIM_COLOR_BORDER), 0);
    lv_obj_set_style_border_width(row, 1, 0);
    lv_obj_set_style_radius(row, 14, 0);
    lv_obj_set_style_pad_hor(row, 8, 0);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(row, 12, 0);
    lv_obj_add_flag(row, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(row, prescription_event, LV_EVENT_CLICKED, (void *)(uintptr_t)index);

    badge = make_plain(row);
    lv_obj_set_size(badge, 30, 30);
    lv_obj_set_style_radius(badge, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(badge, color(STIM_COLOR_GRAD_START), 0);
    lv_obj_set_style_bg_grad_color(badge, color(STIM_COLOR_GRAD_END), 0);
    lv_obj_set_style_bg_grad_dir(badge, LV_GRAD_DIR_HOR, 0);
    lv_obj_set_style_bg_opa(badge, LV_OPA_COVER, 0);
    (void)snprintf(number, sizeof(number), "%02u", (unsigned)(index + 1U));
    lv_obj_t * badge_label = make_label(badge, number, &lv_font_montserrat_20, 0xFFFFFFU);
    lv_obj_center(badge_label);

    lv_obj_t * text = make_label(row, stim_prescriptions[index], &stim_font_16, STIM_COLOR_TEXT);
    lv_obj_set_flex_grow(text, 1);
    ui.prescription_rows[screen][index] = row;
    return row;
}
```

(`refresh_prescriptions` — the function that recolors the selected row — reads `STIM_COLOR_SELECTED`/`STIM_COLOR_BLUE`/`STIM_COLOR_BORDER`/`STIM_COLOR_CARD` only, all already repointed by Task 1's macro changes, so it needs no edits of its own.)

- [ ] **Step 2: Bump wave buttons to touch size, in `create_wave_row`**

Replace (orig `:815-839`):

```c
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
```

with:

```c
static lv_obj_t * create_wave_row(lv_obj_t * parent, stim_screen_t screen, bool compact)
{
    static const char * const wave_names[3] = {"正弦波", "方波", "三角波"};
    lv_obj_t * row = make_plain(parent);
    size_t index;

    lv_obj_set_size(row, LV_PCT(100), compact ? 52 : 56);
    lv_obj_set_style_pad_hor(row, 12, 0);
    lv_obj_set_style_pad_column(row, 8, 0);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t * title = make_label(row, "波形", &stim_font_16, STIM_COLOR_MUTED);
    lv_obj_set_width(title, compact ? 110 : 120);

    for(index = 0U; index < 3U; ++index) {
        uintptr_t code = ((uintptr_t)screen << 4U) | index;
        ui.wave_buttons[screen][index] = make_button(row, wave_names[index], false, NULL);
        lv_obj_set_height(ui.wave_buttons[screen][index], STIM_TOUCH_MIN);
        lv_obj_set_flex_grow(ui.wave_buttons[screen][index], 1);
        lv_obj_add_event_cb(ui.wave_buttons[screen][index], wave_event, LV_EVENT_CLICKED,
                            (void *)code);
    }
    return row;
}
```

- [ ] **Step 3: Give the active wave button a gradient fill instead of flat blue, in `refresh_parameters`**

Replace the wave-button loop (orig `:435-444`):

```c
    for(index = 0U; index < 3U; ++index) {
        bool selected = index == (size_t)waveform;
        lv_obj_set_style_bg_color(ui.wave_buttons[screen][index],
                                  color(selected ? STIM_COLOR_BLUE : STIM_COLOR_DISABLED), 0);
        lv_obj_set_style_text_color(ui.wave_buttons[screen][index],
                                    color(selected ? 0xFFFFFFU : STIM_COLOR_TEXT), 0);
        lv_obj_t * label = lv_obj_get_child(ui.wave_buttons[screen][index], 0);
        lv_obj_set_style_text_color(label, color(selected ? 0xFFFFFFU : STIM_COLOR_TEXT), 0);
    }
```

with:

```c
    for(index = 0U; index < 3U; ++index) {
        bool selected = index == (size_t)waveform;
        lv_obj_t * label = lv_obj_get_child(ui.wave_buttons[screen][index], 0);

        lv_obj_set_style_bg_color(ui.wave_buttons[screen][index],
                                  color(selected ? STIM_COLOR_GRAD_START : STIM_COLOR_DISABLED), 0);
        lv_obj_set_style_bg_grad_color(ui.wave_buttons[screen][index], color(STIM_COLOR_GRAD_END), 0);
        lv_obj_set_style_bg_grad_dir(ui.wave_buttons[screen][index],
                                    selected ? LV_GRAD_DIR_HOR : LV_GRAD_DIR_NONE, 0);
        lv_obj_set_style_text_color(ui.wave_buttons[screen][index],
                                    color(selected ? 0xFFFFFFU : STIM_COLOR_TEXT), 0);
        lv_obj_set_style_text_color(label, color(selected ? 0xFFFFFFU : STIM_COLOR_TEXT), 0);
    }
```

(Explicitly resetting `bg_grad_dir` to `LV_GRAD_DIR_NONE` when not selected matters — these are long-lived button objects reused across refreshes, so a gradient set once must be cleared, not just overpainted with a flat color that a stale `grad_dir` would still blend with.)

- [ ] **Step 4: Bump +/- steppers to touch size and restyle the slider, in `create_parameter_row`**

Replace (orig `:841-895`):

```c
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
```

with:

```c
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

    lv_obj_set_size(row, LV_PCT(100), 52);
    lv_obj_set_style_pad_hor(row, 12, 0);
    lv_obj_set_style_pad_column(row, 8, 0);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    title_label = make_label(row, title, &stim_font_16, STIM_COLOR_MUTED);
    lv_obj_set_width(title_label, compact ? 110 : 120);

    minus = make_button(row, "-", false, NULL);
    lv_obj_set_size(minus, STIM_TOUCH_MIN, STIM_TOUCH_MIN);
    lv_obj_set_style_text_font(lv_obj_get_child(minus, 0), &lv_font_montserrat_20, 0);
    lv_obj_add_event_cb(minus, parameter_button_event, LV_EVENT_CLICKED, (void *)base);

    ui.parameters[screen][parameter].slider = lv_slider_create(row);
    lv_obj_set_height(ui.parameters[screen][parameter].slider, 8);
    lv_obj_set_flex_grow(ui.parameters[screen][parameter].slider, 1);
    lv_slider_set_range(ui.parameters[screen][parameter].slider,
                        parameter_min(screen, parameter), parameter_max(screen, parameter));
    lv_obj_set_style_radius(ui.parameters[screen][parameter].slider, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_radius(ui.parameters[screen][parameter].slider, LV_RADIUS_CIRCLE, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(ui.parameters[screen][parameter].slider,
                              color(STIM_COLOR_DISABLED), LV_PART_MAIN);
    lv_obj_set_style_bg_color(ui.parameters[screen][parameter].slider,
                              color(STIM_COLOR_GRAD_START), LV_PART_INDICATOR);
    lv_obj_set_style_bg_grad_color(ui.parameters[screen][parameter].slider,
                                   color(STIM_COLOR_GRAD_END), LV_PART_INDICATOR);
    lv_obj_set_style_bg_grad_dir(ui.parameters[screen][parameter].slider, LV_GRAD_DIR_HOR, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(ui.parameters[screen][parameter].slider,
                              color(STIM_COLOR_CARD), LV_PART_KNOB);
    lv_obj_set_style_border_width(ui.parameters[screen][parameter].slider, 2, LV_PART_KNOB);
    lv_obj_set_style_border_color(ui.parameters[screen][parameter].slider,
                                  color(STIM_COLOR_GRAD_END), LV_PART_KNOB);
    lv_obj_set_style_shadow_width(ui.parameters[screen][parameter].slider, 6, LV_PART_KNOB);
    lv_obj_set_style_shadow_color(ui.parameters[screen][parameter].slider, color(STIM_COLOR_SHADOW), LV_PART_KNOB);
    lv_obj_set_style_shadow_opa(ui.parameters[screen][parameter].slider, LV_OPA_20, LV_PART_KNOB);
    lv_obj_set_style_pad_all(ui.parameters[screen][parameter].slider, 9, LV_PART_KNOB);
    lv_obj_add_event_cb(ui.parameters[screen][parameter].slider, parameter_slider_event,
                        LV_EVENT_VALUE_CHANGED, (void *)slider_code);

    ui.parameters[screen][parameter].value_label =
        make_label(row, "--", &stim_font_16, STIM_COLOR_NAVY);
    lv_obj_set_width(ui.parameters[screen][parameter].value_label, compact ? 90 : 100);
    lv_obj_set_style_text_align(ui.parameters[screen][parameter].value_label, LV_TEXT_ALIGN_RIGHT, 0);

    plus = make_button(row, "+", false, NULL);
    lv_obj_set_size(plus, STIM_TOUCH_MIN, STIM_TOUCH_MIN);
    lv_obj_set_style_text_font(lv_obj_get_child(plus, 0), &lv_font_montserrat_20, 0);
    lv_obj_add_event_cb(plus, parameter_button_event, LV_EVENT_CLICKED, (void *)(base | 1U));
}
```

(Dropped the `border_side`/`border_color`/`border_width(0)` lines from the original row — they were setting a border side while width stayed 0, i.e. already a no-op; removing them is a same-behavior cleanup, not a functional change. The knob goes from a solid blue disc to a white disc with a colored ring and a faint shadow — same drag/click behavior, `parameter_slider_event` untouched.)

- [ ] **Step 5: Bump the apply/placement buttons to a uniform touch height, in `create_parameter_panel`**

Replace the action row buttons (orig `:926-939`):

```c
    if(screen == STIM_SCREEN_LOW) {
        lv_obj_t * placement = make_button(action_row, "查看贴敷位置", false, NULL);
        lv_obj_set_size(placement, 250, compact ? 42 : 48);
        lv_obj_add_event_cb(placement, placement_event, LV_EVENT_CLICKED, NULL);
    }

    apply = make_button(action_row,
                        screen == STIM_SCREEN_MEDIUM ? "应用到所选通道" : "配置到所选单元",
                        true, NULL);
    lv_obj_set_height(apply, compact ? 42 : 48);
```

with:

```c
    if(screen == STIM_SCREEN_LOW) {
        lv_obj_t * placement = make_button(action_row, "查看贴敷位置", false, NULL);
        lv_obj_set_size(placement, 250, 48);
        lv_obj_add_event_cb(placement, placement_event, LV_EVENT_CLICKED, NULL);
    }

    apply = make_button(action_row,
                        screen == STIM_SCREEN_MEDIUM ? "应用到所选通道" : "配置到所选单元",
                        true, NULL);
    lv_obj_set_height(apply, 48);
```

- [ ] **Step 6: Rebuild, run tests, screenshot both pages**

```bash
cd stim-simulator
cmake --build --preset macos-debug -j
ctest --preset macos-debug
./out/build/macos-debug/bin/stim-simulator --screen medium --screenshot /tmp/task5-medium.bmp
./out/build/macos-debug/bin/stim-simulator --screen low --screenshot /tmp/task5-low.bmp
```
Expected: build succeeds, `ctest` passes, prescription rows show a small gradient circular number badge, the active waveform button shows a gradient fill, +/- steppers and wave buttons are visibly ≥44px, and the slider shows a gradient track with a white ringed knob.

- [ ] **Step 7: Commit**

```bash
git add stim-simulator/src/ui/stim_ui.c
git commit -m "style: modernize prescription badges, wave buttons, sliders and steppers"
```

---

### Task 6: Full regression pass + regenerate committed screenshots

**Files:**
- No source changes. Regenerate: `stim-simulator/docs/screenshots/medium.png`, `stim-simulator/docs/screenshots/low.png` (referenced by `stim-simulator/README.md:20,24`)
- Test: full `ctest` run (regression)

**Interfaces:**
- Consumes: the finished UI from Tasks 1–5

- [ ] **Step 1: Full clean rebuild and test run**

```bash
cd stim-simulator
rm -rf out/build/macos-debug
cmake --preset macos-debug
cmake --build --preset macos-debug -j
ctest --preset macos-debug
```
Expected: clean configure + build succeeds under `-Werror`, all existing model tests in `tests/test_model.c` pass unchanged.

- [ ] **Step 2: Regenerate the two committed screenshots as PNG**

```bash
cd stim-simulator
./out/build/macos-debug/bin/stim-simulator --screen medium --screenshot docs/screenshots/medium.bmp
./out/build/macos-debug/bin/stim-simulator --screen low --screenshot docs/screenshots/low.bmp
sips -s format png docs/screenshots/medium.bmp --out docs/screenshots/medium.png
sips -s format png docs/screenshots/low.bmp --out docs/screenshots/low.png
rm docs/screenshots/medium.bmp docs/screenshots/low.bmp
```
(`sips` is the pre-installed macOS image tool; this mirrors how the currently-committed PNGs were produced, since `stim_sdl_platform_save_bmp` only writes BMP — see `src/main.c:81` / `src/platform/sdl_platform.h:21`.)

- [ ] **Step 3: Manual parity check against the 说明书 information architecture**

Open `docs/screenshots/medium.png` and `docs/screenshots/low.png` side by side with the originals in `output/ui-design/stim-ui-medium-frequency-1280x800.png` / `stim-ui-low-frequency-1280x800.png` and confirm: same tab count (2), same channel count (4) / receiver count (12), same state vocabulary (就绪/治疗中/已暂停/离线/未配置 for channels; 已配置/治疗中/待配置/离线 + 锁定 for receivers), same prescription list (4 rows), same parameter fields per screen (4 sliders + waveform row), same primary/secondary actions ("应用到所选通道"/"配置到所选单元", "查看贴敷位置", "全选"). Nothing added or removed — only chrome changed. If anything is missing, that's a regression, not a design choice — go back and fix the task that dropped it.

- [ ] **Step 4: Commit the regenerated screenshots**

```bash
git add stim-simulator/docs/screenshots/medium.png stim-simulator/docs/screenshots/low.png
git commit -m "docs: regenerate simulator screenshots for the modernized UI"
```

---

## Self-Review Notes (already applied above, kept here for the record)

- **Spec coverage:** every bullet in `output/ui-design/stim-ui-design-spec.md`'s "v2 现代化视觉方案" section maps to a task — design tokens/shadow/pill buttons → Task 1; Tab/gradient/图标 → Task 2; badge headers + icon buttons + touch sizing on channel cards → Task 3; badges + wifi icon + touch sizing on receiver tiles → Task 4; prescription/wave/slider/stepper/apply touch sizing → Task 5; screenshot regen + parity check → Task 6. The "不做的事" list (no layout/IA change, no dark mode, no new font weights, no new animation system, no treatment-parameter changes) has no corresponding task by design — confirmed no task above touches `src/model/*`, adds a font, or adds an `lv_anim_*` call.
- **Placeholder scan:** no TBD/TODO, no "similar to Task N" shorthand — every step embeds the literal before/after C code.
- **Type consistency:** `make_icon_text_button`'s signature (`lv_obj_t * parent, const char * icon, const char * text, bool primary, lv_obj_t ** label_out`) is used identically in all three Task 3 call sites; `style_badge`'s signature (`lv_obj_t * label, uint32_t fg, uint32_t bg`) matches across Task 3 and Task 4 call sites; `receiver_view_t.link_icon` is declared in Task 4 Step 1 and consumed only in Task 4 Steps 2–3 (same task, no dangling reference). `STIM_COLOR_PRESSED` was considered during design and deliberately dropped in favor of a single `LV_OPA_80`-on-press rule used consistently by both `make_button` and `make_icon_text_button` — no unused macro left behind.
- **Duplication caught before dispatch:** the first draft of Task 3 had `make_icon_text_button` repeat `make_button`'s entire pill/gradient/press-opacity style block verbatim. Extracted that block into `apply_button_style(lv_obj_t * button, bool primary)` in Task 1 (used immediately by `make_button` in the same task, so no unused-function risk), and Task 3's `make_icon_text_button` now calls it instead of duplicating it.
