#include <kernel/types.h>
#include <kernel/string.h>
#include <kernel/printf.h>
#include <kernel/vfs.h>
#include <kernel/version.h>
#include <kernel/process.h>
#include <kernel/signal.h>
#include <kernel/misc.h>
#include <kernel/time.h>
#include <kernel/ptrace.h>
#include <kernel/hashmap.h>
#include <kernel/module.h>
#include <kernel/ksym.h>

#include <sys/time.h>
#include <sys/utsname.h>
#include <sys/ptrace.h>

#include <kernel/arch/x86_64/mmu.h>
#include <kernel/arch/x86_64/ports.h>
#include <kernel/arch/x86_64/pml.h>
#include <kernel/arch/x86_64/regs.h>
#include <kernel/arch/x86_64/irq.h>

static struct idt_pointer idtp;
static idt_entry_t idt[256];

void idt_set_gate(uint8_t num, interrupt_handler_t handler, uint16_t selector, uint8_t flags, int userspace) {
	uintptr_t base = (uintptr_t)handler;
	idt[num].base_low  = (base & 0xFFFF);
	idt[num].base_mid  = (base >> 16) & 0xFFFF;
	idt[num].base_high = (base >> 32) & 0xFFFFFFFF;
	idt[num].selector = selector;
	idt[num].zero = 0;
	idt[num].pad = 0;
	idt[num].flags = flags | (userspace ? 0x60 : 0);
}

void idt_install(void) {
	idtp.limit = sizeof(idt);
	idtp.base  = (uintptr_t)&idt;

	/** ISRs */
	idt_set_gate(0,  _isr0,  0x08, 0x8E, 0);
	idt_set_gate(1,  _isr1,  0x08, 0x8E, 0);
	idt_set_gate(2,  _isr2,  0x08, 0x8E, 0);
	idt_set_gate(3,  _isr3,  0x08, 0x8E, 0);
	idt_set_gate(4,  _isr4,  0x08, 0x8E, 0);
	idt_set_gate(5,  _isr5,  0x08, 0x8E, 0);
	idt_set_gate(6,  _isr6,  0x08, 0x8E, 0);
	idt_set_gate(7,  _isr7,  0x08, 0x8E, 0);
	idt_set_gate(8,  _isr8,  0x08, 0x8E, 0);
	idt_set_gate(9,  _isr9,  0x08, 0x8E, 0);
	idt_set_gate(10, _isr10, 0x08, 0x8E, 0);
	idt_set_gate(11, _isr11, 0x08, 0x8E, 0);
	idt_set_gate(12, _isr12, 0x08, 0x8E, 0);
	idt_set_gate(13, _isr13, 0x08, 0x8E, 0);
	idt_set_gate(14, _isr14, 0x08, 0x8E, 0);
	idt_set_gate(15, _isr15, 0x08, 0x8E, 0);
	idt_set_gate(16, _isr16, 0x08, 0x8E, 0);
	idt_set_gate(17, _isr17, 0x08, 0x8E, 0);
	idt_set_gate(18, _isr18, 0x08, 0x8E, 0);
	idt_set_gate(19, _isr19, 0x08, 0x8E, 0);
	idt_set_gate(20, _isr20, 0x08, 0x8E, 0);
	idt_set_gate(21, _isr21, 0x08, 0x8E, 0);
	idt_set_gate(22, _isr22, 0x08, 0x8E, 0);
	idt_set_gate(23, _isr23, 0x08, 0x8E, 0);
	idt_set_gate(24, _isr24, 0x08, 0x8E, 0);
	idt_set_gate(25, _isr25, 0x08, 0x8E, 0);
	idt_set_gate(26, _isr26, 0x08, 0x8E, 0);
	idt_set_gate(27, _isr27, 0x08, 0x8E, 0);
	idt_set_gate(28, _isr28, 0x08, 0x8E, 0);
	idt_set_gate(29, _isr29, 0x08, 0x8E, 0);
	idt_set_gate(30, _isr30, 0x08, 0x8E, 0);
	idt_set_gate(31, _isr31, 0x08, 0x8E, 0);
	idt_set_gate(32, _irq0,  0x08, 0x8E, 0);
	idt_set_gate(33, _irq1,  0x08, 0x8E, 0);
	idt_set_gate(34, _irq2,  0x08, 0x8E, 0);
	idt_set_gate(35, _irq3,  0x08, 0x8E, 0);
	idt_set_gate(36, _irq4,  0x08, 0x8E, 0);
	idt_set_gate(37, _irq5,  0x08, 0x8E, 0);
	idt_set_gate(38, _irq6,  0x08, 0x8E, 0);
	idt_set_gate(39, _irq7,  0x08, 0x8E, 0);
	idt_set_gate(40, _irq8,  0x08, 0x8E, 0);
	idt_set_gate(41, _irq9,  0x08, 0x8E, 0);
	idt_set_gate(42, _irq10, 0x08, 0x8E, 0);
	idt_set_gate(43, _irq11, 0x08, 0x8E, 0);
	idt_set_gate(44, _irq12, 0x08, 0x8E, 0);
	idt_set_gate(45, _irq13, 0x08, 0x8E, 0);
	idt_set_gate(46, _irq14, 0x08, 0x8E, 0);
	idt_set_gate(47, _irq15, 0x08, 0x8E, 0);

	idt_set_gate(123, _isr123, 0x08, 0x8E, 0); /* Clock interrupt for other processors */
	idt_set_gate(124, _isr124, 0x08, 0x8E, 0); /* Bad TLB shootdown. */
	idt_set_gate(125, _isr125, 0x08, 0x8E, 0); /* Halts everyone. */
	idt_set_gate(126, _isr126, 0x08, 0x8E, 0); /* Does nothing, used to exit wait-for-interrupt sleep. */
	idt_set_gate(127, _isr127, 0x08, 0x8E, 1); /* Legacy system call entry point, called by userspace. */

	asm volatile (
		"lidt %0"
		: : "m"(idtp)
	);
}

