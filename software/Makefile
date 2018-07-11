SHELL            := /bin/bash
ROOT_DIR         := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
BRICKLIB_PATH    := $(realpath $(ROOT_DIR)/src/bricklib)
BRICKLETLIB_PATH := $(realpath $(ROOT_DIR)/src/brickletlib)

include $(BRICKLIB_PATH)/toolchains/Makefile_Bricklet.mk
