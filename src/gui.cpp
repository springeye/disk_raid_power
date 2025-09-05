//
// Created by develop on 8/31/25.
//

#include "gui.h"
#include <cstdio> // 添加此行以支持 snprintf
static const lv_font_t *font_normal = LV_FONT_DEFAULT;
static const lv_font_t *font_large = LV_FONT_DEFAULT;
static lv_style_t style_text_muted;
static lv_style_t style_title;
static lv_style_t style_icon;
static lv_style_t style_bullet;
int32_t tab_h = 45;

static void tabview_delete_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_DELETE)
    {
        lv_style_reset(&style_text_muted);
        lv_style_reset(&style_title);
        lv_style_reset(&style_icon);
        lv_style_reset(&style_bullet);
    }
}

// 容器1
static void create_container1(lv_obj_t *parent, int x, int y, int w, int h, lv_style_t *style)
{
    lv_obj_t *container = lv_obj_create(parent);
    lv_obj_add_style(container, style, 0);
    lv_obj_set_size(container, w, h);
    lv_obj_set_pos(container, x, y);
    lv_obj_set_style_pad_all(container, 0, 0);
    lv_obj_t *label = lv_label_create(container);
    lv_label_set_text(label, "1");
}

// 容器2
static void create_container2(lv_obj_t *parent, int x, int y, int w, int h, lv_style_t *style)
{
    static lv_style_t style_icon_bg;
    lv_style_init(&style_icon_bg);
    lv_style_set_bg_color(&style_icon_bg, lv_color_hex(0x00FF00));
    lv_style_set_bg_opa(&style_icon_bg, LV_OPA_COVER);
    lv_style_set_text_color(&style_icon_bg, lv_color_hex(0x000000));
    lv_style_set_radius(&style_icon_bg, 3);
    lv_style_set_pad_all(&style_icon_bg, 2);

    static lv_style_t style_text_white;
    lv_style_init(&style_text_white);
    lv_style_set_text_color(&style_text_white, lv_color_hex(0xFFFFFF));

    lv_obj_t *container = lv_obj_create(parent);
    lv_obj_add_style(container, style, 0);
    lv_obj_set_size(container, w, h);
    lv_obj_set_pos(container, x, y);
    lv_obj_set_style_pad_all(container, 0, 0);

    // 关键：设置为Grid布局
    lv_obj_set_layout(container, LV_LAYOUT_GRID);

    // 定义列描述符：第一列25%，第二列75%
    static lv_coord_t col_dsc[] = {
        LV_PCT(25), // 第一列占25%宽度
        LV_PCT(75), // 第二列占75%宽度
        LV_GRID_TEMPLATE_LAST};

    // 定义行描述符：两行平分高度
    static lv_coord_t row_dsc[] = {
        LV_GRID_FR(1), // 第一行平分
        LV_GRID_FR(1), // 第二行平分
        LV_GRID_TEMPLATE_LAST};

    // 应用网格描述符
    lv_obj_set_grid_dsc_array(container, col_dsc, row_dsc);

    // 图标和文字内容
    const char *icons[2] = {"\xF0\x9F\x94\xA7", "\xF0\x9F\x93\xB1"};
    const char *texts[2] = {"20.1V", "5.2A"};

    lv_color_t bg_colors[2][2] = {
        {lv_color_hex(0xFF0000), lv_color_hex(0x00FF00)}, // Red and Green
        {lv_color_hex(0x0000FF), lv_color_hex(0xFFFF00)}  // Blue and Yellow
    };

    for (int row = 0; row < 2; row++)
    {
        // 图标
        lv_obj_t *icon = lv_label_create(container);
        lv_label_set_text(icon, icons[row]);
        lv_obj_add_style(icon, &style_icon_bg, 0);
        lv_obj_set_style_pad_all(icon, 0, 0);
        lv_obj_set_size(icon, LV_SIZE_CONTENT, LV_SIZE_CONTENT); // 确保内容自适应大小
        lv_obj_set_style_align(icon, LV_ALIGN_CENTER, 0); // 居中对齐
        lv_obj_set_grid_cell(icon,
                             LV_GRID_ALIGN_STRETCH, 0, 1,
                             LV_GRID_ALIGN_STRETCH, row, 1);

        // 文字
        lv_obj_t *text = lv_label_create(container);
        lv_label_set_text(text, texts[row]);
        lv_obj_add_style(text, &style_text_white, 0);
        lv_obj_set_style_text_font(text, &lv_font_montserrat_12, 0);
        lv_obj_set_style_pad_all(text, 0, 0);
        lv_obj_set_size(text, LV_SIZE_CONTENT, LV_SIZE_CONTENT); // 确保内容自适应大小
        lv_obj_set_style_align(text, LV_ALIGN_CENTER, 0); // 居中对齐
        lv_obj_set_grid_cell(text,
                             LV_GRID_ALIGN_STRETCH, 1, 1,
                             LV_GRID_ALIGN_CENTER, row, 1);
    }
}

