# 低中频治疗仪 UI 概念设计说明

## 交付基准

- 目标画布：1280 × 800，横屏 16:10，作为 LVGL PC Simulator 的首个视觉基准。
- 页面：`中频治疗`、`低频治疗` 两个一级 Tab。
- 设计定位：医疗设备触控 HMI；高对比、低装饰、状态文字与颜色同时表达。
- 说明书真源：10 寸触摸屏；中频为 A/B/C/D 四个独立通道；低频为无线接收器批量配置；治疗时间范围 1–180 min。

## 中频治疗页

- 首屏顶部放置 A–D 四个独立通道卡片。
- 通道状态覆盖：就绪、治疗中、未配置；选中态使用描边与图标双重提示。
- 每个通道直接展示处方、剩余时间、强度和启停控制。
- 下半屏左侧为处方列表，右侧为波形及参数编辑区。
- 主操作为“应用到所选通道”，支持多通道选择后批量应用。

## 低频治疗页

- 首屏主体改为 12 个无线治疗单元的 2 × 6 网格，不沿用中频四通道卡片。
- 单元状态覆盖：已选择/已配置、治疗中、待配置、离线。
- 展示在线数量、无线连接、电池、锁定和治疗剩余时间。
- 下半屏左侧为处方列表，右侧为批量配置区。
- 主操作为“配置到所选单元”，辅操作为“查看贴敷位置”。

## LVGL 落地建议

- 根屏使用 1280 × 800 固定布局；顶部 Tab 高度约 64 px。
- 基础间距使用 8 px 网格；卡片圆角 12–16 px；主要触控热区不小于 48 × 48 px。
- 推荐组件拆分：`top_tab_bar`、`channel_card`、`receiver_tile`、`prescription_list`、`parameter_stepper`、`waveform_segment`、`primary_action_bar`。
- 状态色：主蓝 `#0B4F8A`、强调青 `#16A6C9`、运行青绿 `#16A085`、停止珊瑚红 `#E45F5F`、背景 `#F4F7FA`、正文 `#17324D`。
- 不依赖颜色单独表达状态；离线、禁用、选中、运行均同时使用文字/图标/描边。
- 后续模拟器应先实现静态页面与状态切换，再连接处方参数和治疗状态机。

## 参数声明

设计图中的处方名称、参数值、强度和运行时间仅用于展示信息层级与控件状态，不代表产品默认治疗参数。正式默认值、上下限、故障恢复策略和可编辑权限必须以经产品、硬件、法规共同评审的需求和安全边界为准。

## ImageGen 最终提示词摘要

生成模式：内置 ImageGen；分类：`ui-mockup`。

### 中频页

以说明书中频/低频界面和视频前 25 秒的设备界面为信息架构参考，生成一个可在 LVGL 实现的 1280 × 800 比例医疗设备 HMI。顶部为“中频治疗 / 低频治疗”双 Tab，中频激活；主体为 A–D 四个通道卡片，覆盖就绪、治疗中、未配置状态；下半屏为处方列表和处方参数；使用医疗蓝、青色运行态、珊瑚红停止态；平面、清晰、大触控热区，无设备外框、手、透视、品牌或水印。

### 低频页

严格沿用中频页的视觉系统，生成”低频治疗”激活页；主体改为 12 个无线治疗单元的 2 × 6 网格，覆盖已配置、治疗中、待配置、离线状态，显示无线、电池、锁定与剩余时间；下半屏为处方列表和批量配置参数，包含”查看贴敷位置”和”配置到所选单元”；保持 LVGL 可实现、平面、高可读、无设备外框、手、透视、品牌或水印。

## v2 现代化视觉方案（柔和健康风格）— 2026-08-03

### 背景

当前 `stim-simulator` 的 LVGL 实现（`src/ui/stim_ui.c`）比本文档最初的概念图（见上方两张 PNG）更平淡：整块深色卡头、直角边框、无图标、阴影很弱。本节记录对现有实现的现代化改版方案，仅调整视觉层，不改变页面结构、状态机或治疗参数。

