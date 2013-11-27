#include "nitro_x86.h"

#include <linux/nitro_main.h>



int nitro_set_syscall_trap(struct kvm *kvm){
  printk(KERN_INFO "nitro: set syscall trap\n");
  //kvm->nitro_kvm.trap_syscalls = 1;
  
  
  
  return 0;
}

int nitro_unset_syscall_trap(struct kvm *kvm){
  printk(KERN_INFO "nitro: unset syscall trap\n");
  //kvm->nitro_kvm.trap_syscalls = 0;
  return 0;
}

