




















#if !defined(set_binary_mode)
bool set_binary_mode (int, bool);
#if ! HAVE_SETMODE_DOS
#define set_binary_mode(fd, mode) true
#endif
#endif