背景说明：该 PC 模拟器的正式产品目标运行环境是嵌入式 Linux（候选 SoC：Rockchip RK3562，四核 Cortex-A53 + Mali GPU），配 10 寸电容触摸屏（说明书真源，见本文档开头）；与 `CLAUDE.md` 中记录的候选主控 MM32F0140（Cortex-M0，无 GPU）是两颗不同芯片——后者服务于低/中频功率输出等实时安全控制，前者服务于本 UI。这意味着阴影、渐变、圆角等视觉效果在目标硬件上是可行的，不必按 Cortex-M0 的资源上限自我设限。RK3562 这一具体型号目前只在对话中确认，尚未写入任何正式需求/评审文档，仍需后续在硬件框图定稿后补充真源。

### 设计令牌

| 令牌 | 值 | 说明 |
|---|---|---|
| 背景 | `#FBF8F3` | 暖白，替换现有冷灰 `#F4F7FA` |
| 卡片/面板 | 白 `#FFFFFF`，描边 `#F1E9DC` | 边框更暖、更淡 |
| 圆角 | 大面板/卡片 20px；小组件（指标块等） 12px；所有按钮胶囊形（`LV_RADIUS_CIRCLE`） | 替换现有 8–12px 直角感 |
| 阴影 | 暖色柔阴影，约 `rgba(140,120,90,.10)`，8–24px 扩散 | 用阴影表达层级，而非满版色块 header |
| 主强调色 | 蓝绿到蓝渐变 `#22C1C3 → #5B8DEF`（LVGL `bg_grad_dir = LV_GRAD_DIR_HOR`） | 仅用于激活 Tab、主操作按钮、进度条填充，避免滥用 |
| 语义色（沿用现有含义，只换色值/呈现方式） | 就绪/选中 = 蓝 `#5B8DEF`；治疗中 = 青绿 `#2BB3AE`；暂停 = 琥珀 `#B4741F`；停止 = 珊瑚红 `#F2685C` | 与当前 `STIM_COLOR_*` 常量的状态映射保持一致，只调整具体色值和视觉呈现（发光描边/徽标，而非整块色头）；琥珀色由 `#F0A857` 加深为 `#B4741F`，配合下一行的暂停底色以满足前景/背景对比度 |
| 暂停态徽标底色 | 暖淡琥珀 `#FDF0DF`（`STIM_COLOR_PAUSED_SOFT`） | 之前误借用了"选中"蓝底 `STIM_COLOR_SELECTED`（`#EAF0FE`），对比度不足且语义混淆；新增此专用令牌后暂停态不再与选中态共用背景色 |
| 文字 | 正文 `#33302B`；弱化文字 `#8C8478`；字号沿用现有 16/20/24（不新增字重/字号资源） | 层级依靠字号 + 颜色 + 间距 |

### 组件改造要点

- **Header / Tab**：整块深色按钮 Tab → 胶囊形分段控件，激活态用渐变填充。
- **通道卡片（中频 A–D）/ 治疗单元卡片（低频 12 格）**：整块深色卡头 → 白卡 + 右上角彩色徽标（胶囊）；运行中用外发光描边表达，而非整条色块头。
- **按钮体系**：开始/暂停/继续/停止统一为胶囊按钮，主操作渐变填充，次要描边；新增图标 `LV_SYMBOL_PLAY` / `PAUSE` / `STOP` / `SETTINGS` / `WIFI` / `BATTERY_FULL..EMPTY`（LVGL 内置符号，复用已启用的 `lv_font_montserrat_*`，不新增字体资源）。中文标签字体（`stim_font_16/20/24`）不含符号字形，图标需用独立 label 对象承载。
- **触控尺寸**：偏小的控件（波形按钮现 34–38px 高、部分步进按钮 32–36px）统一提到 ≥44px，匹配电容触摸屏手指直触的交互方式。
- **动效**：仅做轻量按压反馈（按下轻微变暗/缩小），不引入新动画系统或过渡时间线。
- **锁定态**：LVGL 默认符号集没有锁形图标；”已配置·锁定” 继续用文字，但改为徽标样式而非纯文字，与其它状态徽标视觉统一。

