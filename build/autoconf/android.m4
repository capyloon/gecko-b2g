dnl This Source Code Form is subject to the terms of the Mozilla Public
dnl License, v. 2.0. If a copy of the MPL was not distributed with this
dnl file, You can obtain one at http://mozilla.org/MPL/2.0/.

AC_DEFUN([MOZ_ANDROID_NDK],
[

case "$target" in
*-android*|*-linuxandroid*)
    dnl $android_* will be set for us by Python configure.
    directory_include_args="-isystem $android_system -isystem $android_sysroot/usr/include"

    # clang will do any number of interesting things with host tools unless we tell
    # it to use the NDK tools.
    extra_opts=""
    CPPFLAGS="$extra_opts $CPPFLAGS"
    ASFLAGS="$extra_opts $ASFLAGS"
    LDFLAGS="$extra_opts $LDFLAGS"

    CPPFLAGS="$directory_include_args $CPPFLAGS"
    CFLAGS="-fno-short-enums -fno-exceptions $CFLAGS"
    CXXFLAGS="-fno-short-enums -fno-exceptions $CXXFLAGS $stlport_cppflags"
    ASFLAGS="$directory_include_args -DANDROID $ASFLAGS"

    # If we're building for a gonk target add more sysroot system includes and
    # library paths
    if test -n "$gonkdir"; then
        CPPFLAGS="$CPPFLAGS -isystem $gonkdir/include"
        # HACK: Some system headers in the AOSP sources are laid out
        # differently than the others so they get included both as
        # #include <camera/path/to/header.h> and directly as
        # #include <path/to/header.h>. To accomodate for this we need camera/
        # in the default include path until we can fix the issue.
        CPPFLAGS="$CPPFLAGS -isystem $gonkdir/include/camera"

        # Needed for config/system-headers.mozbuild
        ANDROID_VERSION="${android_version}"
        AC_SUBST(ANDROID_VERSION)
    fi

    ;;
esac

])

AC_DEFUN([MOZ_ANDROID_STLPORT],
[

if test "$OS_TARGET" = "Android"; then
    if test -z "$STLPORT_LIBS"; then
        # android-ndk-r25b
        # Example: $android_ndk/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/lib/aarch64-linux-android/
        # TODO: don't hardcode host and target
        cxx_libs="$android_ndk/toolchains/llvm/prebuilt/linux-x86_64/sysroot/usr/lib/aarch64-linux-android"
        # NDK r12 removed the arm/thumb library split and just made
        # everything thumb by default.  Attempt to compensate.
        if test "$MOZ_THUMB2" = 1 -a -d "$cxx_libs/thumb"; then
            cxx_libs="$cxx_libs/thumb"
        fi

        if ! test -e "$cxx_libs/libc++_static.a"; then
            AC_MSG_ERROR(["Couldn't find path to llvm-libc++ in the android ndk at $cxx_libs"])
        fi

        # See https://developer.android.com/ndk/guides/cpp-support#use_clang_directly
        STLPORT_LIBS="-static-libstdc++ -lc++_static"
        # NDK r12 split the libc++ runtime libraries into pieces.
        for lib in c++abi unwind android_support; do
            if test -e "$cxx_libs/lib${lib}.a"; then
                 STLPORT_LIBS="$STLPORT_LIBS -l${lib}"
            fi
        done
    fi
fi
AC_SUBST_LIST([STLPORT_LIBS])

])
