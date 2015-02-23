#include "CPU.h"

#define CPU_Reg_eax 0
#define CPU_Reg_ax 1
#define CPU_Reg_al 2
#define CPU_Reg_ah 3
#define CPU_Reg_ebx 4
#define CPU_Reg_bx 5
#define CPU_Reg_bl 6
#define CPU_Reg_bh 7
#define CPU_Reg_ecx 8
#define CPU_Reg_cx 9
#define CPU_Reg_cl 10
#define CPU_Reg_ch 11
#define CPU_Reg_edx 12
#define CPU_Reg_dx 13
#define CPU_Reg_dl 14
#define CPU_Reg_dh 15
#define CPU_Reg_esp 16
#define CPU_Reg_sp 17
#define CPU_Reg_ebp 18
#define CPU_Reg_bp 19
#define CPU_Reg_esfp 20
#define CPU_Reg_sfp 21
#define CPU_Reg_ebfp 22
#define CPU_Reg_bfp 23
#define CPU_Reg_eip 24
#define CPU_Reg_ip 25
#define CPU_Reg_r0 26
#define CPU_Reg_r1 27
#define CPU_Reg_r2 28
#define CPU_Reg_r3 29
#define CPU_Reg_r4 30
#define CPU_Reg_r5 31
#define CPU_Reg_r6 32
#define CPU_Reg_r7 33
#define CPU_Reg_f0 34
#define CPU_Reg_f1 35
#define CPU_Reg_f2 36
#define CPU_Reg_f3 37
#define CPU_Reg_f4 38
#define CPU_Reg_f5 39
#define CPU_Reg_f6 40
#define CPU_Reg_f7 41

