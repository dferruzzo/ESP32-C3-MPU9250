idf_component_register(SRCS "hmc5883l.c"
                       INCLUDE_DIRS "."
                       REQUIRES driver my_i2c hmc5883l)