void idt_ap_install(void) {
	idtp.limit = sizeof(idt);
	idtp.base  = (uintptr_t)&idt;
	asm volatile (
		"lidt %0"
		: : "m"(idtp)
	);
}

static spin_lock_t dump_lock = {0};
static void dump_regs(struct regs * r) {
	spin_lock(dump_lock);
	dprintf(
		"Registers at interrupt:\n"
		"  $rip=0x%016lx\n"
		"  $rsi=0x%016lx,$rdi=0x%016lx,$rbp=0x%016lx,$rsp=0x%016lx\n"
		"  $rax=0x%016lx,$rbx=0x%016lx,$rcx=0x%016lx,$rdx=0x%016lx\n"
		"  $r8= 0x%016lx,$r9= 0x%016lx,$r10=0x%016lx,$r11=0x%016lx\n"
		"  $r12=0x%016lx,$r13=0x%016lx,$r14=0x%016lx,$r15=0x%016lx\n"
		"  cs=0x%016lx  ss=0x%016lx rflags=0x%016lx int=0x%02lx err=0x%02lx\n",
		r->rip,
		r->rsi, r->rdi, r->rbp, r->rsp,
		r->rax, r->rbx, r->rcx, r->rdx,
		r->r8, r->r9, r->r10, r->r11,
		r->r12, r->r13, r->r14, r->r15,
		r->cs, r->ss, r->rflags, r->int_no, r->err_code
	);

	uint32_t gs_base_low, gs_base_high;
	asm volatile ( "rdmsr" : "=a" (gs_base_low), "=d" (gs_base_high): "c" (0xc0000101) );
	uint32_t kgs_base_low, kgs_base_high;
	asm volatile ( "rdmsr" : "=a" (kgs_base_low), "=d" (kgs_base_high): "c" (0xc0000102) );
	dprintf("  gs=0x%08x%08x kgs=0x%08x%08x\n",
		gs_base_high, gs_base_low, kgs_base_high, kgs_base_low);

	spin_unlock(dump_lock);
}

extern void syscall_handler(struct regs *);

#define IRQ_CHAIN_SIZE  16
#define IRQ_CHAIN_DEPTH 4
static irq_handler_chain_t irq_routines[IRQ_CHAIN_SIZE * IRQ_CHAIN_DEPTH] = { NULL };
static const char * _irq_handler_descriptions[IRQ_CHAIN_SIZE * IRQ_CHAIN_DEPTH] = { NULL };

