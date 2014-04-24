CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

SET(PROJECT_NAME nfc-rfid-bricklet)
SET(CHIP sam3s4)
SET(BOARD sam3s-ek)
SET(MCU cortex-m3\ -mthumb)
PROJECT(${PROJECT_NAME})

# Optimization level, can be [0, 1, 2, 3, s]. 
# 0 = turn off optimization. s = optimize for size.
SET(OPTIMIZATION_LEVEL 3)
SET(DEBUG "")

SET(CMAKE_BUILD_TYPE None)

INCLUDE_DIRECTORIES(
	"${PROJECT_SOURCE_DIR}/src/"
)

# find source files
FILE(GLOB SOURCES "src/everything.c")

MESSAGE(STATUS "\nFound following source files:\n ${SOURCES}\n")

# define executable
ADD_EXECUTABLE(${PROJECT_NAME}.elf ${SOURCES})

SET_TARGET_PROPERTIES(${PROJECT_NAME}.elf PROPERTIES LINKER_LANGUAGE C)

# define compile flags 
SET_TARGET_PROPERTIES(${PROJECT_NAME}.elf PROPERTIES COMPILE_FLAGS
	"${DEBUG} -mcpu=${MCU} -std=gnu99  -Wall -mlong-calls -ffunction-sections -O${OPTIMIZATION_LEVEL} -fPIE -pie"
)

#define linker flags 
SET_TARGET_PROPERTIES(${PROJECT_NAME}.elf PROPERTIES LINK_FLAGS
	"-mcpu=${MCU} -nostartfiles  -Wl,--gc-sections -T\"${PROJECT_SOURCE_DIR}/src/brickletlib/bricklet.lds\" -fPIE -pie"
)


ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}.elf POST_BUILD COMMAND 
                   ${CMAKE_OBJCOPY} -S -O binary 
                   ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin)

ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME}.elf POST_BUILD COMMAND 
                   ${CMAKE_SIZE} 
                   ${OBJECT} ${PROJECT_NAME}.elf)

# add preprocessor defines
ADD_DEFINITIONS(-D${CHIP} -Dflash -DNOASSERT -DNDEBUG ) 
