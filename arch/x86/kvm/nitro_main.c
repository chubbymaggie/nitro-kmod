#include <linux/spinlock.h>
#include <linux/list.h>
#include <linux/pid.h>
#include <linux/slab.h>
#include <linux/stddef.h>
#include <linux/compiler.h>
#include <asm/current.h>
#include <asm-generic/errno-base.h>

#include <linux/kvm_host.h>

#include <linux/nitro_main.h>

extern int create_vcpu_fd(struct kvm_vcpu*);

struct kvm* nitro_get_vm_by_creator(pid_t creator){
  struct kvm *rv;
  struct kvm *kvm;
  
  rv = NULL;
  
  raw_spin_lock(&kvm_lock);
  list_for_each_entry(kvm,&vm_list,vm_list)
    if(kvm->mm->owner->pid == creator){
      rv = kvm;
      break;
    }
  raw_spin_unlock(&kvm_lock);
  
  return rv;
}

void nitro_create_vm_hook(struct kvm *kvm){
  pid_t pid;
  struct nitro_kvm *nitro_kvm;
  
  //get current pid
  pid = pid_nr(get_task_pid(current, PIDTYPE_PID));
  printk(KERN_INFO "nitro: new VM created, creating process: %d\n", pid);
  
  kvm->nitro_kvm = NULL;
  
  //allocate memory
  nitro_kvm = (struct nitro_kvm*) kzalloc(sizeof(struct nitro_kvm),GFP_KERNEL | __GFP_REPEAT);
  if(nitro_kvm == NULL){
    printk(KERN_WARNING "nitro: unable to alocate memory for nitro_kvm. the VM(%d) will start, but nitro cannot be attached. Please restart the VM for nitro functionality.\n", pid);
    return;
  }
  
  //init nitro_kvm
  nitro_kvm->placeholder = 0;
  
  //add nitro_kvm to kvm
  kvm->nitro_kvm = nitro_kvm;
}

void nitro_destroy_vm_hook(struct kvm *kvm){
  if(kvm->nitro_kvm != NULL)
    kfree(kvm->nitro_kvm);
  kvm->nitro_kvm = NULL;
}

int nitro_iotcl_num_vms(void){
  struct kvm *kvm;
  int rv = 0;
  
  raw_spin_lock(&kvm_lock);
  list_for_each_entry(kvm, &vm_list, vm_list)
    rv++;
  raw_spin_unlock(&kvm_lock);
  
  return rv;
}

int nitro_iotcl_attach_vcpus(struct kvm *kvm, struct nitro_vcpus *nvcpus){
  int r,i;
  struct kvm_vcpu *v;
  
  mutex_lock(&kvm->lock);
  
  nvcpus->num_vcpus = atomic_read(&kvm->online_vcpus);
  if(unlikely(nvcpus->num_vcpus > NITRO_MAX_VCPUS)){
    goto error_out;
  }
  
  kvm_for_each_vcpu(r, v, kvm){
    nvcpus->ids[r] = v->vcpu_id;
    kvm_get_kvm(kvm);
    nvcpus->fds[r] = create_vcpu_fd(v);
    if(nvcpus->fds[r]<0){
      for(i=r;r>=0;i--){
	nvcpus->ids[r] = 0;
	nvcpus->fds[i] = 0;
	kvm_put_kvm(kvm);
      }
      goto error_out;
    }
  }
  
  mutex_unlock(&kvm->lock);
  return 0;
  
  error_out:
  mutex_unlock(&kvm->lock);
  return -1;
}