### 明确不做的事

- 不改变页面结构、信息架构、状态机或任何治疗参数默认值/上下限。
- 不新增深色模式切换、不新增字体粗细/字号、不引入复杂动画系统。
- 这套改动只影响 PC 模拟器展示层；能否/如何落地到 RK3562 正式产品 UI，仍需后续由产品/UI 单独评审确认，本次不代表定稿。

### 涉及文件

- `stim-simulator/src/ui/stim_ui.c` — 颜色常量、`make_button`/`make_panel`/`make_label` 等构造函数、各 `create_*` 组装函数。
- `output/ui-design/stim-ui-design-spec.md`（本文件）— 记录新色板/圆角/间距规则。
- `stim-simulator/README.md` 和 `docs/screenshots/*.png` — 重新生成截图。

### 验证方式

macOS 下 `cmake --build` + `ctest`（现有单元测试针对 model 层，不受视觉改动影响，应保持全绿）；运行 simulator 生成 low/medium 两张新截图，人工对照说明书信息架构（功能点/状态覆盖无增减）逐条核对。

## v3 现代化视觉方案（Material 配色 + Ant 按钮圆角）— 2026-08-04

### 背景

用户反馈 v2"柔和健康"方案配色偏淡（暖白背景、浅色渐变、极淡阴影），要求参照"大厂"UI 重新调整配色和边框处理。经浏览器三方对比（Material Design / Ant Design / Flat UI，均基于同一份中频治疗页信息渲染），确认方向：整体采用 Material 的配色/阴影/圆角体系（纯色不用渐变，卡片去边框靠阴影分层），但按钮圆角改用 Ant Design 的小圆角风格（不再是胶囊形）。本节只改视觉令牌数值和去渐变逻辑，不改变 v2 已确立的组件结构（`make_panel`/`make_button`/`apply_button_style`/`make_icon_text_button`/`style_badge` 等共享函数继续沿用，只改它们内部设置的颜色/圆角/边框/阴影数值）。

### 设计令牌（沿用 v2 的宏名，只改数值）

| 令牌 | v3 新值 | v2 旧值 | 说明 |
|---|---|---|---|
| `STIM_COLOR_BG` | `#F1F3F5` | `#FBF8F3` | 中性浅灰替换暖白 |
| `STIM_COLOR_NAVY` / `STIM_COLOR_BLUE` | `#1976D2`（Material Blue 700） | `#5B8DEF` | 纯色主色，不再渐变 |
| `STIM_COLOR_TEAL` | `#00897B`（Material Teal 600） | `#2BB3AE` | 治疗中状态色 |
| `STIM_COLOR_CORAL` | `#D32F2F`（Material Red 700） | `#F2685C` | 停止状态色，更饱和 |
| `STIM_COLOR_AMBER` | `#EF6C00`（Material Orange 800） | `#B4741F` | 暂停状态色，更饱和 |
| `STIM_COLOR_TEXT` | `#212121` | `#33302B` | 近黑，对比度更高 |
| `STIM_COLOR_MUTED` | `#757575` | `#8C8478` | |
| `STIM_COLOR_DISABLED` | `#EEEEEE` | `#F2ECE0` | 冷灰替换暖灰，用作中性禁用面/未配置徽标底色 |
| `STIM_COLOR_SELECTED` | `#E3F2FD`（Material Blue 50） | `#EAF0FE` | 选中态浅色底（卡片背景用，非徽标） |
| `STIM_COLOR_RUNNING_SOFT` | `#E0F2F1`（Material Teal 50） | `#E4F5F3` | 治疗中浅色底（卡片背景用，非徽标） |
| `STIM_COLOR_PAUSED_SOFT` | 保留 `#FDF0DF` | 不变 | 暂停浅色底（卡片背景用，非徽标）；徽标本身改实心见下 |
| `STIM_COLOR_SHADOW` | `#000000`（中性黑） | `#8C6E46`（暖棕） | 阴影改中性色，不再带暖色调 |
| `STIM_PANEL_RADIUS` | `16` | `20` | 面板圆角略收 |
| `STIM_METRIC_RADIUS` | `8` | `12` | 指标块圆角略收 |
| `STIM_BUTTON_RADIUS`（新增宏） | `8` | 原为 `LV_RADIUS_CIRCLE` | 按钮改 Ant 风格小圆角，不再是胶囊形；Tab 分段控件、+/- 步进、波形按钮、设置齿轮同步套用 |

