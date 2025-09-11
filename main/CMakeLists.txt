SET(SOURCES ui_schome.c
    ui_scota.c
    ui.c
    ui_comp_hook.c
    ui_helpers.c
    fonts/ui_font_milight50.c
    fonts/ui_font_mibold50.c
    fonts/ui_font_mibol20.c
    fonts/ui_font_mibol40.c
    fonts/ui_font_milight15.c
    fonts/ui_font_mibol16.c)

add_library(ui ${SOURCES})
