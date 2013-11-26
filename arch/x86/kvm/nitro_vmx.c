

#include "nitro_vmx.h"

#include "x86.h"

int nitro_handle_gp(struct kvm_vcpu *vcpu){
  printk(KERN_INFO "nitro: #GP trapped.\n");
  kvm_clear_exception_queue(vcpu);
  kvm_clear_interrupt_queue(vcpu);
  kvm_queue_exception_e(vcpu,GP_VECTOR,vcpu->run->ex.error_code);
  return 0;
}