`STIM_COLOR_GRAD_START`/`STIM_COLOR_GRAD_END` 两个渐变令牌在 v3 中不再使用（见下）。

### 去渐变

v2 中所有用 `bg_grad_color`/`bg_grad_dir` 做渐变填充的地方，v3 统一改成纯色填充（`bg_color` = `STIM_COLOR_BLUE`）：
- `apply_button_style` 的 primary 分支（影响 `make_button`/`make_icon_text_button` 所有主操作按钮）
- Tab 激活态（`set_tab_active`）
- 波形选择按钮激活态（`refresh_parameters`）
- 处方编号徽标（`create_prescription_row`）
- 参数滑块进度条（`create_parameter_row` 的 `LV_PART_INDICATOR`）

### 卡片：去边框，靠阴影分层

- `make_panel` 的 `border_width` 改为 `0`（不再画边框），阴影加重：`shadow_color` 改用中性黑 `STIM_COLOR_SHADOW`，不透明度从约 10% 提到约 15%。
- 选中/治疗中/暂停的通道卡、接收器卡，之前用"彩色描边 + 浅色底"表达强调，v3 改为"浅色底（沿用 `STIM_COLOR_SELECTED`/`RUNNING_SOFT`/`PAUSED_SOFT`）+ 阴影加重一档"，不再画彩色描边（`border_width` 统一为 0）。

### 状态徽标：实心纯色

- 就绪/选中、治疗中、暂停三种"状态类"徽标，`style_badge` 的前景/背景反转：徽标底色直接用状态色本身（如 `STIM_COLOR_BLUE`/`STIM_COLOR_TEAL`/`STIM_COLOR_AMBER`），前景固定白色 `0xFFFFFFU`。
- 未配置/离线（非状态类，中性）徽标保持 v2 逻辑不变：浅灰底（`STIM_COLOR_DISABLED`）+ 弱化灰字（`STIM_COLOR_MUTED`）。

### 按钮圆角

- 所有按钮（开始/暂停/停止、波形选择、+/-步进、应用/查看贴敷位置、全选、设置齿轮）圆角从 `LV_RADIUS_CIRCLE` 统一改为新增宏 `STIM_BUTTON_RADIUS`（8px）。
- 按钮的颜色/填充/描边逻辑不变（主操作纯色蓝底白字，次要操作白底描边），只改形状。
- Tab 分段控件（`create_header` 里的 `tabs` 轨道容器）圆角同步从 `LV_RADIUS_CIRCLE` 改为 `STIM_BUTTON_RADIUS`，与其内部按钮保持一致的直角/圆角风格。

### 明确不做的事

- 不改变页面结构、信息架构、状态机或任何治疗参数默认值/上下限（沿用 v2 承诺）。
- 不改变已确立的组件结构和触控尺寸（继续 ≥44px）、图标方案（`LV_SYMBOL_*` 复用不变）。
- 不引入新的动画/过渡效果。
- 这套改动仍只影响 PC 模拟器展示层，不代表 RK3562 正式产品 UI 定稿。

### 涉及文件

