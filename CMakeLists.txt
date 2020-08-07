if(DEFINED ENV{JSTEST_ENABLE})
    execute_process(COMMAND cmd /c "test_sffs.bat ${out_hex_dir}" ${CMAKE_SOURCE_DIR}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/demo/js/
        OUTPUT_VARIABLE SFFS_VAR
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
else()
    execute_process(COMMAND cmd /c "sffs.bat ${out_hex_dir}" ${CMAKE_SOURCE_DIR}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/demo/js/
        OUTPUT_VARIABLE SFFS_VAR
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif()
message("${SFFS_VAR}")

include_directories(
    .
    duktape
    duktape_task
    lib
    duktape/module-node
    platform
    platform/luat
)
set(APP_SRCS
    app.c 
    fsdep.c     
    
    # debugger.c
    # debugger_uart.c
)
set(DUKTAPE_SRCS
    duktape/utils.c
    duktape/init.c
    duktape/duktape.c
    duktape/eval_file.c
    duktape/module-node/duk_module_node.c
    duktape/module-node/mod_load.c
)

set(DUKTAPE_TASK_SRCS
    duktape_task/duktape_task.c
    duktape_task/on_msg.c
)
set(JS_LIB_SRCS
    lib/os.c
    lib/iot_module.c
    lib/tts.c
    lib/audio.c
    lib/network.c
    lib/udp.c
    lib/misc.c
    lib/fs.c
    lib/uart.c
    lib/g711a.c
    lib/gpio.c
    lib/keypad.c
    lib/pmd.c
)
set(PLATFORM_SRCS
    platform/luat/os.c
    platform/luat/tts.c
    platform/luat/audio.c
    platform/luat/network.c
    platform/luat/udp.c
    platform/luat/misc.c
    platform/luat/fs.c
    platform/luat/log.c
    platform/luat/uart.c
    platform/luat/gpio.c
    platform/luat/keypad.c
    platform/luat/pmd.c
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -w -DDUK_USE_DATE_GET_NOW=get_now -DMAX_GPIO=32")
set(CMAKE_C_LDFLAGS,"${CMAKE_C_LDFLAGS} -printf")

if(DEFINED ENV{LOG_TO_UART1})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLOG_TO_UART1")
elseif(DEFINED ENV{LOG_TO_UART2})
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLOG_TO_UART2")
endif()

if(CONFIG_APPIMG_LOAD_FLASH)
add_appimg(${target} ${flash_ldscript} ${APP_SRCS} ${JS_LIB_SRCS} ${DUKTAPE_SRCS} ${PLATFORM_SRCS} ${DUKTAPE_TASK_SRCS})
else(CONFIG_APPIMG_LOAD_FLASH)
add_appimg(${target} ${file_ldscript} ${APP_SRCS} ${JS_LIB_SRCS} ${DUKTAPE_SRCS} ${PLATFORM_SRCS} ${DUKTAPE_TASK_SRCS})
endif()