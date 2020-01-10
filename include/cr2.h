#ifndef __CR2_CR2_H__
#define __CR2_CR2_H__
#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#define CR2_MAX_THREADS 32
#define CR2_TICK_RATE_HZ 1000

#define cr2_yield() __asm__ volatile("ecall")

typedef uint32_t cr2_stack_type_t;

// Thread Control Block
typedef struct {
  void* stack_ptr __attribute__((aligned(4)));
  uint32_t timeout __attribute__((aligned(4)));
  cr2_stack_type_t* stack;
  size_t stack_size;
  unsigned int priority;
} cr2_thread_t;

typedef void (*cr2_thread_handler_t)();

void cr2_init(void);
void cr2_start(void);
void cr2_schedule(void);
void cr2_thread_init(cr2_thread_t* t, unsigned int priority,
                     cr2_thread_handler_t th);
void cr2_enter_critical_section(void);
void cr2_exit_critical_section(void);
void cr2_delay(uint32_t timeout);

#ifdef __cplusplus
}
void __throw_bad_exception(void);
#include <exception>
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

#include <map>

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