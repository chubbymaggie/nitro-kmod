#ifndef NITRO_VMX_H_
#define NITRO_VMX_H_

#include <linux/kvm_host.h>

int nitro_handle_gp(struct kvm_vcpu*);

#endif //NITRO_VMX_H_