cpubasetype CPU_getIntRegisterValue(CPU * cpu, cpubasetype id)
{
	if (id == CPU_Reg_esp)
    {
        return cpu->esp;
    }
    if (id == CPU_Reg_ebp)
    {
        return cpu->ebp;
    }
    if (id >= CPU_Reg_f0) //is float register
    {
        return 0;
    }
    if (id <= CPU_Reg_esfp)
    {
        if (id <= CPU_Reg_cl)
        {
            if (id <= CPU_Reg_bx)
            {
                if (id <= CPU_Reg_al)
                {
                    if (id <= CPU_Reg_ax)
                    {
                        if (id <= CPU_Reg_eax)
                        {
                            //CPU_Reg_eax
                            return cpu->eax;
                        }
                        else
                        {
                            //CPU_Reg_ax
                            return cpu->eax&0xFFFF;
                        }
                    }
                    else
                    {
                        //CPU_Reg_al
                        return cpu->eax&0xFF;
                    }
                }
                else
                {
                    if (id <= CPU_Reg_ebx)
                    {
                        if (id <= CPU_Reg_ah)
                        {
                            //CPU_Reg_ah
                            return (cpu->eax&0xFF00)>>8;
                        }
                        else
                        {
                            //CPU_Reg_ebx
                            return cpu->ebx;
                        }
                    }
                    else
                    {
                        //CPU_Reg_bx
                        return cpu->ebx&0xFFFF;
                    }
                }
            }
            else
            {
                if (id <= CPU_Reg_ecx)
                {
                    if (id <= CPU_Reg_bh)
                    {
                        if (id <= CPU_Reg_bl)
                        {
                            //CPU_Reg_bl
                            return cpu->ebx&0xFF;
                        }
                        else
                        {
                            //CPU_Reg_bh
                            return (cpu->ebx&0xFF00)>>8;
                        }
                    }
                    else
                    {
                        //CPU_Reg_ecx
                        return cpu->ecx;
                    }
                }
                else
                {
                    if (id <= CPU_Reg_cx)
                    {
                        //CPU_Reg_cx
                        return cpu->ecx&0xFFFF;
                    }
                    else
                    {
                        //CPU_Reg_cl
                        return cpu->ecx&0xFF;
                    }
                }
            }
        }
        else
        {
            if (id <= CPU_Reg_dh)
            {
                if (id <= CPU_Reg_dx)
                {
                    if (id <= CPU_Reg_edx)
                    {
                        if (id <= CPU_Reg_ch)
                        {
                            //CPU_Reg_ch
                            return (cpu->ecx&0xFF00)>>8;
                        }
                        else
                        {
                            //CPU_Reg_edx
                            return cpu->edx;
                        }
                    }
                    else
                    {
                        //CPU_Reg_dx
                        return cpu->edx&0xFFFF;
                    }
                }
                else
                {
                    if (id <= CPU_Reg_dl)
                    {
                        //CPU_Reg_dl
                        return cpu->edx&0xFF;
                    }
                    else
                    {
                        //CPU_Reg_dh
                        return (cpu->edx&0xFF00)>>8;
                    }
                }
            }
            else
            {
                if (id <= CPU_Reg_ebp)
                {
                    if (id <= CPU_Reg_sp)
                    {
                        if (id <= CPU_Reg_esp)
                        {
                            //CPU_Reg_esp
                            return cpu->esp;
                        }
                        else
                        {
                            //CPU_Reg_sp
                            return cpu->esp&0xFFFF;
                        }
                    }
                    else
                    {
                        //CPU_Reg_ebp
                        return cpu->ebp;
                    }
                }
                else
                {
                    if (id <= CPU_Reg_bp)
                    {
                        //CPU_Reg_bp
                        return cpu->ebp&0xFFFF;
                    }
                    else
                    {
                        //CPU_Reg_esfp
                        return cpu->esfp;
                    }
                }
            }
        }
    }
    else
    {
        if (id <= CPU_Reg_r5)
        {
            if (id <= CPU_Reg_r0)
            {
                if (id <= CPU_Reg_bfp)
                {
                    if (id <= CPU_Reg_ebfp)
                    {
                        if (id <= CPU_Reg_sfp)
                        {
                            //CPU_Reg_sfp
                            return cpu->esfp&0xFFFF;
                        }
                        else
                        {
                            //CPU_Reg_ebfp
                            return cpu->ebfp;
                        }
                    }
                    else
                    {
                        //CPU_Reg_bfp
                        return cpu->ebfp&0xFFFF;
                    }
                }
                else
                {
                    if (id <= CPU_Reg_ip)
                    {
                        if (id <= CPU_Reg_eip)
                        {
                            //CPU_Reg_eip
                            return cpu->eip;
                        }
                        else
                        {
                            //CPU_Reg_ip
                            return cpu->eip&0xFFFF;
                        }
                    }
                    else
                    {
                        //CPU_Reg_r0
                        return cpu->r0;
                    }
                }
            }
            else
            {
                if (id <= CPU_Reg_r3)
                {
                    if (id <= CPU_Reg_r2)
                    {
                        if (id <= CPU_Reg_r1)
                        {
                            //CPU_Reg_r1
                            return cpu->r1;
                        }
                        else
                        {
                            //CPU_Reg_r2
                            return cpu->r2;
                        }
                    }
                    else
                    {
                        //CPU_Reg_r3
                        return cpu->r3;
                    }
                }
                else
                {
                    if (id <= CPU_Reg_r4)
                    {
                        //CPU_Reg_r4
                        return cpu->r4;
                    }
                    else
                    {
                        //CPU_Reg_r5
                        return cpu->r5;
                    }
                }
            }
        }
        else
        {
            if (id <= CPU_Reg_f2)
            {
                if (id <= CPU_Reg_f0)
                {
                    if (id <= CPU_Reg_r7)
                    {
                        if (id <= CPU_Reg_r6)
                        {
                            //CPU_Reg_r6
                            return cpu->r6;
                        }
                        else
                        {
                            //CPU_Reg_r7
                            return cpu->r7;
                        }
                    }
                    else
                    {
                        //CPU_Reg_f0
                        return 0;
                    }
                }
                else
                {
                    if (id <= CPU_Reg_f1)
                    {
                        //CPU_Reg_f1
                        return 0;
                    }
                    else
                    {
                        //CPU_Reg_f2
                        return 0;
                    }
                }
            }
            else
            {
                if (id <= CPU_Reg_f5)
                {
                    if (id <= CPU_Reg_f4)
                    {
                        if (id <= CPU_Reg_f3)
                        {
                            //CPU_Reg_f3
                            return 0;
                        }
                        else
                        {
                            //CPU_Reg_f4
                            return 0;
                        }
                    }
                    else
                    {
                        //CPU_Reg_f5
                        return 0;
                    }
                }
                else
                {
                    if (id <= CPU_Reg_f6)
                    {
                        //CPU_Reg_f6
                        return 0;
                    }
                    else
                    {
                        //CPU_Reg_f7
                        return 0;
                    }
                }
            }
        }
    }
}

