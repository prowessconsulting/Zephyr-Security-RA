ifdef CONFIG_CC3200SDK_LIBRARY
ZEPHYRINCLUDE += -I$(CONFIG_CC3200SDK_INSTALL_PATH)
LIB_INCLUDE_DIR += -L$(CONFIG_CC3200SDK_INSTALL_PATH:"%"=%)/driverlib/gcc/exe
ALL_LIBS += driver
endif  # CONFIG_CC3200SDK_LIBRARY

ifdef CONFIG_CC3200SDK_BUILTIN
ZEPHYRINCLUDE +=-I$(srctree)/ext/hal/ti/cc3200sdk
ZEPHYRINCLUDE +=-I$(srctree)/ext/hal/ti/cc3200sdk/inc
ZEPHYRINCLUDE +=-I$(srctree)/ext/hal/ti/cc3200sdk/driverlib
endif # CONFIG_CC3200_BUILTIN
