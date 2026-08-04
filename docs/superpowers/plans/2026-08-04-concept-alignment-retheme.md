# Concept-Alignment Retheme Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Bring the LVGL simulator UI back in line with this project's own original concept mockups (`output/ui-design/stim-ui-*-1280x800.png`) — cool-blue palette, solid navy header bars, blue-bordered cards, and circular icon status badges — after two successive restyle rounds (v2 soft-health, v3 Material) drifted away from them.

**Architecture:** A value-and-structure revert of the same shared style helpers and card builders the previous rounds established. No new source files. Three tasks: (1) tokens + panel/header chrome, (2) circular icon badges across both card types, (3) remaining reference icons + screenshot regeneration.

**Tech Stack:** C11, LVGL v9.5.0 (vendored at `stim-simulator/out/build/macos-debug/_deps/lvgl-src`), SDL2, CMake presets, ctest.

## Global Constraints

- Do not change `src/model/*`, page structure, tab count, card counts, or any treatment parameter default/range. Visual-only (see `output/ui-design/stim-ui-design-spec.md`, section "v4 回归概念稿方案").
- **No new font assets.** Every icon must be an LVGL built-in `LV_SYMBOL_*`. All symbols named in this plan were verified present in `_deps/lvgl-src/src/font/lv_symbol_def.h`. Icons render only via `lv_font_montserrat_*`-styled labels — the Chinese subset fonts (`stim_font_16/20/24`) contain **no** symbol glyphs, so an icon and its Chinese text must always be two separate labels.
- **No new Chinese characters in any UI string.** The Chinese fonts are pre-generated subsets containing only characters currently used; a new character renders as a blank gap. This plan introduces no new Chinese text.
- The build enables `-Wall -Wextra -Wpedantic -Werror`. An unused `static` **function** is a hard build failure (macros are exempt). Any helper must be introduced in the same task that first calls it, and the last removal of a function's final call site must delete the function in that same task.
- Minimum touch target 44×44 px (`STIM_TOUCH_MIN`) for anything tappable. This plan changes no control's size — inherited automatically.
- Slider knob keeps its current small visual size (`pad_all` 6 on `LV_PART_KNOB`) plus `lv_obj_set_ext_click_area(slider, 14)` compensation. Do not revert that.
- **Adapted verification, not classic TDD:** there is no meaningful unit-testable assertion for LVGL color/border/icon values. Each task's "test" step is: (1) clean rebuild under `-Werror`, (2) `ctest` still passes (regression guard over the untouched model layer), (3) a screenshot is generated and visually reviewed.
- Build/verify commands, run from `stim-simulator/`:
  ```bash
  cmake --build --preset macos-debug -j
  ctest --preset macos-debug
  ./out/build/macos-debug/bin/stim-simulator --screen medium --screenshot /tmp/m.bmp
  ./out/build/macos-debug/bin/stim-simulator --screen low --screenshot /tmp/l.bmp
  ```
  Screenshots are 32bpp `BITMAPV4HEADER`/`BI_BITFIELDS` BMP. **`sips` cannot decode this variant** (fails with "Cannot extract image from file"). To view one, either Read the `.bmp` directly (the Read tool renders BMP), or convert with the stdlib-only Python snippet given in Task 3 Step 6.

---

### Task 1: Cool-blue tokens, bordered panels, solid navy header bars

**Files:**
- Modify: `stim-simulator/src/ui/stim_ui.c` — macro block (`:8-31`), `make_panel` (`:127-143`), `make_section_header` (`:228-239`), its two call sites in `create_prescription_panel` (`:904`) and `create_parameter_panel` (`:1022`), `create_header` (`:716-772`), `set_tab_active` (`:1210-1224`)
- Test: none (visual-only) — verify via build + `ctest` + screenshot

**Interfaces:**
- Consumes: nothing new
- Produces: same macro names with reverted cool-blue values; `STIM_PANEL_RADIUS` becomes `12`. **`make_section_header` changes signature** from `(lv_obj_t * parent, const char * title)` to `(lv_obj_t * parent, const char * icon, const char * title)` — both existing call sites are updated in this task. Later tasks do not call it.

- [ ] **Step 1: Revert the palette to the original cool-blue tokens**

Replace `stim-simulator/src/ui/stim_ui.c:8-31`:

```c
#define STIM_COLOR_BG 0xF1F3F5U
#define STIM_COLOR_CARD 0xFFFFFFU
#define STIM_COLOR_NAVY 0x1976D2U
#define STIM_COLOR_BLUE 0x1976D2U
#define STIM_COLOR_CYAN 0x16A6C9U
#define STIM_COLOR_TEAL 0x00897BU
#define STIM_COLOR_CORAL 0xD32F2FU
#define STIM_COLOR_AMBER 0xEF6C00U
#define STIM_COLOR_TEXT 0x212121U
#define STIM_COLOR_MUTED 0x757575U
#define STIM_COLOR_BORDER 0xBDBDBDU
#define STIM_COLOR_DISABLED 0xEEEEEEU
#define STIM_COLOR_SELECTED 0xE3F2FDU
#define STIM_COLOR_PAUSED_SOFT 0xFDF0DFU
#define STIM_COLOR_RUNNING_SOFT 0xE0F2F1U
#define STIM_COLOR_SHADOW 0x000000U

#define STIM_HEADER_HEIGHT 64
#define STIM_PAGE_TOP 64
#define STIM_PAGE_HEIGHT 736
#define STIM_PANEL_RADIUS 16
#define STIM_METRIC_RADIUS 8
#define STIM_BUTTON_RADIUS 8
#define STIM_TOUCH_MIN 44
```

with:

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
#define STIM_COLOR_PAUSED_SOFT 0xFBF1DFU
#define STIM_COLOR_RUNNING_SOFT 0xE8F6F5U
#define STIM_COLOR_SHADOW 0x0B2F4FU