- `stim-simulator/src/ui/stim_ui.c` — 颜色/圆角宏定义、`make_panel`/`apply_button_style`/`style_badge` 等共享样式函数、`refresh_channel`/`refresh_receiver`/`refresh_parameters` 里的徽标与渐变逻辑。
- `output/ui-design/stim-ui-design-spec.md`（本文件）— 记录新色板/圆角规则。
- `stim-simulator/docs/screenshots/*.png` — 重新生成截图。

### 验证方式

macOS 下 `cmake --build` + `ctest`（不受影响，应保持全绿）；重新生成 low/medium 截图，人工核对：背景色、主色、阴影层级、按钮圆角、状态徽标实心填充是否符合本节令牌表；确认无渐变残留（`bg_grad_dir` 相关调用清零或显式设为 `LV_GRAD_DIR_NONE`）。

## v4 回归概念稿方案（深蓝标题栏 + 蓝描边卡片 + 圆形图标徽章）— 2026-08-04

### 背景

v2（柔和健康）和 v3（Material）两轮改版都在逐步远离本文档开头的两张概念设计图（`output/ui-design/stim-ui-medium-frequency-1280x800.png` / `stim-ui-low-frequency-1280x800.png`）。用户重新对比后确认：**概念稿本身的色彩、字体和布局观感更好**，要求向概念稿回归。

本节因此是一次**方向性回退 + 图标补全**，不是继续在 Material 方向上微调：
- 配色回到本项目最初的冷蓝色板（即 `stim_ui.c` 在 commit `cdbfa69` 时的原始取值），而不是 v3 的 Material 色板；
- 恢复概念稿的"实心深蓝标题栏"（v2/v3 都把它改成了白底纯文字）；
- 恢复概念稿的"卡片蓝色描边"（v3 把边框全部去掉改成纯阴影分层）；
- 状态表达从 v3 的"实心文字药丸"改为概念稿的"圆形图标徽章"；
- 按概念稿补齐图标，**唯一例外是无线相关图标**：设备已明确取消无线功能改为 USB（见本轮之前的改动），因此概念稿中的 wifi 图标一律改用 `LV_SYMBOL_USB`。

### 设计令牌（回到原始冷蓝色板）

| 令牌 | v4 值 | v3 值 | 说明 |
|---|---|---|---|
| `STIM_COLOR_BG` | `#F4F7FA` | `#F1F3F5` | 冷调浅蓝灰 |
| `STIM_COLOR_NAVY` | `#0B4F8A` | `#1976D2` | 深蓝，用于标题栏实心底色 |
| `STIM_COLOR_BLUE` | `#0A61B8` | `#1976D2` | 主蓝，用于主按钮/描边/强调（与 NAVY 重新分离为两个不同值） |
| `STIM_COLOR_TEAL` | `#159A99` | `#00897B` | 治疗中 |
| `STIM_COLOR_CORAL` | `#E45F5F` | `#D32F2F` | 停止 |
| `STIM_COLOR_AMBER` | `#C98518` | `#EF6C00` | 暂停 |
| `STIM_COLOR_TEXT` | `#17324D` | `#212121` | 冷调深蓝灰正文 |
| `STIM_COLOR_MUTED` | `#708399` | `#757575` | 冷调弱化文字 |
| `STIM_COLOR_BORDER` | `#D8E1EA` | `#BDBDBD` | 冷调中性描边（用于非强调边框） |
| `STIM_COLOR_DISABLED` | `#EEF2F6` | `#EEEEEE` | 冷调禁用面 |
| `STIM_COLOR_SELECTED` | `#EAF4FF` | `#E3F2FD` | 选中浅蓝底 |
| `STIM_COLOR_RUNNING_SOFT` | `#E8F6F5` | `#E0F2F1` | 治疗中浅青底 |
| `STIM_COLOR_PAUSED_SOFT` | `#FBF1DF` | `#FDF0DF` | 暂停浅琥珀底（调冷一档与新色板协调） |
| `STIM_PANEL_RADIUS` | `12` | `16` | 回到概念稿的较小圆角 |
| `STIM_METRIC_RADIUS` | `8` | `8` | 不变 |
| `STIM_BUTTON_RADIUS` | `8` | `8` | 不变（概念稿按钮也是小圆角，非胶囊） |

