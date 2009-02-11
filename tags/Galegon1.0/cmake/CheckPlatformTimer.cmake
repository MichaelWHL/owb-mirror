if(USE_TIMER STREQUAL "GLIB")
    pkg_check_modules(GLIB REQUIRED glib-2.0>=2.0)
    set(TIMER_INCLUDE_DIRS ${GLIB_INCLUDE_DIRS})
    set(TIMER_LIBRARIES ${GLIB_LIBRARIES})

    set(USE_TIMER_GLIB TRUE)
    mark_as_advanced(USE_TIMER_GLIB)
endif(USE_TIMER STREQUAL "GLIB")

if(USE_TIMER STREQUAL "LINUX")
    set(USE_TIMER_LINUX TRUE)
    mark_as_advanced(USE_TIMER_LINUX)
endif(USE_TIMER STREQUAL "LINUX")

if(USE_TIMER STREQUAL "QT")
    pkg_check_modules(QTTIMER REQUIRED QtCore)
    set(TIMER_INCLUDE_DIRS ${QTTIMER_INCLUDE_DIRS})
    set(TIMER_LIBRARIES ${QTTIMER_LIBRARIES})

    set(USE_TIMER_QT TRUE)
    mark_as_advanced(USE_TIMER_QT)
endif(USE_TIMER STREQUAL "QT")
