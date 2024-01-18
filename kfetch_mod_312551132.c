#include <linux/init.h> // #include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

// info
#include <linux/utsname.h>
#include <linux/time_namespace.h>
#include <linux/mm.h>
#include <linux/sched/stat.h>


#define MAJOR_NUM 509
#define SUCCESS 0
#define DEVICE_NAME "kfetch"
#define LOGO_LINE 8
#define LOGOLINE_LEN 19

#define KFETCH_RELEASE   (1 << 0)
#define KFETCH_NUM_CPUS  (1 << 1)
#define KFETCH_CPU_MODEL (1 << 2)
#define KFETCH_MEM       (1 << 3)
#define KFETCH_UPTIME    (1 << 4)
#define KFETCH_NUM_PROCS (1 << 5)

MODULE_LICENSE("GPL");

// Prototypes
static int __init kfetch_mod_init(void);
static void __exit kfetch_mod_exit(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);



// Global variables
// static int Major;
static int Device_Open = 0;
static int mask = 63;

static const char logo[LOGO_LINE][LOGOLINE_LEN] = {
    "                   ",
    "        .-.        ",
    "       (.. |       ",
    "       <>  |       ",
    "      / --- \\      ",
    "     ( |   | |     ",
    "   |\\\\_)___/\\)/\\   ",
    "  <__)------(__/   "
};
static int logo_line;

static char msg[1024];
static char *msg_Ptr;

static struct file_operations fops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

static int kfetch_mod_init(void) {
    printk(KERN_INFO "Initializing the kfetch module\n");
    register_chrdev(MAJOR_NUM, DEVICE_NAME, &fops);
    return SUCCESS;
}

static void kfetch_mod_exit(void) {
    printk(KERN_INFO "Exiting the kfetch module\n");
    unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
}

static int device_open(struct inode *inode, struct file *filp) {
    printk(KERN_INFO "Opening the device\n");
    if (Device_Open)
        return -EBUSY;
    Device_Open++;
    msg_Ptr = msg;
    try_module_get(THIS_MODULE);
    return SUCCESS;
}

static int device_release(struct inode *inode, struct file *filp) {
    printk(KERN_INFO "Releasing the device\n");
    Device_Open--;
    module_put(THIS_MODULE);
    return SUCCESS;
}

int read_logo_line(void) {
    if (logo_line >= LOGO_LINE)
        return 0;
    memcpy(msg_Ptr, logo + (logo_line++), LOGOLINE_LEN);
    msg_Ptr += LOGOLINE_LEN;
    return 1;
}

static ssize_t print_to_msg(const char *format, ...) {
    va_list args;
    int len;
    va_start(args, format);
    len = vsprintf(msg_Ptr, format, args);
    va_end(args);
    msg_Ptr += len;
    return len;
}

static void print_line_separator(int length) {
    int i;
    for (i = 0; i < length - 1; i++) {
       *msg_Ptr++ = '-';
    }
    *msg_Ptr++ = '\n';
}

static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t *offset) {
    int len = 0;
    int process_counter = 0;
    struct task_struct *task_list;
    struct sysinfo info;
    struct cpuinfo_x86 *c;
    struct timespec64 uptime;

    logo_line = 0;

    read_logo_line();
    len = print_to_msg("%s\n", utsname()->nodename);

    read_logo_line();
    print_line_separator(len);

    if (mask & KFETCH_NUM_PROCS) {
        read_logo_line();
    }

    if (mask & KFETCH_NUM_PROCS) {
        for_each_process(task_list)
            ++process_counter;
       len = print_to_msg("Procs:\t%d\n", process_counter);
    }

    if (mask & KFETCH_RELEASE) {
        read_logo_line();
    }

    if (mask & KFETCH_RELEASE) {
        len = print_to_msg("Release:\t%s\n", utsname()->release);
    }
    if (mask & KFETCH_MEM) {
        read_logo_line();
    }
    if (mask & KFETCH_MEM) {
        si_meminfo(&info);
        len = print_to_msg("Mem:\t\t%lu MB / %lu MB\n", info.freeram >> 10, info.totalram >> 10);
    }

    if (mask & KFETCH_CPU_MODEL) {
        read_logo_line();
    }
    if (mask & KFETCH_CPU_MODEL) {
        c = &cpu_data(0);
        len = print_to_msg("CPU:\t\t%s\n", c->x86_model_id[0] ? c->x86_model_id : "unknown");
    }

    if (mask & KFETCH_UPTIME) {
        read_logo_line();
    }

    if (mask & KFETCH_UPTIME) {
        ktime_get_boottime_ts64(&uptime);
        timens_add_boottime(&uptime);
        len = print_to_msg("Uptime:\t%lu mins\n", (unsigned long)uptime.tv_sec / 60);
    }

    if (mask & KFETCH_NUM_CPUS) {
        read_logo_line();
    }
    if (mask & KFETCH_NUM_CPUS) {
        len = print_to_msg("CPUs:\t%u / %u\n", num_online_cpus(), num_present_cpus());
    }


    for (; read_logo_line();)
        *msg_Ptr++ = '\n';

    *msg_Ptr++ = '\0';

    msg_Ptr = msg;

    for (msg_Ptr = msg; *msg_Ptr;){
        put_user(*(msg_Ptr++), buffer++);
    }
   put_user(*(msg_Ptr++), buffer++);
   return strlen(msg);
}

static ssize_t device_write(struct file *filp, const char *buff, size_t len, loff_t *off) {
    char *char_mask = (char *)&mask;
    int i = 0;
    for (; i++ < len;) {
        get_user(*(char_mask++), buff++);
    }
    printk(KERN_INFO "Setting mask from user:%d.\n", mask);
    return SUCCESS;
}



module_init(kfetch_mod_init);
module_exit(kfetch_mod_exit);
