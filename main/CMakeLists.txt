idf_component_register(SRCS "main.c"
                    SRCS "BTDevice.c"
                    SRCS "UartDevice.c"
                    INCLUDE_DIRS ".")
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
