TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    Common.c \
    liblog.c \
    libthread.c \
    MY_UDP.c \
    Sudp.c \
    tcp_method.c \
    thread_process.c \
    threadpool.c \
    file_helper.c

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    inc/Common.h \
    inc/defs.h \
    inc/liblog.h \
    inc/libthread.h \
    inc/my_all.h \
    inc/MY_UDP.h \
    inc/mytypes.h \
    inc/Protocol.h \
    inc/tcp_method.h \
    inc/thread_process.h \
    Protocol.h \
    threadpool.h \
    file_helper.h \
    lock.h

