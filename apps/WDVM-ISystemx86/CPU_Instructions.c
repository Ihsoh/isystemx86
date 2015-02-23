#include "CPU.h"
#include "linked_list.h"

#define CPU_Ins_nop 0
#define CPU_Ins_hlt 1
#define CPU_Ins_ldri 2
#define CPU_Ins_ldrf 3
#define CPU_Ins_stri 4
#define CPU_Ins_strf 5
#define CPU_Ins_popi 6
#define CPU_Ins_pushi 7
#define CPU_Ins_popf 8
#define CPU_Ins_pushf 9
#define CPU_Ins_itof 10
#define CPU_Ins_ftoi 11
#define CPU_Ins_and 12
#define CPU_Ins_or 13
#define CPU_Ins_xor 14
#define CPU_Ins_not 15
#define CPU_Ins_addi 16
#define CPU_Ins_subi 17
#define CPU_Ins_muli 18
#define CPU_Ins_divi 19
#define CPU_Ins_negi 20
#define CPU_Ins_inci 21
#define CPU_Ins_deci 22
#define CPU_Ins_addf 23
#define CPU_Ins_subf 24
#define CPU_Ins_mulf 25
#define CPU_Ins_divf 26
#define CPU_Ins_negf 27
#define CPU_Ins_incf 28
#define CPU_Ins_decf 29
#define CPU_Ins_gti 30
#define CPU_Ins_gtei 31
#define CPU_Ins_lti 32
#define CPU_Ins_ltei 33
#define CPU_Ins_eqi 34
#define CPU_Ins_neqi 35
#define CPU_Ins_gtf 36
#define CPU_Ins_gtef 37
#define CPU_Ins_ltf 38
#define CPU_Ins_ltef 39
#define CPU_Ins_eqf 40
#define CPU_Ins_neqf 41
#define CPU_Ins_shl 42
#define CPU_Ins_shr 43
#define CPU_Ins_jmp 44
#define CPU_Ins_jt 45
#define CPU_Ins_jf 46
#define CPU_Ins_call 47
#define CPU_Ins_ret 48
#define CPU_Ins_pushreg 49
#define CPU_Ins_popreg 50
#define CPU_Ins_randi 51
#define CPU_Ins_randf 52
#define CPU_Ins_cli 53
#define CPU_Ins_sei 54
#define CPU_Ins_int 55
#define CPU_Ins_jint 56
#define CPU_Ins_rdtsc 57