// 容器3
static void create_container3(lv_obj_t *parent, int x, int y, int w, int h, lv_style_t *style)
{
    static lv_style_t style_icon_bg;
    lv_style_init(&style_icon_bg);
    lv_style_set_bg_color(&style_icon_bg, lv_color_hex(0x00FF00));
    lv_style_set_bg_opa(&style_icon_bg, LV_OPA_COVER);
    lv_style_set_text_color(&style_icon_bg, lv_color_hex(0x000000));
    lv_style_set_radius(&style_icon_bg, 3);
    lv_style_set_pad_all(&style_icon_bg, 2);

    static lv_style_t style_text_white;
    lv_style_init(&style_text_white);
    lv_style_set_text_color(&style_text_white, lv_color_hex(0xFFFFFF));

    lv_obj_t *container = lv_obj_create(parent);
    lv_obj_add_style(container, style, 0);
    lv_obj_set_size(container, w, h);
    lv_obj_set_pos(container, x, y);
    lv_obj_set_style_pad_all(container, 0, 0);

    lv_obj_t *row1 = lv_obj_create(container);
    lv_obj_set_size(row1, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(row1, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(row1, 0, 0);
    lv_obj_set_width(row1, LV_PCT(100));
    lv_obj_align(row1, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t *icon1 = lv_label_create(row1);
    lv_label_set_text(icon1, "\xF0\x9F\x94\xA7");
    lv_obj_add_style(icon1, &style_icon_bg, 0);

    lv_obj_t *text1 = lv_label_create(row1);
    lv_label_set_text(text1, "Line 1");
    lv_obj_add_style(text1, &style_text_white, 0);
    lv_obj_set_style_text_font(text1, &lv_font_montserrat_12, 0);
    lv_obj_set_flex_grow(text1, 1);

    lv_obj_t *row2 = lv_obj_create(container);
    lv_obj_set_size(row2, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(row2, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(row2, 0, 0);
    lv_obj_set_width(row2, LV_PCT(100));
    lv_obj_align(row2, LV_ALIGN_BOTTOM_MID, 0, 0);

    lv_obj_t *icon2 = lv_label_create(row2);
    lv_label_set_text(icon2, "\xF0\x9F\x93\xB1");
    lv_obj_add_style(icon2, &style_icon_bg, 0);

    lv_obj_t *text2 = lv_label_create(row2);
    lv_label_set_text(text2, "Line 2");
    lv_obj_add_style(text2, &style_text_white, 0);
    lv_obj_set_style_text_font(text2, &lv_font_montserrat_12, 0);
    lv_obj_set_flex_grow(text2, 1);
}

// 容器4
static void create_container4(lv_obj_t *parent, int x, int y, int w, int h, lv_style_t *style)
{
    lv_obj_t *container = lv_obj_create(parent);
    lv_obj_add_style(container, style, 0);
    lv_obj_set_size(container, w, h);
    lv_obj_set_pos(container, x, y);
    lv_obj_set_style_pad_all(container, 0, 0);
    lv_obj_t *label = lv_label_create(container);
    lv_label_set_text(label, "4");
}

// 容器5
static void create_container5(lv_obj_t *parent, int x, int y, int w, int h, lv_style_t *style)
{
    lv_obj_t *container = lv_obj_create(parent);
    lv_obj_add_style(container, style, 0);
    lv_obj_set_size(container, w, h);
    lv_obj_set_pos(container, x, y);
    lv_obj_set_style_pad_all(container, 0, 0);
    lv_obj_t *label = lv_label_create(container);
    lv_label_set_text(label, "5");
}

// 容器6
static void create_container6(lv_obj_t *parent, int x, int y, int w, int h, lv_style_t *style, int scr_w, int scr_h)
{
    lv_obj_t *container = lv_obj_create(parent);
    lv_obj_add_style(container, style, 0);
    lv_obj_set_size(container, w, h);
    lv_obj_set_pos(container, x, y);
    lv_obj_set_style_pad_all(container, 0, 0);
    lv_obj_t *label = lv_label_create(container);
    char wh_buf[32];
    snprintf(wh_buf, sizeof(wh_buf), "width: %d\nheight: %d", scr_w, scr_h);
    lv_label_set_text(label, wh_buf);
}

void show_gui()
{
#if LV_USE_THEME_DEFAULT
    lv_theme_default_init(NULL, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK,
                          font_normal);
#endif

    lv_style_init(&style_text_muted);
    lv_style_set_text_opa(&style_text_muted, LV_OPA_50);

    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, font_large);

    lv_style_init(&style_icon);
    lv_style_set_text_color(&style_icon, lv_theme_get_color_primary(NULL));
    lv_style_set_text_font(&style_icon, font_large);

    lv_style_init(&style_bullet);
    lv_style_set_border_width(&style_bullet, 0);
    lv_style_set_radius(&style_bullet, LV_RADIUS_CIRCLE);

    lv_obj_t *scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x000545), 0);
    lv_obj_set_style_radius(scr, 10, 0);
    lv_obj_set_style_border_width(scr, 1, 0);

    // Container style
    static lv_style_t style_container;
    lv_style_init(&style_container);
    lv_style_set_bg_opa(&style_container, LV_OPA_20);
    lv_style_set_border_width(&style_container, 1);
    lv_style_set_border_color(&style_container, lv_color_hex(0x3D71AB));
    lv_style_set_radius(&style_container, 5);

    int spacing = 5;
    int scr_w = lv_obj_get_width(scr);
    int scr_h = lv_obj_get_height(scr);

    // 计算每一行的高度
    int total_spacing = spacing * 4;                      // 3行间距 + 顶部 + 底部
    int row1_h = (scr_h - total_spacing) / 3;             // 第一行高度
    int row2_h = row1_h;                                  // 第二行高度
    int row3_h = scr_h - total_spacing - row1_h - row2_h; // 第三行高度（剩余）

    // 第一行：左（container1），右（container2+container3）
    int col_w = (scr_w - 3 * spacing) / 2;
    int c1_w = col_w;
    int c1_h = row1_h;
    int c2_w = col_w;
    int c2_h = row1_h / 2 - spacing / 2;
    int c3_w = col_w;
    int c3_h = row1_h - c2_h - spacing;

    // 第二行：左（container4），右（container5）
    int c4_w = col_w;
    int c4_h = row2_h;
    int c5_w = col_w;
    int c5_h = row2_h;

    // 第三行：container6
    int c6_w = scr_w - 2 * spacing;
    int c6_h = row3_h;

    // 创建各容器
    create_container1(scr, spacing, spacing, c1_w, c1_h, &style_container);
    create_container2(scr, c1_w + 2 * spacing, spacing, c2_w, c2_h, &style_container);
    create_container3(scr, c1_w + 2 * spacing, spacing + c2_h + spacing, c3_w, c3_h, &style_container);
    create_container4(scr, spacing, spacing + c1_h + spacing, c4_w, c4_h, &style_container);
    create_container5(scr, c4_w + 2 * spacing, spacing + c1_h + spacing, c5_w, c5_h, &style_container);
    create_container6(scr, spacing, spacing + c1_h + spacing + c4_h + spacing, c6_w, c6_h, &style_container, scr_w, scr_h);
    create_container6(scr, spacing, spacing + c1_h + spacing + c4_h + spacing, c6_w, c6_h, &style_container, scr_w, scr_h);
    create_container6(scr, spacing, spacing + c1_h + spacing + c4_h + spacing, c6_w, c6_h, &style_container, scr_w, scr_h);
}