`STIM_COLOR_SHADOW` 保留 `#000000`，但阴影退居次要角色（见下）。

### 标题栏：恢复实心深蓝

- **顶部 Tab 栏**：整条 header 背景改为 `STIM_COLOR_NAVY` 实心；未激活 Tab 为透明底 + 半透明白字；激活 Tab 为白色/浅色底 + 深蓝字（概念稿中激活态是浅色块压在深蓝栏上）。时钟与设置按钮在深蓝底上用白色。
- **面板标题栏**（`make_section_header`，即"处方"/"处方参数"/"批量配置"）：恢复 `STIM_COLOR_NAVY` 实心底 + 白色文字 + 左侧图标，圆角只在顶部两角（`STIM_PANEL_RADIUS`）。
- **治疗单元面板标题栏**（`create_receiver_panel` 的 header）：同样恢复深蓝实心底 + 白字，左侧 USB 图标，右侧"全选"按钮为白底深蓝字。

### 卡片：恢复蓝色描边

- `make_panel` 恢复 `border_width = 1`、`border_color = STIM_COLOR_BORDER`；阴影保留但减弱（`LV_OPA_10`、width 12、offset_y 2），作为轻微层次而非主要分界手段。
- 通道卡 / 接收器卡的状态强调恢复为**彩色描边**（v3 改成了阴影抬升）：选中 = 2px `STIM_COLOR_BLUE`；治疗中 = 2px `STIM_COLOR_TEAL`；暂停 = 2px `STIM_COLOR_AMBER`；离线/未配置 = 1px `STIM_COLOR_BORDER`。卡片底色仍按状态使用对应的浅色底（`SELECTED`/`RUNNING_SOFT`/`PAUSED_SOFT`），与 v3 一致。
- `set_card_elevation` 不再承担状态表达职责，改由描边负责；该函数随之删除（其唯一用途消失）。

### 状态徽章：圆形图标徽章

概念稿中通道卡/接收器卡右上角是**圆形实心图标徽章**，而非文字药丸：
- 选中/就绪 → `LV_SYMBOL_OK`（对钩），底色 `STIM_COLOR_BLUE`
- 治疗中 → `LV_SYMBOL_PLAY`，底色 `STIM_COLOR_TEAL`
- 暂停 → `LV_SYMBOL_PAUSE`，底色 `STIM_COLOR_AMBER`
- 未配置/离线 → `LV_SYMBOL_MINUS`，底色 `STIM_COLOR_MUTED`

徽章为固定尺寸圆形（`LV_RADIUS_CIRCLE`），白色图标居中，字体用 `lv_font_montserrat_14`（图标必须独立 label，中文子集字体无符号字形）。

**通道卡**（中频）：概念稿标题栏内同时有"A通道"白字与徽章 + 状态文字（如"就绪"），标题栏为实心状态色。因此通道卡标题栏底色按状态取色，白色标题文字，右侧为"圆形图标 + 状态文字"组合。取色规则（穷举，无其它情况）：

| 通道状态 | 标题栏底色 | 徽章底色 | 徽章图标 |
|---|---|---|---|
| 治疗中 | `STIM_COLOR_TEAL` | 白色半透明 | `LV_SYMBOL_PLAY` |
| 已暂停 | `STIM_COLOR_AMBER` | 白色半透明 | `LV_SYMBOL_PAUSE` |
| 就绪 / 未配置（含选中态） | `STIM_COLOR_NAVY` | 白色半透明 | 就绪=`LV_SYMBOL_OK`，未配置=`LV_SYMBOL_MINUS` |

徽章位于深色标题栏之上，因此徽章底用白色半透明（`LV_OPA_30`）+ 白色图标，而不是实心状态色（实心状态色徽章压在同色标题栏上会看不见）。选中状态额外通过卡片 2px 蓝色描边表达，不改变标题栏底色。