#define STIM_HEADER_HEIGHT 64
#define STIM_PAGE_TOP 64
#define STIM_PAGE_HEIGHT 736
#define STIM_PANEL_RADIUS 12
#define STIM_METRIC_RADIUS 8
#define STIM_BUTTON_RADIUS 8
#define STIM_TOUCH_MIN 44
```

`STIM_COLOR_NAVY` and `STIM_COLOR_BLUE` become two **different** values again (they were unified during v2/v3): NAVY is the darker bar color, BLUE the accent/primary. `STIM_COLOR_SHADOW` moves from pure black to a dark blue-grey so the (now weaker) shadow stays in the cool family. `STIM_COLOR_CYAN` remains defined-but-unused, exactly as before — leave it.

- [ ] **Step 2: Restore the card border and weaken the shadow, in `make_panel`**

Replace `stim-simulator/src/ui/stim_ui.c:127-143`:

```c
static lv_obj_t * make_panel(lv_obj_t * parent)
{
    lv_obj_t * panel = lv_obj_create(parent);

    lv_obj_set_style_bg_color(panel, color(STIM_COLOR_CARD), 0);
    lv_obj_set_style_bg_opa(panel, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(panel, 0, 0);
    lv_obj_set_style_radius(panel, STIM_PANEL_RADIUS, 0);
    lv_obj_set_style_pad_all(panel, 0, 0);
    lv_obj_set_style_shadow_width(panel, 20, 0);
    lv_obj_set_style_shadow_offset_y(panel, 6, 0);
    lv_obj_set_style_shadow_spread(panel, 0, 0);
    lv_obj_set_style_shadow_color(panel, color(STIM_COLOR_SHADOW), 0);
    lv_obj_set_style_shadow_opa(panel, LV_OPA_20, 0);
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
    lv_obj_set_style_shadow_width(panel, 12, 0);
    lv_obj_set_style_shadow_offset_y(panel, 2, 0);
    lv_obj_set_style_shadow_spread(panel, 0, 0);
    lv_obj_set_style_shadow_color(panel, color(STIM_COLOR_SHADOW), 0);
    lv_obj_set_style_shadow_opa(panel, LV_OPA_10, 0);
    no_scroll(panel);
    return panel;
}
```

The 1px border returns as the primary edge definition (concept style); the shadow drops to a subtle lift. The `allow_child_shadow_overflow(...)` calls added in a prior round stay untouched — a smaller shadow simply uses less of the 24px allowance they grant.

- [ ] **Step 3: Give `make_section_header` a navy bar, white text, and a leading icon**

Replace `stim-simulator/src/ui/stim_ui.c:228-239`:

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

with:

```c
static lv_obj_t * make_section_header(lv_obj_t * parent, const char * icon, const char * title)
{
    lv_obj_t * header = make_plain(parent);

    lv_obj_set_size(header, LV_PCT(100), 44);
    lv_obj_set_style_bg_color(header, color(STIM_COLOR_NAVY), 0);
    lv_obj_set_style_bg_opa(header, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_left(header, 16, 0);
    lv_obj_set_style_pad_column(header, 10, 0);
    lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    (void)make_label(header, icon, &lv_font_montserrat_16, 0xFFFFFFU);
    (void)make_label(header, title, &stim_font_20, 0xFFFFFFU);
    return header;
}
```

The icon gets its own `lv_font_montserrat_16` label because `stim_font_20` (Chinese subset) has no symbol glyphs.

- [ ] **Step 4: Update both `make_section_header` call sites and enable corner clipping on their panels**

In `create_prescription_panel`, replace this line (originally `stim-simulator/src/ui/stim_ui.c:904`):

```c
    (void)make_section_header(panel, "处方");
```

with:

```c
    lv_obj_set_style_clip_corner(panel, true, 0);
    (void)make_section_header(panel, LV_SYMBOL_FILE, "处方");
```

In `create_parameter_panel`, replace this line (originally `stim-simulator/src/ui/stim_ui.c:1022`):

```c
    (void)make_section_header(panel, screen == STIM_SCREEN_MEDIUM ? "处方参数" : "批量配置");
```

with:

```c
    lv_obj_set_style_clip_corner(panel, true, 0);
    (void)make_section_header(panel, LV_SYMBOL_SETTINGS,
                              screen == STIM_SCREEN_MEDIUM ? "处方参数" : "批量配置");
```

`clip_corner` makes LVGL clip the panel's children to its rounded rectangle (verified in `_deps/lvgl-src/src/core/lv_refr.c` — when `clip_corner` is set and radius is non-zero, children are clipped to the rounded corners). Without it, the square-cornered navy header would visibly overhang the panel's rounded top corners. `LV_SYMBOL_SETTINGS` (gear) is the closest built-in to the concept's slider-adjust glyph; there is no built-in equivalent for that exact icon.

- [ ] **Step 5: Make the top header bar solid navy, in `create_header`**

Replace `stim-simulator/src/ui/stim_ui.c:726-745` (the header styling plus the tabs-track block, keeping the `spacer` lines between them unchanged):

```c
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
    lv_obj_set_size(tabs, 400, 56);
    lv_obj_set_style_bg_color(tabs, color(STIM_COLOR_DISABLED), 0);
    lv_obj_set_style_bg_opa(tabs, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(tabs, STIM_BUTTON_RADIUS, 0);
    lv_obj_set_style_pad_all(tabs, 4, 0);
    lv_obj_set_flex_flow(tabs, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(tabs, 4, 0);
```

with:

```c
    lv_obj_set_style_bg_color(header, color(STIM_COLOR_NAVY), 0);
    lv_obj_set_style_bg_opa(header, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(header, 0, 0);
    lv_obj_set_style_pad_hor(header, 16, 0);
    lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    spacer = make_plain(header);
    lv_obj_set_size(spacer, 205, 1);

    tabs = make_plain(header);
    lv_obj_set_size(tabs, 400, 56);
    lv_obj_set_style_pad_all(tabs, 4, 0);
    lv_obj_set_flex_flow(tabs, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_column(tabs, 4, 0);
```

The tabs track loses its own background entirely — the tabs now sit directly on the navy bar, and only the *active* tab gets a light fill (Step 6).

Then, in the same function, replace the clock and settings lines (originally `stim-simulator/src/ui/stim_ui.c:766-771`):

```c
    (void)make_label(right, "14:28", &lv_font_montserrat_24, STIM_COLOR_TEXT);

    settings = make_button(right, LV_SYMBOL_SETTINGS, false, NULL);
    lv_obj_set_size(settings, STIM_TOUCH_MIN, STIM_TOUCH_MIN);
    lv_obj_set_style_text_font(lv_obj_get_child(settings, 0), &lv_font_montserrat_20, 0);
    lv_obj_add_event_cb(settings, settings_event, LV_EVENT_CLICKED, NULL);
```

with:

```c
    (void)make_label(right, "14:28", &lv_font_montserrat_24, 0xFFFFFFU);

    settings = make_button(right, LV_SYMBOL_SETTINGS, false, NULL);
    lv_obj_set_size(settings, STIM_TOUCH_MIN, STIM_TOUCH_MIN);
    lv_obj_set_style_bg_opa(settings, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(settings, 0, 0);
    lv_obj_set_style_text_color(settings, color(0xFFFFFFU), 0);
    lv_obj_set_style_text_font(lv_obj_get_child(settings, 0), &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(lv_obj_get_child(settings, 0), color(0xFFFFFFU), 0);
    lv_obj_add_event_cb(settings, settings_event, LV_EVENT_CLICKED, NULL);
```

Clock and gear turn white to read on navy; the gear button drops its white pill background so it reads as a bare icon on the bar, matching the concept.

- [ ] **Step 6: Invert the active-tab treatment, in `set_tab_active`**

Replace `stim-simulator/src/ui/stim_ui.c:1210-1224`:

```c
static void set_tab_active(lv_obj_t * tab, bool active)
{
    lv_obj_t * label = lv_obj_get_child(tab, 0);

    if(active) {
        lv_obj_set_style_bg_color(tab, color(STIM_COLOR_BLUE), 0);
        lv_obj_set_style_bg_opa(tab, LV_OPA_COVER, 0);
    }
    else {
        lv_obj_set_style_bg_opa(tab, LV_OPA_TRANSP, 0);
    }
    lv_obj_set_style_text_color(tab, color(active ? 0xFFFFFFU : STIM_COLOR_MUTED), 0);
    lv_obj_set_style_text_color(label, color(active ? 0xFFFFFFU : STIM_COLOR_MUTED), 0);
}
```

with:

```c
static void set_tab_active(lv_obj_t * tab, bool active)
{
    lv_obj_t * label = lv_obj_get_child(tab, 0);
    uint32_t text_color = active ? STIM_COLOR_NAVY : 0xFFFFFFU;

    lv_obj_set_style_bg_color(tab, color(STIM_COLOR_CARD), 0);
    lv_obj_set_style_bg_opa(tab, active ? LV_OPA_COVER : LV_OPA_TRANSP, 0);
    lv_obj_set_style_text_color(tab, color(text_color), 0);
    lv_obj_set_style_text_color(label, color(text_color), 0);
}
```

Inverted from the current scheme: the bar is navy, so the *active* tab is a light card-colored block with navy text, and inactive tabs are transparent with white text.

- [ ] **Step 7: Rebuild, run tests, generate a checkpoint screenshot**

```bash
cd stim-simulator
cmake --build --preset macos-debug -j
ctest --preset macos-debug
./out/build/macos-debug/bin/stim-simulator --screen medium --screenshot /tmp/task1-medium.bmp
```
Expected: build succeeds under `-Werror`, `ctest` passes. The screenshot shows a solid navy top bar with a white active-tab block and white clock/gear; panels have a visible 1px cool-grey border and a much subtler shadow; the "处方" / "处方参数" panel headings are now solid navy bars with a white icon and white text, with their top corners clipped to the panel's rounded corners. Channel-card headers and status badges still look like the previous round — Task 2 changes those.

- [ ] **Step 8: Commit**

```bash
git add stim-simulator/src/ui/stim_ui.c
git commit -m "style: revert to cool-blue palette with navy header bars and bordered panels"
```

---

### Task 2: Circular icon status badges on channel and receiver cards

**Files:**
- Modify: `stim-simulator/src/ui/stim_ui.c` — `channel_view_t` (`:33-45`, add one field), `receiver_view_t` (`:47-56`, replace one field, add two), `style_badge` (`:190-198`, replaced by two new helpers), `set_card_elevation` (`:275-280`, deleted), `refresh_channel` (`:282-339`), `refresh_receiver` (`:350-418`), `create_channel_card` (`:794-860`), `create_receiver_card` (`:1088-1124`)
- Test: none (visual-only) — verify via build + `ctest` + screenshot

**Interfaces:**
- Consumes: the Task 1 palette (`STIM_COLOR_NAVY`/`BLUE`/`TEAL`/`AMBER`/`MUTED`/`SELECTED`/`RUNNING_SOFT`/`PAUSED_SOFT`)
- Produces: three new static helpers — `make_icon_badge(lv_obj_t * parent, int32_t size)`, `set_icon_badge(lv_obj_t * badge, const char * icon, uint32_t bg, lv_opa_t bg_opa)`, and `battery_symbol(uint8_t percent)` — all three defined and called within this task. `style_badge` and `set_card_elevation` are both deleted here (their last call sites disappear in this task; leaving either would fail the `-Werror` unused-function check).

- [ ] **Step 1: Add the new view fields**

Replace `stim-simulator/src/ui/stim_ui.c:33-56` (both structs):

```c
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
    lv_obj_t * link_icon;
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
    lv_obj_t * header;
    lv_obj_t * title_label;
    lv_obj_t * state_badge;
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
    lv_obj_t * state_badge;
    lv_obj_t * link_icon;
    lv_obj_t * battery_icon;
    lv_obj_t * link_label;
    lv_obj_t * prescription_label;
    lv_obj_t * time_label;
    lv_obj_t * state_dot;
    lv_obj_t * state_label;
} receiver_view_t;
```

`receiver_view_t.selection_label` (which held the "已选" text) is replaced by `state_badge`; `battery_icon` and `state_dot` are new.

- [ ] **Step 2: Replace `style_badge` with the circular icon-badge helpers and the battery mapper**

Replace `stim-simulator/src/ui/stim_ui.c:190-198`:

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
```

with:

```c
static lv_obj_t * make_icon_badge(lv_obj_t * parent, int32_t size)
{
    lv_obj_t * badge = make_plain(parent);
    lv_obj_t * icon;

    lv_obj_set_size(badge, size, size);
    lv_obj_set_style_radius(badge, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_opa(badge, LV_OPA_COVER, 0);

    icon = make_label(badge, LV_SYMBOL_MINUS, &lv_font_montserrat_14, 0xFFFFFFU);
    lv_obj_center(icon);
    return badge;
}

static void set_icon_badge(lv_obj_t * badge, const char * icon, uint32_t bg, lv_opa_t bg_opa)
{
    lv_obj_set_style_bg_color(badge, color(bg), 0);
    lv_obj_set_style_bg_opa(badge, bg_opa, 0);
    lv_label_set_text(lv_obj_get_child(badge, 0), icon);
}

static const char * battery_symbol(uint8_t percent)
{
    if(percent >= 90U) {
        return LV_SYMBOL_BATTERY_FULL;
    }
    if(percent >= 70U) {
        return LV_SYMBOL_BATTERY_3;
    }
    if(percent >= 45U) {
        return LV_SYMBOL_BATTERY_2;
    }
    if(percent >= 20U) {
        return LV_SYMBOL_BATTERY_1;
    }
    return LV_SYMBOL_BATTERY_EMPTY;
}
```

The badge's single child is its icon label, always `lv_font_montserrat_14`; `set_icon_badge` reaches it via `lv_obj_get_child(badge, 0)`. `bg_opa` is a parameter because a badge sitting on a **colored** header needs a translucent white fill, while one on a white card needs a solid state color.

- [ ] **Step 3: Delete `set_card_elevation`**

Delete this entire function from `stim-simulator/src/ui/stim_ui.c:275-280`:

```c
static void set_card_elevation(lv_obj_t * card, bool elevated)
{
    lv_obj_set_style_shadow_opa(card, elevated ? LV_OPA_30 : LV_OPA_20, 0);
    lv_obj_set_style_shadow_width(card, elevated ? 28 : 20, 0);
    lv_obj_set_style_shadow_offset_y(card, elevated ? 8 : 6, 0);
}
```

Its two call sites are removed in Steps 4 and 5 below; state emphasis moves back to colored borders. Delete it in this same task or the build fails with an unused-function error.

- [ ] **Step 4: Rewrite `refresh_channel` for colored headers, icon badges, and colored borders**

Replace `stim-simulator/src/ui/stim_ui.c:282-339`:

```c
static void refresh_channel(size_t index)
{
    stim_channel_t * channel = &ui.model->channels[index];
    channel_view_t * view = &ui.channels[index];
    char buffer[32];
    uint32_t badge_fg = STIM_COLOR_MUTED;
    uint32_t badge_bg = STIM_COLOR_DISABLED;
    uint32_t card_bg = STIM_COLOR_CARD;
    bool elevated = false;
    bool configured = channel->state != STIM_STATE_UNCONFIGURED;
    bool active = (channel->state == STIM_STATE_RUNNING) || (channel->state == STIM_STATE_PAUSED);

    if(channel->state == STIM_STATE_RUNNING) {
        badge_fg = 0xFFFFFFU;
        badge_bg = STIM_COLOR_TEAL;
        card_bg = STIM_COLOR_RUNNING_SOFT;
        elevated = true;
    }
    else if(channel->state == STIM_STATE_PAUSED) {
        badge_fg = 0xFFFFFFU;
        badge_bg = STIM_COLOR_AMBER;
        card_bg = STIM_COLOR_PAUSED_SOFT;
        elevated = true;
    }
    else if(channel->selected) {
        badge_fg = 0xFFFFFFU;
        badge_bg = STIM_COLOR_BLUE;
        card_bg = STIM_COLOR_SELECTED;
        elevated = true;
    }

    style_badge(view->state_label, badge_fg, badge_bg);
    lv_obj_set_style_bg_color(view->card, color(card_bg), 0);
    set_card_elevation(view->card, elevated);

    (void)snprintf(buffer, sizeof(buffer), "%c通道", channel->id);
    lv_label_set_text(view->title_label, buffer);
    lv_label_set_text(view->state_label, unit_state_text(channel->state));
    lv_label_set_text(view->prescription_label,
                      configured && (channel->prescription != NULL) ? channel->prescription : "未配置");
```

with:

```c
static void refresh_channel(size_t index)
{
    stim_channel_t * channel = &ui.model->channels[index];
    channel_view_t * view = &ui.channels[index];
    char buffer[32];
    const char * badge_icon = LV_SYMBOL_MINUS;
    uint32_t header_bg = STIM_COLOR_NAVY;
    uint32_t card_bg = STIM_COLOR_CARD;
    uint32_t border_color = STIM_COLOR_BORDER;
    int32_t border_width = 1;
    bool configured = channel->state != STIM_STATE_UNCONFIGURED;
    bool active = (channel->state == STIM_STATE_RUNNING) || (channel->state == STIM_STATE_PAUSED);

    if(channel->state == STIM_STATE_RUNNING) {
        badge_icon = LV_SYMBOL_PLAY;
        header_bg = STIM_COLOR_TEAL;
        card_bg = STIM_COLOR_RUNNING_SOFT;
        border_color = STIM_COLOR_TEAL;
        border_width = 2;
    }
    else if(channel->state == STIM_STATE_PAUSED) {
        badge_icon = LV_SYMBOL_PAUSE;
        header_bg = STIM_COLOR_AMBER;
        card_bg = STIM_COLOR_PAUSED_SOFT;
        border_color = STIM_COLOR_AMBER;
        border_width = 2;
    }
    else if(channel->state == STIM_STATE_READY) {
        badge_icon = LV_SYMBOL_OK;
    }

    if(channel->selected && !active) {
        card_bg = STIM_COLOR_SELECTED;
        border_color = STIM_COLOR_BLUE;
        border_width = 2;
    }

    set_icon_badge(view->state_badge, badge_icon, 0xFFFFFFU, LV_OPA_30);
    lv_obj_set_style_bg_color(view->header, color(header_bg), 0);
    lv_obj_set_style_bg_color(view->card, color(card_bg), 0);
    lv_obj_set_style_border_color(view->card, color(border_color), 0);
    lv_obj_set_style_border_width(view->card, border_width, 0);

    (void)snprintf(buffer, sizeof(buffer), "%c通道", channel->id);
    lv_label_set_text(view->title_label, buffer);
    lv_label_set_text(view->state_label, unit_state_text(channel->state));
    lv_label_set_text(view->prescription_label,
                      configured && (channel->prescription != NULL) ? channel->prescription : "未配置");
```

Leave the rest of the function (from `if(configured) {` through the closing brace) exactly as it is. Note the state→icon mapping is now explicit for all four cases: RUNNING→play, PAUSED→pause, READY→check, UNCONFIGURED→minus (the initializer default). Selection is a separate, additive concern — it recolors the border and card background but never overrides a running/paused card's own colors, which is why it is a second `if` rather than another `else if`.

- [ ] **Step 5: Rewrite `refresh_receiver` for icon badges, battery icon, and colored borders**

Replace `stim-simulator/src/ui/stim_ui.c:350-418` — the whole `refresh_receiver` function:

```c
static void refresh_receiver(size_t index)
{
    stim_receiver_t * receiver = &ui.model->receivers[index];
    receiver_view_t * view = &ui.receivers[index];
    char buffer[48];
    uint32_t background = STIM_COLOR_CARD;
    uint32_t badge_fg = STIM_COLOR_MUTED;
    uint32_t badge_bg = STIM_COLOR_DISABLED;
    bool elevated = false;
    bool configured = (receiver->state != STIM_STATE_UNCONFIGURED) &&
                      (receiver->state != STIM_STATE_OFFLINE);

    if(receiver->state == STIM_STATE_RUNNING) {
        background = STIM_COLOR_RUNNING_SOFT;
        badge_fg = 0xFFFFFFU;
        badge_bg = STIM_COLOR_TEAL;
        elevated = true;
    }
    else if(receiver->selected) {
        background = STIM_COLOR_SELECTED;
        badge_fg = 0xFFFFFFU;
        badge_bg = STIM_COLOR_BLUE;
        elevated = true;
    }
    else if(receiver->state == STIM_STATE_OFFLINE) {
        background = STIM_COLOR_DISABLED;
    }

    lv_obj_set_style_bg_color(view->card, color(background), 0);
    set_card_elevation(view->card, elevated);
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
        lv_label_set_text(view->link_label, "USB --  电量 --");
        lv_label_set_text(view->prescription_label, "离线");
        lv_label_set_text(view->time_label, "--:--");
    }
    else {
        (void)snprintf(buffer, sizeof(buffer), "USB已连接  电量 %u%%", receiver->battery_percent);
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

with:

```c
static void refresh_receiver(size_t index)
{
    stim_receiver_t * receiver = &ui.model->receivers[index];
    receiver_view_t * view = &ui.receivers[index];
    char buffer[48];
    const char * badge_icon = LV_SYMBOL_MINUS;
    uint32_t badge_bg = STIM_COLOR_MUTED;
    uint32_t background = STIM_COLOR_CARD;
    uint32_t border_color = STIM_COLOR_BORDER;
    uint32_t accent = STIM_COLOR_MUTED;
    int32_t border_width = 1;
    bool offline = receiver->state == STIM_STATE_OFFLINE;
    bool configured = (receiver->state != STIM_STATE_UNCONFIGURED) && !offline;

    if(receiver->state == STIM_STATE_RUNNING) {
        badge_icon = LV_SYMBOL_PLAY;
        badge_bg = STIM_COLOR_TEAL;
        background = STIM_COLOR_RUNNING_SOFT;
        border_color = STIM_COLOR_TEAL;
        border_width = 2;
        accent = STIM_COLOR_TEAL;
    }
    else if(receiver->selected) {
        badge_icon = LV_SYMBOL_OK;
        badge_bg = STIM_COLOR_BLUE;
        background = STIM_COLOR_SELECTED;
        border_color = STIM_COLOR_BLUE;
        border_width = 2;
        accent = STIM_COLOR_BLUE;
    }
    else if(offline) {
        background = STIM_COLOR_DISABLED;
    }

    set_icon_badge(view->state_badge, badge_icon, badge_bg, LV_OPA_COVER);
    lv_obj_set_style_bg_color(view->card, color(background), 0);
    lv_obj_set_style_border_color(view->card, color(border_color), 0);
    lv_obj_set_style_border_width(view->card, border_width, 0);
    lv_obj_set_style_text_opa(view->card, offline ? LV_OPA_60 : LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(view->link_icon,
                                color(offline ? STIM_COLOR_MUTED : STIM_COLOR_TEAL), 0);
    lv_obj_set_style_text_color(view->battery_icon,
                                color(offline ? STIM_COLOR_MUTED : STIM_COLOR_TEAL), 0);
    lv_obj_set_style_text_color(view->state_dot, color(accent), 0);

    (void)snprintf(buffer, sizeof(buffer), "%02u号", receiver->id);
    lv_label_set_text(view->id_label, buffer);

    if(offline) {
        lv_label_set_text(view->battery_icon, LV_SYMBOL_BATTERY_EMPTY);
        lv_label_set_text(view->link_label, "USB --  电量 --");
        lv_label_set_text(view->prescription_label, "离线");
        lv_label_set_text(view->time_label, "--:--");
    }
    else {
        lv_label_set_text(view->battery_icon, battery_symbol(receiver->battery_percent));
        (void)snprintf(buffer, sizeof(buffer), "USB已连接  电量 %u%%", receiver->battery_percent);
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

The `RUNNING > selected > offline` priority chain is preserved exactly. The old `selection_label` ("已选" text) is gone — selection now reads from the check-mark badge plus the blue border. `accent` colors the bullet in front of the bottom state text.

- [ ] **Step 6: Rebuild the channel card header with a colored bar, white title, badge and state text**

Replace `stim-simulator/src/ui/stim_ui.c:810-818` (inside `create_channel_card`):

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

with:

```c
    lv_obj_set_style_clip_corner(view->card, true, 0);

    view->header = make_plain(view->card);
    lv_obj_set_size(view->header, LV_PCT(100), 48);
    lv_obj_set_style_bg_color(view->header, color(STIM_COLOR_NAVY), 0);
    lv_obj_set_style_bg_opa(view->header, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_hor(view->header, 14, 0);
    lv_obj_set_flex_flow(view->header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(view->header, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    view->title_label = make_label(view->header, "A通道", &stim_font_20, 0xFFFFFFU);

    state_group = make_plain(view->header);
    lv_obj_set_size(state_group, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(state_group, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(state_group, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(state_group, 6, 0);
    view->state_badge = make_icon_badge(state_group, 22);
    view->state_label = make_label(state_group, "就绪", &stim_font_16, 0xFFFFFFU);
```

and add `state_group` to the function's local declarations — replace `stim-simulator/src/ui/stim_ui.c:797-799`:

```c
    lv_obj_t * body;
    lv_obj_t * metrics;
    lv_obj_t * controls;
```

with:

```c
    lv_obj_t * body;
    lv_obj_t * metrics;
    lv_obj_t * controls;
    lv_obj_t * state_group;
```

`clip_corner` on the card keeps the square-cornered colored header inside the card's 12px rounded corners.

- [ ] **Step 7: Rebuild the receiver card with a corner badge, battery icon, and bulleted state row**

Replace `stim-simulator/src/ui/stim_ui.c:1103-1123` (inside `create_receiver_card`, from `title_row = ...` through the final `style_badge(...)` line):

```c
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
    view->link_icon = make_label(link_row, LV_SYMBOL_USB, &lv_font_montserrat_14, STIM_COLOR_MUTED);
    view->link_label = make_label(link_row, "USB已连接  电量 90%", &stim_font_16, STIM_COLOR_MUTED);

    view->prescription_label = make_label(view->card, "待配置", &stim_font_16, STIM_COLOR_TEXT);
    lv_obj_set_width(view->prescription_label, LV_PCT(100));
    view->time_label = make_label(view->card, "--:--", &lv_font_montserrat_20, STIM_COLOR_NAVY);
    view->state_label = make_label(view->card, "待配置", &stim_font_16, STIM_COLOR_MUTED);
    style_badge(view->state_label, STIM_COLOR_MUTED, STIM_COLOR_DISABLED);
```

with:

```c
    title_row = make_plain(view->card);
    lv_obj_set_size(title_row, LV_PCT(100), 24);
    lv_obj_set_flex_flow(title_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(title_row, LV_FLEX_ALIGN_SPACE_BETWEEN,
                          LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    view->id_label = make_label(title_row, "01号", &stim_font_20, STIM_COLOR_NAVY);
    view->state_badge = make_icon_badge(title_row, 22);

    link_row = make_plain(view->card);
    lv_obj_set_size(link_row, LV_PCT(100), 16);
    lv_obj_set_flex_flow(link_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(link_row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(link_row, 4, 0);
    view->link_icon = make_label(link_row, LV_SYMBOL_USB, &lv_font_montserrat_14, STIM_COLOR_TEAL);
    view->battery_icon = make_label(link_row, LV_SYMBOL_BATTERY_FULL, &lv_font_montserrat_14,
                                    STIM_COLOR_TEAL);
    view->link_label = make_label(link_row, "USB已连接  电量 90%", &stim_font_16, STIM_COLOR_MUTED);

    view->prescription_label = make_label(view->card, "待配置", &stim_font_16, STIM_COLOR_TEXT);
    lv_obj_set_width(view->prescription_label, LV_PCT(100));
    view->time_label = make_label(view->card, "--:--", &lv_font_montserrat_20, STIM_COLOR_NAVY);

    state_row = make_plain(view->card);
    lv_obj_set_size(state_row, LV_PCT(100), 18);
    lv_obj_set_flex_flow(state_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(state_row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(state_row, 5, 0);
    view->state_dot = make_label(state_row, LV_SYMBOL_BULLET, &lv_font_montserrat_14, STIM_COLOR_MUTED);
    view->state_label = make_label(state_row, "待配置", &stim_font_16, STIM_COLOR_MUTED);
```

and add `state_row` to the function's locals — replace `stim-simulator/src/ui/stim_ui.c:1091-1092`:

```c
    lv_obj_t * title_row;
    lv_obj_t * link_row;
```

with:

```c
    lv_obj_t * title_row;
    lv_obj_t * link_row;
    lv_obj_t * state_row;
```

The card is 197×120 with `pad_all` 8 and `pad_row` 2; the rows now total 24+16+~20+~24+18 plus gaps, which fits within the 104px content height. If the build renders any row clipped, reduce `link_row`/`state_row` heights by 1-2px rather than growing the card (its 197×120 size is load-bearing for the 12-tile grid fitting the 322px panel).

- [ ] **Step 8: Rebuild, run tests, generate checkpoint screenshots**

```bash
cd stim-simulator
cmake --build --preset macos-debug -j
ctest --preset macos-debug
./out/build/macos-debug/bin/stim-simulator --screen medium --screenshot /tmp/task2-medium.bmp
./out/build/macos-debug/bin/stim-simulator --screen low --screenshot /tmp/task2-low.bmp
```
Expected: build succeeds (confirming `style_badge` and `set_card_elevation` are both genuinely gone and nothing still references them), `ctest` passes. Medium: each channel card has a solid colored header bar (navy for ready/unconfigured, teal for C通道 which is running) with white title text and, at the right, a translucent circular badge showing a check/play/minus glyph plus white state text; running/selected cards have a 2px colored border. Low: each receiver tile shows its number in navy at left, a solid colored circular badge at right (check for selected 01-04, play for running 05, grey minus otherwise), a USB glyph plus a battery glyph before the text, and a colored bullet before the bottom state text.

- [ ] **Step 9: Commit**

```bash
git add stim-simulator/src/ui/stim_ui.c
git commit -m "style: circular icon status badges and colored card headers"
```

---

### Task 3: Remaining reference icons + regenerated screenshots

**Files:**
- Modify: `stim-simulator/src/ui/stim_ui.c` — `create_prescription_row` (`:862-893`), `refresh_prescriptions` (`:546-558`), `create_receiver_panel` header block (`:1137-1148`), `create_parameter_panel` action row (`:1029-1050`)
- Regenerate: `stim-simulator/docs/screenshots/medium.png`, `stim-simulator/docs/screenshots/low.png`
- Test: none (visual-only) — verify via build + `ctest` + screenshot

**Interfaces:**
- Consumes: `make_icon_text_button` (pre-existing), the Task 1 palette, `ui.prescription_rows`
- Produces: nothing for later tasks — this is the final task

- [ ] **Step 1: Add a trailing chevron/check indicator to prescription rows**

Replace `stim-simulator/src/ui/stim_ui.c:889-892` (the tail of `create_prescription_row`):

```c
    lv_obj_t * text = make_label(row, stim_prescriptions[index], &stim_font_16, STIM_COLOR_TEXT);
    lv_obj_set_flex_grow(text, 1);
    ui.prescription_rows[screen][index] = row;
    return row;
```

with:

```c
    lv_obj_t * text = make_label(row, stim_prescriptions[index], &stim_font_16, STIM_COLOR_TEXT);
    lv_obj_set_flex_grow(text, 1);

    (void)make_label(row, LV_SYMBOL_RIGHT, &lv_font_montserrat_16, STIM_COLOR_MUTED);

    ui.prescription_rows[screen][index] = row;
    return row;
```

The row's children are now, in order: badge (index 0), text (index 1), indicator (index 2). `refresh_prescriptions` uses that index-2 position in the next step.

- [ ] **Step 2: Swap the indicator to a check mark on the selected row, in `refresh_prescriptions`**

Replace `stim-simulator/src/ui/stim_ui.c:546-558`:

```c
static void refresh_prescriptions(void)
{
    size_t screen;
    size_t index;

    for(screen = 0U; screen < 2U; ++screen) {
        for(index = 0U; index < STIM_PRESCRIPTION_COUNT; ++index) {
            bool selected = index == ui.model->selected_prescription;
            lv_obj_set_style_bg_color(ui.prescription_rows[screen][index],
                                      color(selected ? STIM_COLOR_SELECTED : STIM_COLOR_CARD), 0);
        }
    }
}
```

with:

```c
static void refresh_prescriptions(void)
{
    size_t screen;
    size_t index;

    for(screen = 0U; screen < 2U; ++screen) {
        for(index = 0U; index < STIM_PRESCRIPTION_COUNT; ++index) {
            bool selected = index == ui.model->selected_prescription;
            lv_obj_t * row = ui.prescription_rows[screen][index];
            lv_obj_t * indicator = lv_obj_get_child(row, 2);

            lv_obj_set_style_bg_color(row, color(selected ? STIM_COLOR_SELECTED : STIM_COLOR_CARD), 0);
            lv_label_set_text(indicator, selected ? LV_SYMBOL_OK : LV_SYMBOL_RIGHT);
            lv_obj_set_style_text_color(indicator,
                                        color(selected ? STIM_COLOR_BLUE : STIM_COLOR_MUTED), 0);
        }
    }
}
```

- [ ] **Step 3: Give the 治疗单元 panel header its navy bar, USB icon, and a check on 全选**

Replace `stim-simulator/src/ui/stim_ui.c:1137-1148` (inside `create_receiver_panel`):

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

with:

```c
    lv_obj_set_style_clip_corner(panel, true, 0);

    header = make_plain(panel);
    lv_obj_set_size(header, LV_PCT(100), 48);
    lv_obj_set_style_bg_color(header, color(STIM_COLOR_NAVY), 0);
    lv_obj_set_style_bg_opa(header, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_hor(header, 16, 0);
    lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(header, 10, 0);
    (void)make_label(header, LV_SYMBOL_USB, &lv_font_montserrat_16, 0xFFFFFFU);
    (void)make_label(header, "治疗单元", &stim_font_20, 0xFFFFFFU);
    lv_obj_t * summary = make_label(header, "在线 10 / 12", &stim_font_16, 0xFFFFFFU);
    lv_obj_set_flex_grow(summary, 1);
    select_all = make_icon_text_button(header, LV_SYMBOL_OK, "全选", false, NULL);
    lv_obj_set_size(select_all, 104, STIM_TOUCH_MIN);
    lv_obj_add_event_cb(select_all, select_all_event, LV_EVENT_CLICKED, NULL);
```

The concept shows a wireless-tower glyph here; per the v4 spec this becomes `LV_SYMBOL_USB` because the device has no wireless function. The 全选 button widens from 88 to 104 to fit the added check glyph.

- [ ] **Step 4: Add icons to the placement and apply buttons, in `create_parameter_panel`**

Replace `stim-simulator/src/ui/stim_ui.c:1036-1048`:

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
    lv_obj_set_flex_grow(apply, 1);
    lv_obj_add_event_cb(apply,
                        screen == STIM_SCREEN_MEDIUM ? apply_medium_event : apply_low_event,
                        LV_EVENT_CLICKED, NULL);
```

with:

```c
    if(screen == STIM_SCREEN_LOW) {
        lv_obj_t * placement = make_icon_text_button(action_row, LV_SYMBOL_GPS, "查看贴敷位置",
                                                     false, NULL);
        lv_obj_set_size(placement, 250, 48);
        lv_obj_add_event_cb(placement, placement_event, LV_EVENT_CLICKED, NULL);
    }

    apply = make_icon_text_button(action_row, LV_SYMBOL_RIGHT,
                                  screen == STIM_SCREEN_MEDIUM ? "应用到所选通道" : "配置到所选单元",
                                  true, NULL);
    lv_obj_set_height(apply, 48);
    lv_obj_set_flex_grow(apply, 1);
    lv_obj_add_event_cb(apply,
                        screen == STIM_SCREEN_MEDIUM ? apply_medium_event : apply_low_event,
                        LV_EVENT_CLICKED, NULL);
```

`make_icon_text_button` already handles white icon+text on a primary (blue) button, so the apply button needs no extra color work.

- [ ] **Step 5: Rebuild, run tests, generate checkpoint screenshots**

```bash
cd stim-simulator
cmake --build --preset macos-debug -j
ctest --preset macos-debug
./out/build/macos-debug/bin/stim-simulator --screen medium --screenshot /tmp/task3-medium.bmp
./out/build/macos-debug/bin/stim-simulator --screen low --screenshot /tmp/task3-low.bmp
```
Expected: build succeeds, `ctest` passes. Prescription rows show a grey chevron, except the selected row which shows a blue check on a light-blue background. The 治疗单元 header is a navy bar with a USB glyph, white title, white summary, and a 全选 button carrying a check glyph. The apply button shows an arrow glyph, and 查看贴敷位置 a location-pin glyph.

- [ ] **Step 6: Regenerate the committed documentation screenshots**

```bash
cd stim-simulator
./out/build/macos-debug/bin/stim-simulator --screen medium --screenshot docs/screenshots/medium.bmp
./out/build/macos-debug/bin/stim-simulator --screen low --screenshot docs/screenshots/low.bmp
```

`sips` cannot decode this BMP variant. Convert with this stdlib-only Python script (save as `/tmp/bmp2png.py` and run it), then delete the intermediate `.bmp` files:

```python
import struct, zlib

def bmp32_to_png(bmp_path, png_path):
    with open(bmp_path, 'rb') as f:
        raw = f.read()
    offset = struct.unpack('<I', raw[10:14])[0]
    width = struct.unpack('<i', raw[18:22])[0]
    height = struct.unpack('<i', raw[22:26])[0]
    redmask, greenmask, bluemask, _ = struct.unpack('<IIII', raw[54:70])
    def shift_for_mask(m):
        s = 0
        while not (m & 1):
            m >>= 1; s += 1
        return s
    rs, gs, bs = shift_for_mask(redmask), shift_for_mask(greenmask), shift_for_mask(bluemask)
    pixdata = raw[offset:offset + width * height * 4]
    rows = []
    for y in range(height):
        src_y = height - 1 - y          # BMP rows are bottom-up when height > 0
        row_start = src_y * width * 4
        row = bytearray(width * 3)
        for x in range(width):
            px = struct.unpack_from('<I', pixdata, row_start + x * 4)[0]
            row[x * 3] = (px >> rs) & 0xFF
            row[x * 3 + 1] = (px >> gs) & 0xFF
            row[x * 3 + 2] = (px >> bs) & 0xFF
        rows.append(b'\x00' + bytes(row))   # PNG filter type 0 per row
    compressed = zlib.compress(b''.join(rows), 6)
    def chunk(tag, data):
        c = tag + data
        return struct.pack('>I', len(data)) + c + struct.pack('>I', zlib.crc32(c) & 0xffffffff)
    png = b'\x89PNG\r\n\x1a\n'
    png += chunk(b'IHDR', struct.pack('>IIBBBBB', width, height, 8, 2, 0, 0, 0))
    png += chunk(b'IDAT', compressed)
    png += chunk(b'IEND', b'')
    with open(png_path, 'wb') as f:
        f.write(png)

bmp32_to_png('docs/screenshots/medium.bmp', 'docs/screenshots/medium.png')
bmp32_to_png('docs/screenshots/low.bmp', 'docs/screenshots/low.png')
```

- [ ] **Step 7: Compare both regenerated screenshots against the concept mockups**

Open the two regenerated PNGs alongside `output/ui-design/stim-ui-medium-frequency-1280x800.png` and `output/ui-design/stim-ui-low-frequency-1280x800.png`, and confirm each of these:
- top bar is solid navy with a light active-tab block and white clock/gear
- panel headings ("处方" / "处方参数" / "批量配置" / "治疗单元") are navy bars with a white icon and white text, corners clipped to the panel radius
- channel cards have colored header bars with white text and a translucent circular badge
- receiver tiles have a solid circular badge at top-right, USB + battery glyphs, and a colored bullet before the state text
- prescription rows show chevrons, with a blue check on the selected row
- overall color temperature is cool blue, not the warm/Material tones of the previous rounds

Report any element that is clipped, overlapping, or visibly different from the concept. If a receiver tile's bottom state row is cut off, that is the known tight spot flagged in Task 2 Step 7 — fix it by shaving 1-2px from `link_row`/`state_row` height, not by enlarging the tile.

- [ ] **Step 8: Commit**

```bash
git add stim-simulator/src/ui/stim_ui.c
git commit -m "style: add concept-aligned icons to prescription rows, panel headers and actions"
git add stim-simulator/docs/screenshots/medium.png stim-simulator/docs/screenshots/low.png
git commit -m "docs: regenerate simulator screenshots for the concept-alignment retheme"
```

---

## Self-Review Notes (already applied above, kept here for the record)

- **Spec coverage:** every bullet of `output/ui-design/stim-ui-design-spec.md`'s "v4 回归概念稿方案" maps to a task — token table + 标题栏 + 卡片描边 → Task 1; 状态徽章圆形图标 + 通道卡彩色标题栏 + 电池图标 → Task 2; 图标补全表的其余行 (处方行箭头/对钩、治疗单元 USB、全选、贴敷位置、应用按钮) + 截图 → Task 3. The spec's "明确不做的事" list has no task by design: no task adds the 编辑处方 button, adds a waveform icon, changes a control size, or touches `src/model/*`.
- **Placeholder scan:** no TBD/TODO; every step carries literal before/after C.
- **Symbol availability:** `LV_SYMBOL_OK / PLAY / PAUSE / MINUS / RIGHT / FILE / SETTINGS / GPS / USB / BULLET / BATTERY_FULL / BATTERY_3 / BATTERY_2 / BATTERY_1 / BATTERY_EMPTY` were each confirmed present in `_deps/lvgl-src/src/font/lv_symbol_def.h` before writing this plan. `lv_obj_set_style_clip_corner` was confirmed in `lv_obj_style_gen.h:3165`, and its children-clipping behavior in `lv_refr.c`.
- **Unused-function ordering:** `style_badge` and `set_card_elevation` each lose their final call site in Task 2, and Task 2 deletes both — Task 1 deliberately leaves them alone even though it changes surrounding code, because Task 1's checkpoint build still has live callers for both.
- **No new Chinese characters:** every Chinese string in this plan ("处方", "治疗单元", "全选", "查看贴敷位置", "应用到所选通道", "配置到所选单元", "就绪", "待配置", …) already exists in the current source, so the pre-generated font subsets cover them. No font regeneration needed.
- **Type consistency:** `make_icon_badge(parent, size)` returns the badge container whose child 0 is the icon label; `set_icon_badge(badge, icon, bg, bg_opa)` is called only with badges produced by it (`view->state_badge` in both card types). `battery_symbol(uint8_t)` matches `stim_receiver_t.battery_percent`'s declared `uint8_t` type (`src/model/stim_model.h:39`).