void CPU_setIntRegisterValue(CPU * cpu, cpubasetype id, cpubasetype data)
{
    if (id == CPU_Reg_esp)
    {
        cpu->esp=data;
        return;
    }
    if (id == CPU_Reg_ebp)
    {
        cpu->ebp=data;
        return;
    }
    if (id >= CPU_Reg_f0) //is float register
    {
        return;
    }
    if (id <= CPU_Reg_esfp)
    {
        if (id <= CPU_Reg_cl)
        {
            if (id <= CPU_Reg_bx)
            {
                if (id <= CPU_Reg_al)
                {
                    if (id <= CPU_Reg_ax)
                    {
                        if (id <= CPU_Reg_eax)
                        {
                            //CPU_Reg_eax
                            cpu->eax=data;
                        }
                        else
                        {
                            //CPU_Reg_ax
                            cpu->eax = (cpu->eax&0xFFFFFFFFFFFF0000) | (data&0xFFFF);
                        }
                    }
                    else
                    {
                        //CPU_Reg_al
                        cpu->eax = (cpu->eax&0xFFFFFFFFFFFFFF00) | (data&0xFF);
                    }
                }
                else
                {
                    if (id <= CPU_Reg_ebx)
                    {
                        if (id <= CPU_Reg_ah)
                        {
                            //CPU_Reg_ah
                            cpu->eax = (cpu->eax&0xFFFFFFFFFFFF00FF) | ((data&0xFF)<<8);
                        }
                        else
                        {
                            //CPU_Reg_ebx
                            cpu->ebx=data;
                        }
                    }
                    else
                    {
                        //CPU_Reg_bx
                        cpu->ebx = (cpu->ebx&0xFFFFFFFFFFFF0000) | (data&0xFFFF);
                    }
                }
            }
            else
            {
                if (id <= CPU_Reg_ecx)
                {
                    if (id <= CPU_Reg_bh)
                    {
                        if (id <= CPU_Reg_bl)
                        {
                            //CPU_Reg_bl
                            cpu->ebx = (cpu->ebx&0xFFFFFFFFFFFFFF00) | (data&0xFF);
                        }
                        else
                        {
                            //CPU_Reg_bh
                            cpu->ebx = (cpu->ebx&0xFFFFFFFFFFFF00FF) | ((data&0xFF)<<8);
                        }
                    }
                    else
                    {
                        //CPU_Reg_ecx
                        cpu->ecx=data;
                    }
                }
                else
                {
                    if (id <= CPU_Reg_cx)
                    {
                        //CPU_Reg_cx
                        cpu->ecx = (cpu->ecx&0xFFFFFFFFFFFF0000) | (data&0xFFFF);
                    }
                    else
                    {
                        //CPU_Reg_cl
                        cpu->ecx = (cpu->ecx&0xFFFFFFFFFFFFFF00) | (data&0xFF);
                    }
                }
            }
        }
        else
        {
            if (id <= CPU_Reg_dh)
            {
                if (id <= CPU_Reg_dx)
                {
                    if (id <= CPU_Reg_edx)
                    {
                        if (id <= CPU_Reg_ch)
                        {
                            //CPU_Reg_ch
                            cpu->ecx = (cpu->ecx&0xFFFFFFFFFFFF00FF) | ((data&0xFF)<<8);
                        }
                        else
                        {
                            //CPU_Reg_edx
                            cpu->edx=data;
                        }
                    }
                    else
                    {
                        //CPU_Reg_dx
                        cpu->edx = (cpu->edx&0xFFFFFFFFFFFF0000) | (data&0xFFFF);
                    }
                }
                else
                {
                    if (id <= CPU_Reg_dl)
                    {
                        //CPU_Reg_dl
                        cpu->edx = (cpu->edx&0xFFFFFFFFFFFFFF00) | (data&0xFF);
                    }
                    else
                    {
                        //CPU_Reg_dh
                        cpu->edx = (cpu->edx&0xFFFFFFFFFFFF00FF) | ((data&0xFF)<<8);
                    }
                }
            }
            else
            {
                if (id <= CPU_Reg_ebp)
                {
                    if (id <= CPU_Reg_sp)
                    {
                        if (id <= CPU_Reg_esp)
                        {
                            //CPU_Reg_esp
                            cpu->esp=data;
                        }
                        else
                        {
                            //CPU_Reg_sp
                            cpu->esp = (cpu->esp&0xFFFFFFFFFFFF0000) | (data&0xFFFF);
                        }
                    }
                    else
                    {
                        //CPU_Reg_ebp
                        cpu->ebp=data;
                    }
                }
                else
                {
                    if (id <= CPU_Reg_bp)
                    {
                        //CPU_Reg_bp
                        cpu->ebp = (cpu->ebp&0xFFFFFFFFFFFF0000) | (data&0xFFFF);
                    }
                    else
                    {
                        //CPU_Reg_esfp
                        cpu->esfp=data;
                    }
                }
            }
        }
    }
    else
    {
        if (id <= CPU_Reg_r5)
        {
            if (id <= CPU_Reg_r0)
            {
                if (id <= CPU_Reg_bfp)
                {
                    if (id <= CPU_Reg_ebfp)
                    {
                        if (id <= CPU_Reg_sfp)
                        {
                            //CPU_Reg_sfp
                            cpu->esfp = (cpu->esfp&0xFFFFFFFFFFFF0000) | (data&0xFFFF);
                        }
                        else
                        {
                            //CPU_Reg_ebfp
                            cpu->ebfp=data;
                        }
                    }
                    else
                    {
                        //CPU_Reg_bfp
                        cpu->ebfp = (cpu->ebfp&0xFFFFFFFFFFFF0000) | (data&0xFFFF);
                    }
                }
                else
                {
                    if (id <= CPU_Reg_ip)
                    {
                        if (id <= CPU_Reg_eip)
                        {
                            //CPU_Reg_eip
                            cpu->eip=data;
                        }
                        else
                        {
                            //CPU_Reg_ip
                            cpu->eip = (cpu->eip&0xFFFFFFFFFFFF0000) | (data&0xFFFF);
                        }
                    }
                    else
                    {
                        //CPU_Reg_r0
                        cpu->r0=data;
                    }
                }
            }
            else
            {
                if (id <= CPU_Reg_r3)
                {
                    if (id <= CPU_Reg_r2)
                    {
                        if (id <= CPU_Reg_r1)
                        {
                            //CPU_Reg_r1
                            cpu->r1=data;
                        }
                        else
                        {
                            //CPU_Reg_r2
                            cpu->r2=data;
                        }
                    }
                    else
                    {
                        //CPU_Reg_r3
                        cpu->r3=data;
                    }
                }
                else
                {
                    if (id <= CPU_Reg_r4)
                    {
                        //CPU_Reg_r4
                        cpu->r4=data;
                    }
                    else
                    {
                        //CPU_Reg_r5
                        cpu->r5=data;
                    }
                }
            }
        }
        else
        {
            if (id <= CPU_Reg_f2)
            {
                if (id <= CPU_Reg_f0)
                {
                    if (id <= CPU_Reg_r7)
                    {
                        if (id <= CPU_Reg_r6)
                        {
                            //CPU_Reg_r6
                            cpu->r6=data;
                        }
                        else
                        {
                            //CPU_Reg_r7
                            cpu->r7=data;
                        }
                    }
                    else
                    {
                        //CPU_Reg_f0
                        return;
                    }
                }
                else
                {
                    if (id <= CPU_Reg_f1)
                    {
                        //CPU_Reg_f1
                        return;
                    }
                    else
                    {
                        //CPU_Reg_f2
                        return;
                    }
                }
            }
            else
            {
                if (id <= CPU_Reg_f5)
                {
                    if (id <= CPU_Reg_f4)
                    {
                        if (id <= CPU_Reg_f3)
                        {
                            //CPU_Reg_f3
                            return;
                        }
                        else
                        {
                            //CPU_Reg_f4
                            return;
                        }
                    }
                    else
                    {
                        //CPU_Reg_f5
                        return;
                    }
                }
                else
                {
                    if (id <= CPU_Reg_f6)
                    {
                        //CPU_Reg_f6
                        return;
                    }
                    else
                    {
                        //CPU_Reg_f7
                        return;
                    }
                }
            }
        }
    }
}

