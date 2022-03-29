#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/string.h>
// #include <stdlib.h>
#define MAX_SIZE 128
// static char *"519021910924" = "519021910924";

static int operand1;
static char *operand1_char;
module_param(operand1, int, 0);
static char *operator;
module_param(operator, charp, 0);
static int operand2[MAX_SIZE];
static int ninp;
// static int result = NULL;
static int *result;

module_param_array(operand2, int, &ninp, 0);
// ninp操作系统告诉你传入的数据长度,而且传入的时候给了逗号，应该是智能分了
/* The size of the buffer */
static unsigned long procfs_buffer_size = 0;
static struct proc_dir_entry *proc_ent;
static struct proc_dir_entry *proc_dir;
// static char result_buffer[PROCFS_MAX_SIZE];

static int i = 0;

static int calculate(void)
{

    if (operator[0] == 'a' && operator[1] == 'd' && operator[2] == 'd')
    {
        // printk(KERN_INFO " sizeof(operand2):%ld,sizeof(operand2[0]:%ld", sizeof(operand2), sizeof(operand2[0]));
        //  result = (int *)kmalloc_array(sizeof(operand2) / sizeof(operand2[0]), sizeof(int), GFP_KERNEL);
        for (i = 0; i < ninp; ++i)
        {
            result[i] = operand2[i] + operand1;
            // printk(KERN_INFO "result: %ld\n", result[i]);
            //  printk(KERN_INFO "计算完成，cat查看结果123");
        }
        printk(KERN_INFO "加法完成");
        return 0;
        //     if (copy_from_user(result, usr_buf, len))
        // {
        //         printk(KERN_ERR "Copy from user unfinished\n");
        //         return -EFAULT;
        // }
        // printk(KERN_INFO "write_proc: write %lu bytes\n", len);
    }

    else if (operator[0] == 'm' && operator[1] == 'u' && operator[2] == 'l')
    {
        // result = (int *)kmalloc_array(sizeof(operand2) / sizeof(operand2[0]), sizeof(int), GFP_KERNEL);
        for (i = 0; i < ninp; ++i)
        {
            // printk(KERN_INFO "进入循环%d", i);
            result[i] = operand2[i] * operand1;
        }
        printk(KERN_INFO "乘法完成");
        return 0;
    }

    else
    {
        printk(KERN_INFO "要求的操作不是加也不是乘，出错！(操作符检查)");
        return 1;
    }
}
// static int free_space(void)
// {
//     free
// }
static ssize_t proc_read(struct file *fp, char __user *ubuf, size_t buffer_length, loff_t *pos) // buffer_length 什么用
{
    //由于我直接用cat命令，表示读取所有的，因此这里不会用到buffer_length
    static int finished = 0; //表示一个cat运行两次这个函数
    if (finished)
    {
        printk(KERN_INFO "read_proc: END\n");
        finished = 0;
        return 0;
    }

    char **s = (char **)kmalloc(ninp * sizeof(char *), GFP_KERNEL);
    char *s1 = (char *)kmalloc(8 * ninp * sizeof(char), GFP_KERNEL); //"保存逗号和运算结果"，可能不是8应该是95行的4+一个',',也就是5,但是对实验效果影响不大
    char *s1_start = s1;

    int offset = 0;
    int offset_sum = 0; //总的传输数量
    for (i = 0; i < ninp; ++i)
    {
        s[i] = (char *)kmalloc(4 * sizeof(char), GFP_KERNEL); // 假设最长的int数据为4位
        //  itoa(result[i], s[i], 10);
        sprintf(s[i], "%d", result[i]); //把整数123打印成一个字符串保存在s中
        // strcat(s, s[i]);
        // cout << s[i] << endl;
        if (i < ninp - 1)
            offset = sprintf(s1, "%s%s", s[i], ","); //把他和逗号拼接在一起
        else
            offset = sprintf(s1, "%s", s[i]); //最后一个不要逗号，不美观
        // cout << "offset:" << offset << endl;
        // cout << "s1:" << s1 << endl;
        s1 += offset;
        offset_sum += offset;
    }
    pr_info(" offset_sum:%d", offset_sum);

    pr_info("s1_start:%s", s1_start);
    if (*pos >= offset_sum || copy_to_user(ubuf, s1_start, offset_sum))
    {
        pr_info("copy_to_user failed\n");
    }
    else
    {
        pr_info("procfile read %s\n", fp->f_path.dentry->d_name.name);
        *pos += offset_sum;
        finished = 1;
    }
    for (i = 0; i < ninp; ++i)
    {
        kfree(s[i]);
    }
    kfree(s);
    // kfree(s1);
    kfree(s1_start);
    return offset_sum;
}
// int len = sizeof(result) / sizeof(result[0]);