const char * get_irq_handler(int irq, int chain) {
	if (irq >= IRQ_CHAIN_SIZE) return NULL;
	if (chain >= IRQ_CHAIN_DEPTH) return NULL;
	return _irq_handler_descriptions[IRQ_CHAIN_SIZE * chain + irq];
}

void irq_install_handler(size_t irq, irq_handler_chain_t handler, const char * desc) {
	for (size_t i = 0; i < IRQ_CHAIN_DEPTH; i++) {
		if (irq_routines[i * IRQ_CHAIN_SIZE + irq])
			continue;
		irq_routines[i * IRQ_CHAIN_SIZE + irq] = handler;
		_irq_handler_descriptions[i * IRQ_CHAIN_SIZE + irq ] = desc;
		break;
	}
}

void irq_uninstall_handler(size_t irq) {
	for (size_t i = 0; i < IRQ_CHAIN_DEPTH; i++)
		irq_routines[i * IRQ_CHAIN_SIZE + irq] = NULL;
}

static struct LoadedModule * find_module(uintptr_t addr, char ** name) {
	hashmap_t * modules = modules_get_list();

	for (size_t i = 0; i < modules->size; ++i) {
		hashmap_entry_t * x = modules->entries[i];
		while (x) {
			struct LoadedModule * info = x->value;
			if (info->baseAddress <= addr && addr <= info->baseAddress + info->loadedSize) {
				*name = (char*)x->key;
				return info;
			}
			x = x->next;
		}
	}

	return NULL;
}

static int validate_pointer(uintptr_t base, size_t size) {
	uintptr_t end  = size ? (base + (size - 1)) : base;
	uintptr_t page_base = base >> 12;
	uintptr_t page_end  =  end >> 12;
	for (uintptr_t page = page_base; page <= page_end; ++page) {
		if ((page & 0xffff800000000) != 0 && (page & 0xffff800000000) != 0xffff800000000) return 0;
		union PML * page_entry = mmu_get_page_other(this_core->current_process->thread.page_directory->directory, page << 12);
		if (!page_entry) return 0;
		if (!page_entry->bits.present) return 0;
	}
	return 1;
}

extern char end[];

static uintptr_t matching_symbol(uintptr_t ip, char ** name) {
	hashmap_t * symbols = ksym_get_map();
	uintptr_t best_match = 0;
	for (size_t i = 0; i < symbols->size; ++i) {
		hashmap_entry_t * x = symbols->entries[i];
		while (x) {
			void* sym_addr = x->value;
			char* sym_name = x->key;
			if ((uintptr_t)sym_addr < ip && (uintptr_t)sym_addr > best_match) {
				best_match = (uintptr_t)sym_addr;
				*name = sym_name;
			}
			x = x->next;
		}
	}
	return best_match;
}

static void dump_traceback(uintptr_t ip, uintptr_t bp) {
	int depth = 0;
	int max_depth = 20;

	while (bp && ip && depth < max_depth) {
		dprintf(" 0x%016zx ", ip);
		if (ip >= 0xffffffff80000000UL) {
			char * name = NULL;
			struct LoadedModule * mod = find_module(ip, &name);
			if (mod) {
				dprintf("\a in module '%s', base address %#zx (offset %#zx)\n",
					name, mod->baseAddress, ip - mod->baseAddress);
			} else {
				dprintf("\a (unknown)\n");
			}
		} else if (ip >= (uintptr_t)&end && ip <= 0x800000000000) {
			dprintf("\a in userspace\n");
		} else if (ip <= (uintptr_t)&end) {
			/* Find symbol match */
			char * name;
			uintptr_t addr = matching_symbol(ip, &name);
			if (!addr) {
				dprintf("\a (no match)\n");
			} else {
				dprintf("\a %s+0x%zx\n", name, ip-addr);
			}
		} else {
			dprintf("\a (unknown)\n");
		}
		if (!validate_pointer(bp, sizeof(uintptr_t) * 2)) {
			break;
		}
		ip = *(uintptr_t*)(bp + sizeof(uintptr_t));
		bp = *(uintptr_t*)(bp);
		depth++;
	}
}