double CPU_getFloatRegisterValue(CPU * cpu, cpubasetype id)
{
    if (id <= CPU_Reg_r7) //is int register
    {
        return 0.0;
    }
    if (id <= CPU_Reg_esfp)
    {
        if (id <= CPU_Reg_cl)
        {
            if (id <= CPU_Reg_bx)
            {
                if (id <= CPU_Reg_al)
                {
                    if (id <= CPU_Reg_ax)
                    {
                        if (id <= CPU_Reg_eax)
                        {
                            //CPU_Reg_eax
                            return 0.0;
                        }
                        else
                        {
                            //CPU_Reg_ax
                            return 0.0;
                        }
                    }
                    else
                    {
                        //CPU_Reg_al
                        return 0.0;
                    }
                }
                else
                {
                    if (id <= CPU_Reg_ebx)
                    {
                        if (id <= CPU_Reg_ah)
                        {
                            //CPU_Reg_ah
                            return 0.0;
                        }
                        else
                        {
                            //CPU_Reg_ebx
                            return 0.0;
                        }
                    }
                    else
                    {
                        //CPU_Reg_bx
                        return 0.0;
                    }
                }
            }
            else
            {
                if (id <= CPU_Reg_ecx)
                {
                    if (id <= CPU_Reg_bh)
                    {
                        if (id <= CPU_Reg_bl)
                        {
                            //CPU_Reg_bl
                            return 0.0;
                        }
                        else
                        {
                            //CPU_Reg_bh
                            return 0.0;
                        }
                    }
                    else
                    {
                        //CPU_Reg_ecx
                        return 0.0;
                    }
                }
                else
                {
                    if (id <= CPU_Reg_cx)
                    {
                        //CPU_Reg_cx
                        return 0.0;
                    }
                    else
                    {
                        //CPU_Reg_cl
                        return 0.0;
                    }
                }
            }
        }
        else
        {
            if (id <= CPU_Reg_dh)
            {
                if (id <= CPU_Reg_dx)
                {
                    if (id <= CPU_Reg_edx)
                    {
                        if (id <= CPU_Reg_ch)
                        {
                            //CPU_Reg_ch
                            return 0.0;
                        }
                        else
                        {
                            //CPU_Reg_edx
                            return 0.0;
                        }
                    }
                    else
                    {
                        //CPU_Reg_dx
                        return 0.0;
                    }
                }
                else
                {
                    if (id <= CPU_Reg_dl)
                    {
                        //CPU_Reg_dl
                        return 0.0;
                    }
                    else
                    {
                        //CPU_Reg_dh
                        return 0.0;
                    }
                }
            }
            else
            {
                if (id <= CPU_Reg_ebp)
                {
                    if (id <= CPU_Reg_sp)
                    {
                        if (id <= CPU_Reg_esp)
                        {
                            //CPU_Reg_esp
                            return 0.0;
                        }
                        else
                        {
                            //CPU_Reg_sp
                            return 0.0;
                        }
                    }
                    else
                    {
                        //CPU_Reg_ebp
                        return 0.0;
                    }
                }
                else
                {
                    if (id <= CPU_Reg_bp)
                    {
                        //CPU_Reg_bp
                        return 0.0;
                    }
                    else
                    {
                        //CPU_Reg_esfp
                        return 0.0;
                    }
                }
            }
        }
    }
    else
    {
        if (id <= CPU_Reg_r5)
        {
            if (id <= CPU_Reg_r0)
            {
                if (id <= CPU_Reg_bfp)
                {
                    if (id <= CPU_Reg_ebfp)
                    {
                        if (id <= CPU_Reg_sfp)
                        {
                            //CPU_Reg_sfp
                            return 0.0;
                        }
                        else
                        {
                            //CPU_Reg_ebfp
                            return 0.0;
                        }
                    }
                    else
                    {
                        //CPU_Reg_bfp
                        return 0.0;
                    }
                }
                else
                {
                    if (id <= CPU_Reg_ip)
                    {
                        if (id <= CPU_Reg_eip)
                        {
                            //CPU_Reg_eip
                            return 0.0;
                        }
                        else
                        {
                            //CPU_Reg_ip
                            return 0.0;
                        }
                    }
                    else
                    {
                        //CPU_Reg_r0
                        return 0.0;
                    }
                }
            }
            else
            {
                if (id <= CPU_Reg_r3)
                {
                    if (id <= CPU_Reg_r2)
                    {
                        if (id <= CPU_Reg_r1)
                        {
                            //CPU_Reg_r1
                            return 0.0;
                        }
                        else
                        {
                            //CPU_Reg_r2
                            return 0.0;
                        }
                    }
                    else
                    {
                        //CPU_Reg_r3
                        return 0.0;
                    }
                }
                else
                {
                    if (id <= CPU_Reg_r4)
                    {
                        //CPU_Reg_r4
                        return 0.0;
                    }
                    else
                    {
                        //CPU_Reg_r5
                        return 0.0;
                    }
                }
            }
        }
        else
        {
            if (id <= CPU_Reg_f2)
            {
                if (id <= CPU_Reg_f0)
                {
                    if (id <= CPU_Reg_r7)
                    {
                        if (id <= CPU_Reg_r6)
                        {
                            //CPU_Reg_r6
                            return 0.0;
                        }
                        else
                        {
                            //CPU_Reg_r7
                            return 0.0;
                        }
                    }
                    else
                    {
                        //CPU_Reg_f0
                        return cpu->f0;
                    }
                }
                else
                {
                    if (id <= CPU_Reg_f1)
                    {
                        //CPU_Reg_f1
                        return cpu->f1;
                    }
                    else
                    {
                        //CPU_Reg_f2
                        return cpu->f2;
                    }
                }
            }
            else
            {
                if (id <= CPU_Reg_f5)
                {
                    if (id <= CPU_Reg_f4)
                    {
                        if (id <= CPU_Reg_f3)
                        {
                            //CPU_Reg_f3
                            return cpu->f3;
                        }
                        else
                        {
                            //CPU_Reg_f4
                            return cpu->f4;
                        }
                    }
                    else
                    {
                        //CPU_Reg_f5
                        return cpu->f5;
                    }
                }
                else
                {
                    if (id <= CPU_Reg_f6)
                    {
                        //CPU_Reg_f6
                        return cpu->f6;
                    }
                    else
                    {
                        //CPU_Reg_f7
                        return cpu->f7;
                    }
                }
            }
        }
    }
}

