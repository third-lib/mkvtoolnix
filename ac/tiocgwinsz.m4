AC_SYS_POSIX_TERMIOS
AC_HEADER_TIOCGWINSZ
AC_CHECK_HEADERS([sys/ioctl.h])

if test x"$ac_cv_sys_posix_termios" = "xyes" ; then
  AC_DEFINE([HAVE_TIOCGWINSZ], 1, [define if ioctl & TIOCGWINSZ are available])
fi
