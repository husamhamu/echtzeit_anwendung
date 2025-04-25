set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

#set(CROSS_TOOLCHAIN_PATH "$ENV{MCU_TOOLCHAIN_DIR}")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

if(NOT DEFINED CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER arm-none-eabi-gcc)
else()
    message("Using C Compiler from Command-Line")
endif()
if(NOT DEFINED CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
else()
    message("Using CXX Compiler from Command-Line")
endif()
if(NOT DEFINED CMAKE_ASM_COMPILER)
    set(CMAKE_CXX_COMPILER arm-none-eabi-gcc)
else()
    message("Using ASM Compiler from Command-Line")
endif()
#set(CMAKE_OBJCOPY arm-none-eabi-objcopy) # These should be inferred from the compiler
#set(CMAKE_OBJDUMP arm-none-eabi-objdump)

option(HARD_FLOAT "allows to use the FPU inside the processor" ON)
if(HARD_FLOAT)
    set(FLOAT_FLAGS -mfloat-abi=hard)
else()
    set(FLOAT_FLAGS -mfloat-abi=soft)
endif()

set(CORE_FLAGS "-mthumb -mcpu=cortex-m4 -mlittle-endian ${FLOAT_FLAGS} -mfpu=fpv4-sp-d16 --specs=nano.specs -nostartfiles -D${DEVICE_FAMILY} -DHSE_VALUE=${HSE_VALUE} -DUSE_HAL_DRIVER ${ADDITIONAL_CORE_FLAGS}")

set(COMMON_FLAGS "-fno-builtin -fdata-sections -ffunction-sections")

# compiler: language specific flags
set(CMAKE_C_FLAGS "${CORE_FLAGS} ${COMMON_FLAGS}" CACHE INTERNAL "c compiler flags")
set(CMAKE_C_FLAGS_DEBUG "-DDEBUG" CACHE INTERNAL "c compiler flags: Debug")
set(CMAKE_C_FLAGS_RELEASE "" CACHE INTERNAL "c compiler flags: Release")

set(CMAKE_CXX_FLAGS "${CORE_FLAGS} ${COMMON_FLAGS} -fno-rtti -fno-exceptions" CACHE INTERNAL "cxx compiler flags")
set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG" CACHE INTERNAL "cxx compiler flags: Debug")
set(CMAKE_CXX_FLAGS_RELEASE "" CACHE INTERNAL "cxx compiler flags: Release")

set(CMAKE_ASM_FLAGS "${CORE_FLAGS} -g -ggdb3 -D__USES_CXX" CACHE INTERNAL "asm compiler flags")
set(CMAKE_ASM_FLAGS_DEBUG "" CACHE INTERNAL "asm compiler flags: Debug")
set(CMAKE_ASM_FLAGS_RELEASE "" CACHE INTERNAL "asm compiler flags: Release")