**接收器卡**（低频）：右上角只放圆形图标徽章（无文字），状态文字保留在卡片底部一行（概念稿中底部是"● 已配置 / ● 治疗中 / ● 离线"这种小圆点 + 文字，用 `LV_SYMBOL_BULLET` 实现）。

### 图标补全（全部使用 LVGL 内置符号，不新增字体资源）

| 位置 | 图标 | 说明 |
|---|---|---|
| 治疗单元面板标题 | `LV_SYMBOL_USB` | 概念稿为 wifi 塔图标；设备已改 USB，统一用 USB |
| 接收器卡连接状态 | `LV_SYMBOL_USB` | 已在上一轮改动完成，保持 |
| 接收器卡电量 | `LV_SYMBOL_BATTERY_FULL` / `_3` / `_2` / `_1` / `_EMPTY` | 按 `battery_percent` 分档映射（≥90/≥70/≥45/≥20/其余），替换纯文字百分比前的空白 |
| 接收器卡锁定态 | `LV_SYMBOL_BULLET` | LVGL 无锁形图标；概念稿的锁图标退化为小圆点 + "已配置 锁定"文字（沿用 v2 既定处理原则） |
| 处方面板标题 | `LV_SYMBOL_FILE` | 概念稿为文档图标 |
| 处方参数/批量配置面板标题 | `LV_SYMBOL_SETTINGS` | 概念稿为滑块调节图标；内置符号中 `SETTINGS`（齿轮）最接近 |
| 处方行右侧（未选中） | `LV_SYMBOL_RIGHT` | 概念稿的右向箭头 |
| 处方行右侧（选中） | `LV_SYMBOL_OK` | 概念稿选中行显示对钩（蓝色实心圆底） |
| 查看贴敷位置按钮 | `LV_SYMBOL_GPS` | 概念稿为定位针 |
| 应用/配置主按钮 | `LV_SYMBOL_RIGHT` | 概念稿按钮文字左侧有圆形箭头 |
| 编辑处方按钮（中频页） | `LV_SYMBOL_EDIT` | 概念稿中频页处方面板底部有此按钮；**当前模拟器无此按钮，本轮不新增**（属于功能而非视觉，见下） |
| 全选按钮 | `LV_SYMBOL_OK` | 概念稿复选框图标 |
| 波形选择按钮（正弦/方/三角） | 无 | LVGL 内置符号无对应波形图标，本项目也无自定义图标管线；保持纯文字（与 v2 锁图标同一处理原则） |

### 明确不做的事

- 不改变页面结构、信息架构、状态机或任何治疗参数默认值/上下限。
- **不新增概念稿中存在但当前模拟器没有的功能性控件**（如中频页"编辑处方"按钮）——那是功能范围，不属于本轮视觉调整。
- 不新增字体资源；所有图标使用 LVGL 内置 `LV_SYMBOL_*`（经 `lv_symbol_def.h` 核对确认存在）。中文字体子集若因新增文案缺字需重新生成（本轮预计无新增中文文案）。
- 波形图标不做自定义位图。
- 触控尺寸继续 ≥44px（`STIM_TOUCH_MIN`），滑块旋钮视觉直径保持上一轮调整后的较小尺寸 + `ext_click_area` 补偿。

### 涉及文件

- `stim-simulator/src/ui/stim_ui.c` — 颜色/圆角宏、`make_panel`/`make_section_header`/`style_badge`、`create_header`、通道卡与接收器卡的构造与刷新函数、处方行、参数面板。
- `output/ui-design/stim-ui-design-spec.md`（本文件）。
- `stim-simulator/docs/screenshots/*.png` — 重新生成截图。

### 验证方式

macOS 下 `cmake --build` + `ctest`（应保持全绿）；重新生成 low/medium 截图，与本文档开头的两张概念图**并排比对**：标题栏是否为实心深蓝、卡片是否有蓝色描边、状态是否为圆形图标徽章、各处图标是否到位（wifi 位置应为 USB）、整体色温是否回到冷蓝调。