static void safe_dump_traceback(struct regs * r) {
	dump_traceback(r->rip, r->rbp);
}

void arch_dump_traceback(void) {
	dump_traceback((uintptr_t)arch_dump_traceback+1, (uintptr_t)__builtin_frame_address(0));
}

static void map_more_stack(uintptr_t fromAddr) {
	volatile process_t * volatile proc = this_core->current_process;
	if (proc->group != 0) {
		proc = process_from_pid(proc->group);
	}

	spin_lock(proc->image.lock);
	for (uintptr_t i = fromAddr; i < proc->image.userstack; i += 0x1000) {
		union PML * page = mmu_get_page(i, MMU_GET_MAKE);
		mmu_frame_allocate(page, MMU_FLAG_WRITABLE);
	}
	proc->image.userstack = fromAddr;
	spin_unlock(proc->image.lock);
}

static void panic(const char * desc, struct regs * r, uintptr_t faulting_address) {
	arch_fatal_prepare();
	dprintf("\033[31mPanic!\033[0m %s pid=%d (%s) at %#zx\n", desc,
		this_core->current_process ? (int)this_core->current_process->id : 0,
		this_core->current_process ? this_core->current_process->name : "kernel",
		faulting_address
	);
	dump_regs(r);
	safe_dump_traceback(r);
	arch_fatal();
}

struct regs * isr_handler(struct regs * r) {
	this_core->interrupt_registers = r;

	if (r->cs != 0x08 && this_core->current_process) {
		this_core->current_process->time_switch = arch_perf_timer();
	}

	switch (r->int_no) {
		case 14: /* Page fault */ {
			uintptr_t faulting_address;
			asm volatile("mov %%cr2, %0" : "=r"(faulting_address));
			if (!this_core->current_process || r->cs == 0x08) {
				panic("Page fault in kernel", r, faulting_address);
			}
			if (faulting_address == 0xFFFFB00F) {
				/* Thread exit */
				task_exit(0);
				break;
			}
			if (faulting_address == 0x8DEADBEEF) {
				return_from_signal_handler();
				break;
			}
			if (faulting_address < 0x800000000000 && faulting_address > 0x700000000000) {
				map_more_stack(faulting_address & 0xFFFFffffFFFFf000);
				break;
			}
			send_signal(this_core->current_process->id, SIGSEGV, 1);
			break;
		}
		case 13: /* GPF */ {
			if (!this_core->current_process || r->cs == 0x08) {
				panic("GPF in kernel", r, 0);
			}
			send_signal(this_core->current_process->id, SIGSEGV, 1);
			break;
		}
		case 8: /* Double fault */ {
			panic("Double fault", r, 0);
			break;
		}
		case 127: /* syscall */ {
			syscall_handler(r);
			asm volatile("sti");
			return r;
		}
		case 123: {
			switch_task(1);
			return r;
		}
		case 39: {
			/* Spurious interrupt */
			break;
		}
		case 1: {
			/* Debug interrupt */
			r->rflags &= ~(1 << 8);
			if (this_core->current_process->flags & PROC_FLAG_TRACE_SIGNALS) {
				ptrace_signal(SIGTRAP, PTRACE_EVENT_SINGLESTEP);
			}
			return r;
		}
		default: {
			if (r->int_no < 32) {
				if (!this_core->current_process || r->cs == 0x08) {
					panic("Unexpected interrupt", r, r->int_no);
				}
				send_signal(this_core->current_process->id, SIGILL, 1);
			} else {
				for (size_t i = 0; i < IRQ_CHAIN_DEPTH; i++) {
					irq_handler_chain_t handler = irq_routines[i * IRQ_CHAIN_SIZE + (r->int_no - 32)];
					if (!handler) break;
					if (handler(r)) {
						goto done;
					}
				}
				irq_ack(r->int_no - 32);
				break;
			}
		}
	}

done:

	if (this_core->current_process == this_core->kernel_idle_task && process_queue && process_queue->head) {
		/* If this is kidle and we got here, instead of finishing the interrupt
		 * we can just switch task and there will probably be something else
		 * to run that was awoken by the interrupt. */
		switch_next();
	}

	return r;
}

