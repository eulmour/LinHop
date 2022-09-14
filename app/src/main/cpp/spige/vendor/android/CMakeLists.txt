# set(${CMAKE_C_FLAGS}, "${CMAKE_C_FLAGS}")

# build native_app_glue as a static lib
#add_library(native_app_glue STATIC
#    ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

add_library(native_app_glue STATIC android_native_app_glue.c)
