#ifndef __CR2_CR2_HPP__
#define __CR2_CR2_HPP__
#include <exception>
#include <map>

#include <cr2.h>
void __throw_bad_exception(void);
#if defined(__cpp_exceptions) && __cpp_exceptions
void __throw_bad_exception(void) { throw std::bad_exception(); }
#else
void __throw_bad_exception(void) { abort(); }
#endif

void* operator new(std::size_t n) {
  void* const p = std::malloc(n);
  // handle p == 0
  return p;
}

void operator delete(void* p) noexcept { std::free(p); }

class CR2 {
 public:
  CR2() { cr2_init(); }
  ~CR2() {
    for (auto& s : stacks) {
      free(s.second.stack);
    }
  }
  void add_thread(unsigned int priority, cr2_thread_handler_t th,
                  size_t stack_size) {
    cr2_thread_t t;
    t.stack_size = stack_size;
    t.stack = (cr2_stack_type_t*)malloc(stack_size * sizeof(cr2_stack_type_t));
    stacks[priority] = t;
    cr2_thread_init(&stacks[priority], priority, th);
  }
  void start() { cr2_start(); }

 private:
  std::map<unsigned int, cr2_thread_t> stacks;
};
#endif
#endif