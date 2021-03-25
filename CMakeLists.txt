idf_component_register(SRCS "errors.cpp" "logger.cpp" "mqtt.cpp" "wifi.cpp"
                       INCLUDE_DIRS "include"
                       REQUIRES mqtt fmt)