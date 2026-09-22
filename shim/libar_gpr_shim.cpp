#include <stdint.h>
#include <atomic>

extern "C" {
std::atomic<int32_t> gpr_receiver_tid{-1};
void ar_osal_panic_ignore() {
}
void gsl_set_skip_readtfnosound() {
}
}
