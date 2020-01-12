#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <uart.h>

int _kill(pid_t pid, int sig);
int _getpid(void);
int _close(int fd);
int _open(int fd);
int _fstat(int fd);
int _isatty(int fd);
int _lseek(int fd, off_t offset, int whence);
ssize_t _read(int fd, void *buf, size_t count);
ssize_t _write(int fd, const void *buf, size_t count);

int _open(int fd) { return 0; }
int _kill(pid_t pid, int sig) { return 0; }
int _getpid(void) { return 0; }
int _close(int fd) { return -1; }
int _fstat(int fd) { return -1; }
int _isatty(int fd) {
  return fd == (int)UART0 || fd == (int)UART1 || fd == stdout->_file;
}
int _lseek(int fd, off_t offset, int whence) { return -1; }

ssize_t _read(int fd, void *buf, size_t count) {
  char c;
  ssize_t i = 0;
  if (fd == stdin->_file) {
    uart_gets(UART0, (char *)buf, count);
    i = (ssize_t)strlen((char *)buf);
  }
  return i;
}

ssize_t _write(int fd, const void *buf, size_t count) {
  ssize_t i = 0;
  if (fd == stdout->_file) {
    for (; i < (ssize_t)count; i++) {
      uart_putc(UART0, ((const char *)(buf))[i]);
    }
  }
  return i;
}

int getc(FILE *f) {
  if (f->_file == stdin->_file) {
    return uart_getc(UART0);
  }
  return EOF;
}

int getchar(void) { return uart_getchar(UART0); }

int putchar(int c) {
  uart_putc(UART0, (char)c);
  return 0;
}