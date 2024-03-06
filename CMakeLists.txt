if (NOT TARGET pico_st7735_80x160)
    add_library(pico_st7735_80x160 INTERFACE)

    target_sources(pico_st7735_80x160 INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/lcd.c
        ${CMAKE_CURRENT_LIST_DIR}/lcd_extra.c
    )

    target_link_libraries(pico_st7735_80x160 INTERFACE
        hardware_clocks
        hardware_spi
        pico_stdlib
    )
    target_include_directories(pico_st7735_80x160 INTERFACE ${CMAKE_CURRENT_LIST_DIR})
endif()