void CPU_executeInstruction(CPU * cpu, InstructionInfo * ii)
{
    unsigned short id = ii->ID;
    if (id <= CPU_Ins_incf)
    {
        if (id <= CPU_Ins_xor)
        {
            if (id <= CPU_Ins_pushi)
            {
                if (id <= CPU_Ins_ldrf)
                {
                    if (id <= CPU_Ins_hlt)
                    {
                        if (id <= CPU_Ins_nop)
                        {
                            //CPU_Ins_nop
                            //nop~
                        }
                        else
                        {
                            //CPU_Ins_hlt
                            CPU_halt(cpu);
                        }
                    }
                    else
                    {
                        if (id <= CPU_Ins_ldri)
                        {
                            //CPU_Ins_ldri
                            cpubasetype addr = CPU_popi(cpu);
                            if (addr >= cpu->memory->FloatOffset) //load float as int
                            {
                                CPU_pushi(cpu, *(cpubasetype *)(&cpu->memory->floatdata[addr - cpu->memory->FloatOffset]));
                            }
                            else
                            {
                                CPU_pushi(cpu, cpu->memory->intdata[addr]);
                            }
                        }
                        else
                        {
                            //CPU_Ins_ldrf
                            cpubasetype addr = CPU_popi(cpu);
                            if (addr < cpu->memory->FloatOffset) //load int as float
                            {
                                CPU_pushf(cpu, *(double *)(&cpu->memory->intdata[addr]));
                            }
                            else
                            {
                                CPU_pushf(cpu, cpu->memory->floatdata[addr - cpu->memory->FloatOffset]);
                            }
                        }
                    }
                }
                else
                {
                    if (id <= CPU_Ins_strf)
                    {
                        if (id <= CPU_Ins_stri)
                        {
                            //CPU_Ins_stri
                            cpubasetype data = CPU_popi(cpu);
                            cpubasetype addr = CPU_popi(cpu);
                            if (addr >= cpu->memory->FloatOffset) //set int as float
                            {
                                
                                cpu->memory->floatdata[addr - cpu->memory->FloatOffset] = *(double *)(&data);
                            }
                            else
                            {
                                cpu->memory->intdata[addr]=data;
                            }
                        }
                        else
                        {
                            //CPU_Ins_strf
                            double data = CPU_popf(cpu);
                            cpubasetype addr = CPU_popi(cpu);
                            if (addr < cpu->memory->FloatOffset) //set float as int
                            {
                                
                                cpu->memory->intdata[addr] = *(cpubasetype *)(&data);
                            }
                            else
                            {
                                cpu->memory->floatdata[addr - cpu->memory->FloatOffset] = data;
                            }
                        }
                    }
                    else
                    {
                        if (id <= CPU_Ins_popi)
                        {
                            //CPU_Ins_popi
                            if (dsl_lnklst_count(ii->pi) > 0)
                            {
                            	DSLLinkedListNode * node = dsl_lnklst_get(ii->pi, 0);
                                CPU_setIntRegisterValue(cpu, ((ParameterInfo *)(node->value.value.object_value))->value, CPU_popi(cpu));
                            }
                            else
                            {
                                CPU_popi(cpu);
                            }
                        }
                        else
                        {
                            //CPU_Ins_pushi
                            DSLLinkedListNode * node = dsl_lnklst_get(ii->pi, 0);
                            CPU_pushi(cpu, CPU_getIntValue(cpu, node->value.value.object_value));
                        }
                    }
                }
            }
            else
            {
                if (id <= CPU_Ins_ftoi)
                {
                    if (id <= CPU_Ins_pushf)
                    {
                        if (id <= CPU_Ins_popf)
                        {
                            //CPU_Ins_popf
                            if (dsl_lnklst_count(ii->pi) > 0)
                            {
                                DSLLinkedListNode * node = dsl_lnklst_get(ii->pi, 0);
                                CPU_setFloatRegisterValue(cpu, ((ParameterInfo *)(node->value.value.object_value))->value, CPU_popf(cpu));
                            }
                            else
                            {
                                CPU_popf(cpu);
                            }
                        }
                        else
                        {
                            //CPU_Ins_pushf
                            DSLLinkedListNode * node = dsl_lnklst_get(ii->pi, 0);
                            CPU_pushf(cpu, CPU_getFloatValue(cpu, node->value.value.object_value));
                        }
                    }
                    else
                    {
                        if (id <= CPU_Ins_itof)
                        {
                            //CPU_Ins_itof
                            CPU_pushf(cpu, (double)CPU_popi(cpu));
                        }
                        else
                        {
                            //CPU_Ins_ftoi
                            CPU_pushi(cpu, (cpubasetype)CPU_popf(cpu));
                        }
                    }
                }
                else
                {
                    if (id <= CPU_Ins_or)
                    {
                        if (id <= CPU_Ins_and)
                        {
                            //CPU_Ins_and
                            cpubasetype a, b;
                            a = CPU_popi(cpu);
                            b = CPU_popi(cpu);
                            CPU_pushi(cpu, a&b);
                        }
                        else
                        {
                            //CPU_Ins_or
                            cpubasetype a, b;
                            a = CPU_popi(cpu);
                            b = CPU_popi(cpu);
                            CPU_pushi(cpu, a|b);
                        }
                    }
                    else
                    {
                        //CPU_Ins_xor
                        cpubasetype a, b;
                        a = CPU_popi(cpu);
                        b = CPU_popi(cpu);
                        CPU_pushi(cpu, a^b);
                    }
                }
            }
        }
        else
        {
            if (id <= CPU_Ins_inci)
            {
                if (id <= CPU_Ins_muli)
                {
                    if (id <= CPU_Ins_addi)
                    {
                        if (id <= CPU_Ins_not)
                        {
                            //CPU_Ins_not
                            CPU_pushi(cpu, ~CPU_popi(cpu));
                        }
                        else
                        {
                            //CPU_Ins_addi
                            cpubasetype a, b;
                            a = CPU_popi(cpu);
                            b = CPU_popi(cpu);
                            CPU_pushi(cpu, a+b);
                        }
                    }
                    else
                    {
                        if (id <= CPU_Ins_subi)
                        {
                            //CPU_Ins_subi
                            cpubasetype a, b;
                            a = CPU_popi(cpu);
                            b = CPU_popi(cpu);
                            CPU_pushi(cpu, b-a);
                        }
                        else
                        {
                            //CPU_Ins_muli
                            cpubasetype a, b;
                            a = CPU_popi(cpu);
                            b = CPU_popi(cpu);
                            CPU_pushi(cpu, a*b);
                        }
                    }
                }
                else
                {
                    if (id <= CPU_Ins_negi)
                    {
                        if (id <= CPU_Ins_divi)
                        {
                            //CPU_Ins_divi
                            cpubasetype a, b;
                            a = CPU_popi(cpu);
                            b = CPU_popi(cpu);
                            if (a==0) //a==0
                            {
                                CPU_pushi(cpu, 0);
                                CPU_pushi(cpu, 0);
                                CPU_Interrupt(cpu, 0); //div
                            }
                            else
                            {
                            	int b_32 = (int)b;
                            	int a_32 = (int)a;
                                CPU_pushi(cpu, b_32 % a_32);
                                CPU_pushi(cpu, b_32 / a_32);
                            }
                        }
                        else
                        {
                            //CPU_Ins_negi
                            CPU_pushi(cpu, -CPU_popi(cpu));
                        }
                    }
                    else
                    {
                        //CPU_Ins_inci
                        CPU_pushi(cpu, CPU_popi(cpu)+1);
                    }
                }
            }
            else
            {
                if (id <= CPU_Ins_mulf)
                {
                    if (id <= CPU_Ins_addf)
                    {
                        if (id <= CPU_Ins_deci)
                        {
                            //CPU_Ins_deci
                            CPU_pushi(cpu, CPU_popi(cpu)-1);
                        }
                        else
                        {
                            //CPU_Ins_addf
                            double a, b;
                            a = CPU_popf(cpu);
                            b = CPU_popf(cpu);
                            CPU_pushf(cpu, a+b);
                        }
                    }
                    else
                    {
                        if (id <= CPU_Ins_subf)
                        {
                            //CPU_Ins_subf
                            double a, b;
                            a = CPU_popf(cpu);
                            b = CPU_popf(cpu);
                            CPU_pushf(cpu, b-a);
                        }
                        else
                        {
                            //CPU_Ins_mulf
                            double a, b;
                            a = CPU_popf(cpu);
                            b = CPU_popf(cpu);
                            CPU_pushf(cpu, a*b);
                        }
                    }
                }
                else
                {
                    if (id <= CPU_Ins_negf)
                    {
                        if (id <= CPU_Ins_divf)
                        {
                            //CPU_Ins_divf
                            double a, b;
                            a = CPU_popf(cpu);
                            b = CPU_popf(cpu);
                            CPU_pushf(cpu, b/a);
                        }
                        else
                        {
                            //CPU_Ins_negf
                            CPU_pushf(cpu, -CPU_popf(cpu));
                        }
                    }
                    else
                    {
                        //CPU_Ins_incf
                        CPU_pushf(cpu, CPU_popf(cpu) + 1);
                    }
                }
            }
        }
    }
    else
    {
        if (id <= CPU_Ins_shr)
        {
            if (id <= CPU_Ins_gtf)
            {
                if (id <= CPU_Ins_lti)
                {
                    if (id <= CPU_Ins_gti)
                    {
                        if (id <= CPU_Ins_decf)
                        {
                            //CPU_Ins_decf
                            CPU_pushf(cpu, CPU_popf(cpu) - 1);
                        }
                        else
                        {
                            //CPU_Ins_gti
                            cpubasetype a, b;
                            a = CPU_popi(cpu);
                            b = CPU_popi(cpu);
                            CPU_pushi(cpu, b > a);
                        }
                    }
                    else
                    {
                        if (id <= CPU_Ins_gtei)
                        {
                            //CPU_Ins_gtei
                            cpubasetype a, b;
                            a = CPU_popi(cpu);
                            b = CPU_popi(cpu);
                            CPU_pushi(cpu, b >= a);
                        }
                        else
                        {
                            //CPU_Ins_lti
                            cpubasetype a, b;
                            a = CPU_popi(cpu);
                            b = CPU_popi(cpu);
                            CPU_pushi(cpu, b < a);
                        }
                    }
                }
                else
                {
                    if (id <= CPU_Ins_eqi)
                    {
                        if (id <= CPU_Ins_ltei)
                        {
                            //CPU_Ins_ltei
                            cpubasetype a, b;
                            a = CPU_popi(cpu);
                            b = CPU_popi(cpu);
                            CPU_pushi(cpu, b <= a);
                        }
                        else
                        {
                            //CPU_Ins_eqi
                            cpubasetype a, b;
                            a = CPU_popi(cpu);
                            b = CPU_popi(cpu);
                            CPU_pushi(cpu, b == a);
                        }
                    }
                    else
                    {
                        if (id <= CPU_Ins_neqi)
                        {
                            //CPU_Ins_neqi
                            cpubasetype a, b;
                            a = CPU_popi(cpu);
                            b = CPU_popi(cpu);
                            CPU_pushi(cpu, b != a);
                        }
                        else
                        {
                            //CPU_Ins_gtf
                            double a, b;
                            a = CPU_popf(cpu);
                            b = CPU_popf(cpu);
                            CPU_pushi(cpu, b > a);
                        }
                    }
                }
            }
            else
            {
                if (id <= CPU_Ins_eqf)
                {
                    if (id <= CPU_Ins_ltf)
                    {
                        if (id <= CPU_Ins_gtef)
                        {
                            //CPU_Ins_gtef
                            double a, b;
                            a = CPU_popf(cpu);
                            b = CPU_popf(cpu);
                            CPU_pushi(cpu, b >= a);
                        }
                        else
                        {
                            //CPU_Ins_ltf
                            double a, b;
                            a = CPU_popf(cpu);
                            b = CPU_popf(cpu);
                            CPU_pushi(cpu, b < a);
                        }
                    }
                    else
                    {
                        if (id <= CPU_Ins_ltef)
                        {
                            //CPU_Ins_ltef
                            double a, b;
                            a = CPU_popf(cpu);
                            b = CPU_popf(cpu);
                            CPU_pushi(cpu, b <= a);
                        }
                        else
                        {
                            //CPU_Ins_eqf
                            double a, b;
                            a = CPU_popf(cpu);
                            b = CPU_popf(cpu);
                            CPU_pushi(cpu, b == a);
                        }
                    }
                }
                else
                {
                    if (id <= CPU_Ins_shl)
                    {
                        if (id <= CPU_Ins_neqf)
                        {
                            //CPU_Ins_neqf
                            double a, b;
                            a = CPU_popf(cpu);
                            b = CPU_popf(cpu);
                            CPU_pushi(cpu, b != a);
                        }
                        else
                        {
                            //CPU_Ins_shl
                            cpubasetype a, b;
                            a = CPU_popi(cpu);
                            b = CPU_popi(cpu);
                            CPU_pushi(cpu, b << a);
                        }
                    }
                    else
                    {
                        //CPU_Ins_shr
                        cpubasetype a, b;
                        a = CPU_popi(cpu);
                        b = CPU_popi(cpu);
                        CPU_pushi(cpu, b >> a);
                    }
                }
            }
        }
        else
        {
            if (id <= CPU_Ins_popreg)
            {
                if (id <= CPU_Ins_call)
                {
                    if (id <= CPU_Ins_jt)
                    {
                        if (id <= CPU_Ins_jmp)
                        {
                            //CPU_Ins_jmp
                            DSLLinkedListNode * node = dsl_lnklst_get(ii->pi, 0);
                            cpu->eip = CPU_getIntValue(cpu, node->value.value.object_value);
                        }
                        else
                        {
                            //CPU_Ins_jt
                            if (CPU_popi(cpu)&1)
                        	{
                        		DSLLinkedListNode * node = dsl_lnklst_get(ii->pi, 0);
                        		cpu->eip = CPU_getIntValue(cpu, node->value.value.object_value);
                        	}
                        }
                    }
                    else
                    {
                        if (id <= CPU_Ins_jf)
                        {
                            //CPU_Ins_jf
                            if (!(CPU_popi(cpu)&1))
                        	{
                        		DSLLinkedListNode * node = dsl_lnklst_get(ii->pi, 0);
                        		cpu->eip = CPU_getIntValue(cpu, node->value.value.object_value);
                        	}
                        }
                        else
                        {
                            //CPU_Ins_call
                            CPU_pushi(cpu, cpu->eip);
                            DSLLinkedListNode * node = dsl_lnklst_get(ii->pi, 0);
                            cpu->eip = CPU_getIntValue(cpu, node->value.value.object_value);
                        }
                    }
                }
                else
                {
                    if (id <= CPU_Ins_pushreg)
                    {
                        if (id <= CPU_Ins_ret)
                        {
                            //CPU_Ins_ret
                            cpu->eip = CPU_popi(cpu);
                        }
                        else
                        {
                            //CPU_Ins_pushreg
                            CPU_pushreg(cpu);
                        }
                    }
                    else
                    {
                        //CPU_Ins_popreg
                        CPU_popreg(cpu);
                    }
                }
            }
            else
            {
                if (id <= CPU_Ins_sei)
                {
                    if (id <= CPU_Ins_randf)
                    {
                        if (id <= CPU_Ins_randi)
                        {
                            //CPU_Ins_randi
                            CPU_pushi(cpu, 0); //TODO
                        }
                        else
                        {
                            //CPU_Ins_randf
                            CPU_pushf(cpu, 0); //TODO
                        }
                    }
                    else
                    {
                        if (id <= CPU_Ins_cli)
                        {
                            //CPU_Ins_cli
                            cpu->InterruptEnable = FALSE;
                        }
                        else
                        {
                            //CPU_Ins_sei
                            cpu->InterruptEnable = TRUE;
                        }
                    }
                }
                else
                {
                    if (id <= CPU_Ins_jint)
                    {
                        if (id <= CPU_Ins_int)
                        {
                            //CPU_Ins_int
                            DSLLinkedListNode * node = dsl_lnklst_get(ii->pi, 0);
                            CPU_Interrupt(cpu, CPU_getIntValue(cpu, node->value.value.object_value));
                        }
                        else
                        {
                            //CPU_Ins_jint
                            DSLLinkedListNode * node = dsl_lnklst_get(ii->pi, 0);
                            cpubasetype id = CPU_popi(cpu);
                            cpu->InterruptVector[id].type = Interrupt_Inner;
                            cpu->InterruptVector[id].inner_address = CPU_getIntValue(cpu, node->value.value.object_value);
                        }
                    }
                    else
                    {
                        //CPU_Ins_rdtsc
                        CPU_pushi(cpu, cpu->RanCount);
                    }
                }
            }
        }
    }
}