// static ssize_t procfile_read(struct file *filePointer, char __user *buffer,
//                              size_t buffer_length, loff_t *offset)
// {
// pr_info(*pos);
// pr_info(len);

// static int finished = 0;
// if (finished)
// {
//     printk(KERN_INFO "read_proc: END\n");
//     finished = 0;
//     return 0;
// }
// finished = 1;
// if (copy_to_user(ubuf, result, result_buf_size))
// {
//     printk(KERN_ERR "Copy to user unfinished\n");
//     return -EFAULT;
// }
// printk(KERN_INFO "read_proc: read %lu bytes\n", len);
// return len;
// }

static ssize_t proc_write(struct file *file, const char __user *buff,
                          size_t len, loff_t *off)
{
    procfs_buffer_size = len;                                  //操作系统自动传递
    pr_info("从用户空间传入的长度：%d\n", procfs_buffer_size); // 2是因为换行符？
    if (procfs_buffer_size > 8)                                //最长8位数吧
        procfs_buffer_size = 8;
    operand1_char = (char *)kmalloc(procfs_buffer_size * sizeof(char), GFP_KERNEL);
    char *operand1_char_start = operand1_char;
    if (copy_from_user(operand1_char, buff, procfs_buffer_size))
    {
        pr_info("copy_from_user failed\n");
        return -EFAULT;
    }

    // operand1_char[procfs_buffer_size & (procfs_buffer_size - 1)] = '\0'; //？没看懂加上这一句话没有输出了？为什么
    pr_info("procfile write %s\n", operand1_char);
    // if (!kstrtoint(operand1_char, 10, operand1))
    // {
    //     pr_warn("输入的字符串转化为int失败!");不能用这个，报错了
    // };
    int index = 0;
    int num = 0;
    while (index < procfs_buffer_size - 1) //也可以！=\0
    {
        pr_info("第%d次循环的num值:%d\n", index, num);
        num = num * 10 + (*operand1_char - '0');
        operand1_char++;
        index++;
    }
    operand1 = num;
    // pr_info("pr_info 修改后的operand1的值为:%d\n", operand1); //这一行为什么不打印？
    printk(KERN_INFO "printk 修改后的operand1:%d\n", operand1);
    calculate(); //重新计算
    kfree(operand1_char_start);
    return procfs_buffer_size;
}

static const struct proc_ops proc_fops = {
    .proc_read = proc_read,
    .proc_write = proc_write,
};

static int __init proc_init(void)
{
    /* TODO */

    printk(KERN_INFO "welcome to this calculator");
    printk(KERN_INFO "operand1:%d\n", operand1);
    printk(KERN_INFO "operator = %s\n", operator);
    proc_dir = proc_mkdir("519021910924", NULL);
    if (!proc_dir)
    {
        printk(KERN_ERR "Unable to create /proc/519021910924 /\n");
        return -EINVAL;
    }
    else
    {
        printk(KERN_ERR "Create /proc/519021910924 Success!\n");
    }
    proc_ent = proc_create("calc", 0666, proc_dir, &proc_fops);
    if (!proc_ent)
    {
        printk(KERN_ERR "Unable to  create /proc/519021910924/calc /\n");
        proc_remove(proc_dir); //把父文件删了应该自己也没了
        return -EINVAL;
    }
    else
    {
        printk(KERN_ERR "Create /proc/519021910924/calc Success! \n");
    }

    // for (i = 0; i < sizeof(operand2) / sizeof(int); ++i)
    // {
    //     printk(KERN_INFO "operand2[%d] = %d\n", i, operand2[i]);
    // }
    // printk(KERN_INFO "operator1231241 :%d%d%d", operator[0], operator[1], operator[2]);
    // printk(KERN_INFO "传入的数据数量:%ld", ninp);

    result = (int *)kmalloc(ninp * sizeof(int), GFP_KERNEL); //酱紫漏写为(int )kmalloc导致内存泄漏？
    // printk(KERN_INFO "result分配没问题");
    int flag = calculate();
    if (flag)
        return flag;
    else
    {
        // printk(KERN_INFO "sizeof(result) :%ld,sizeof(result[0]:%ld\n", sizeof(result), sizeof(result[0]));

        printk(KERN_INFO "计算完成，cat查看结果");
        return 0;
    }
    // copy_from_user(proc_buf, usr_buf, len)
}

static void __exit proc_exit(void)
{
    proc_remove(proc_ent); //先移除文件
    proc_remove(proc_dir); //删除文件夹
    kfree(result);         //释放result
    printk(KERN_INFO "Exiting Calculate...\n");
}

module_init(proc_init);
module_exit(proc_exit);
MODULE_LICENSE("GPL");