void CPU_setFloatRegisterValue(CPU * cpu, cpubasetype id, double data)
{
    if (id <= CPU_Reg_r7) //is int register
    {
        return;
    }
    if (id <= CPU_Reg_esfp)
    {
        if (id <= CPU_Reg_cl)
        {
            if (id <= CPU_Reg_bx)
            {
                if (id <= CPU_Reg_al)
                {
                    if (id <= CPU_Reg_ax)
                    {
                        if (id <= CPU_Reg_eax)
                        {
                            //CPU_Reg_eax
                            return;
                        }
                        else
                        {
                            //CPU_Reg_ax
                            return;
                        }
                    }
                    else
                    {
                        //CPU_Reg_al
                        return;
                    }
                }
                else
                {
                    if (id <= CPU_Reg_ebx)
                    {
                        if (id <= CPU_Reg_ah)
                        {
                            //CPU_Reg_ah
                            return;
                        }
                        else
                        {
                            //CPU_Reg_ebx
                            return;
                        }
                    }
                    else
                    {
                        //CPU_Reg_bx
                        return;
                    }
                }
            }
            else
            {
                if (id <= CPU_Reg_ecx)
                {
                    if (id <= CPU_Reg_bh)
                    {
                        if (id <= CPU_Reg_bl)
                        {
                            //CPU_Reg_bl
                            return;
                        }
                        else
                        {
                            //CPU_Reg_bh
                            return;
                        }
                    }
                    else
                    {
                        //CPU_Reg_ecx
                        return;
                    }
                }
                else
                {
                    if (id <= CPU_Reg_cx)
                    {
                        //CPU_Reg_cx
                        return;
                    }
                    else
                    {
                        //CPU_Reg_cl
                        return;
                    }
                }
            }
        }
        else
        {
            if (id <= CPU_Reg_dh)
            {
                if (id <= CPU_Reg_dx)
                {
                    if (id <= CPU_Reg_edx)
                    {
                        if (id <= CPU_Reg_ch)
                        {
                            //CPU_Reg_ch
                            return;
                        }
                        else
                        {
                            //CPU_Reg_edx
                            return;
                        }
                    }
                    else
                    {
                        //CPU_Reg_dx
                        return;
                    }
                }
                else
                {
                    if (id <= CPU_Reg_dl)
                    {
                        //CPU_Reg_dl
                        return;
                    }
                    else
                    {
                        //CPU_Reg_dh
                        return;
                    }
                }
            }
            else
            {
                if (id <= CPU_Reg_ebp)
                {
                    if (id <= CPU_Reg_sp)
                    {
                        if (id <= CPU_Reg_esp)
                        {
                            //CPU_Reg_esp
                            return;
                        }
                        else
                        {
                            //CPU_Reg_sp
                            return;
                        }
                    }
                    else
                    {
                        //CPU_Reg_ebp
                        return;
                    }
                }
                else
                {
                    if (id <= CPU_Reg_bp)
                    {
                        //CPU_Reg_bp
                        return;
                    }
                    else
                    {
                        //CPU_Reg_esfp
                        return;
                    }
                }
            }
        }
    }
    else
    {
        if (id <= CPU_Reg_r5)
        {
            if (id <= CPU_Reg_r0)
            {
                if (id <= CPU_Reg_bfp)
                {
                    if (id <= CPU_Reg_ebfp)
                    {
                        if (id <= CPU_Reg_sfp)
                        {
                            //CPU_Reg_sfp
                            return;
                        }
                        else
                        {
                            //CPU_Reg_ebfp
                            return;
                        }
                    }
                    else
                    {
                        //CPU_Reg_bfp
                        return;
                    }
                }
                else
                {
                    if (id <= CPU_Reg_ip)
                    {
                        if (id <= CPU_Reg_eip)
                        {
                            //CPU_Reg_eip
                            return;
                        }
                        else
                        {
                            //CPU_Reg_ip
                            return;
                        }
                    }
                    else
                    {
                        //CPU_Reg_r0
                        return;
                    }
                }
            }
            else
            {
                if (id <= CPU_Reg_r3)
                {
                    if (id <= CPU_Reg_r2)
                    {
                        if (id <= CPU_Reg_r1)
                        {
                            //CPU_Reg_r1
                            return;
                        }
                        else
                        {
                            //CPU_Reg_r2
                            return;
                        }
                    }
                    else
                    {
                        //CPU_Reg_r3
                        return;
                    }
                }
                else
                {
                    if (id <= CPU_Reg_r4)
                    {
                        //CPU_Reg_r4
                        return;
                    }
                    else
                    {
                        //CPU_Reg_r5
                        return;
                    }
                }
            }
        }
        else
        {
            if (id <= CPU_Reg_f2)
            {
                if (id <= CPU_Reg_f0)
                {
                    if (id <= CPU_Reg_r7)
                    {
                        if (id <= CPU_Reg_r6)
                        {
                            //CPU_Reg_r6
                            return;
                        }
                        else
                        {
                            //CPU_Reg_r7
                            return;
                        }
                    }
                    else
                    {
                        //CPU_Reg_f0
                        cpu->f0=data;
                    }
                }
                else
                {
                    if (id <= CPU_Reg_f1)
                    {
                        //CPU_Reg_f1
                        cpu->f1=data;
                    }
                    else
                    {
                        //CPU_Reg_f2
                        cpu->f2=data;
                    }
                }
            }
            else
            {
                if (id <= CPU_Reg_f5)
                {
                    if (id <= CPU_Reg_f4)
                    {
                        if (id <= CPU_Reg_f3)
                        {
                            //CPU_Reg_f3
                            cpu->f3=data;
                        }
                        else
                        {
                            //CPU_Reg_f4
                            cpu->f4=data;
                        }
                    }
                    else
                    {
                        //CPU_Reg_f5
                        cpu->f5=data;
                    }
                }
                else
                {
                    if (id <= CPU_Reg_f6)
                    {
                        //CPU_Reg_f6
                        cpu->f6=data;
                    }
                    else
                    {
                        //CPU_Reg_f7
                        cpu->f7=data;
                    }
                }
            }
        }
    }
}
