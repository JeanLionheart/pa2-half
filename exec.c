#include "../local-include/decode.h"
// #include"/home/wang/ics2020/nemu/include/debug.h"
#include "all-instr.h"
#include <cpu/exec.h>
#include <string.h>
#include <unistd.h>
static inline void set_width(DecodeExecState *s, int width)
{
    if (width == -1)
        return;
    if (width == 0)
    {
        width = s->isa.is_operand_size_16 ? 2 : 4;
    }
    // printf("operand width is %d\n",width);
    s->src1.width = s->dest.width = s->src2.width = width;
}

/* 0x80, 0x81, 0x83 */
static inline def_EHelper(gp1)
{
    switch (s->isa.ext_opcode)
    {
        EXW(0x0, add, -1)
        EXW(1, or, -1)
        EXW(0x2, adc, -1)
        EXW(0x3, sbb, -1)
        EXW(0x4, and, -1)
        EXW(0x5, sub, -1)
        EXW(0x6, xor, -1)
        EXW(0x7, cmp, -1)
        // exec_and
    }
}

/* 0xc0, 0xc1, 0xd0, 0xd1, 0xd2, 0xd3 */
static inline def_EHelper(gp2)
{
    switch (s->isa.ext_opcode)
    {
        EXW(4, shl, -1)
        EXW(5, shr, -1)
        EXW(7, sar, -1) // sar cv,need debug shr
    default:
        printf("\nEMPTY GROUP2, %d\n", s->isa.ext_opcode);
        // assert(0);
        exec_inv(s);
    }
}

/* 0xf6, 0xf7 */
static inline def_EHelper(gp3)
{
    switch (s->isa.ext_opcode)
    {
        IDEXW(0x0, test_I, test, -1)
        EXW(0x2, not, -1) // not
        EXW(0x3, neg, -1)
        EXW(0x4, mul, -1)   // mul
        EXW(0x5, imul1, -1) // imul
        EXW(7, idiv, -1)
    default:
        printf("\nEMPTY GROUP3, %d\n", s->isa.ext_opcode);
        // assert(0);
        exec_inv(s);
    }
}

/* 0xfe */
static inline def_EHelper(gp4)
{
    switch (s->isa.ext_opcode)
    {
    default:
        printf("\nEMPTY GROUP4, %d\n", s->isa.ext_opcode);
        // assert(0);
        exec_inv(s);
        // exec_test
    }
}

/* 0xff */
static inline def_EHelper(gp5) // 0xff 15 bc 01
{
    switch (s->isa.ext_opcode)
    {
        EX(0x0, inc)
        EX(0x2, call_rm)
        // exec_call_rm
        // decode_E2G
        EX(0x4, jmp_rm) // JMP Ev
        EXW(0x6, push, -1)
    default:
        printf("\nEMPTY GROUP5, %d\n", s->isa.ext_opcode);
        exec_inv(s);
        // assert(0);
    }
}

/* 0x0f 0x01*/
static inline def_EHelper(gp7)
{
    switch (s->isa.ext_opcode)
    {
    default:
        printf("\nEMPTY GROUP7, %d\n", s->isa.ext_opcode);
        // assert(0);
        exec_inv(s);
    }
}

static inline def_EHelper(2byte_esc) // 0x
{
    uint8_t opcode = instr_fetch(&s->seq_pc, 1);
    s->opcode = opcode;
    switch (opcode)
    {
        /* TODO: Add more instructions!!! */
        IDEX(0x01, gp7_E, gp7)
        IDEX(0x84, J, jcc)
        IDEX(0x85, J, jcc)
        IDEX(0x8f, J, jcc)
        IDEX(0x8e, J, jcc)
        IDEXW(0x94, setcc_E, setcc, 1)
        IDEXW(0x95, setcc_E, setcc, 1)
        IDEXW(0x9f, setcc_E, setcc, 1)
        IDEX(0xaf, E2G, imul2) // new
        // decode_E2G
        // exec_imul2
        // exec_test
        IDEXW(0xb6, mov_E2G, mov, 1)   // width=?
        IDEXW(0xb7, mov_E2G, movzx, 2) // operand_size 16
        // decode_mov_E2G
        IDEXW(0xbe, mov_E2G, movsx, 1)
        IDEXW(0xbf, mov_E2G, movsx, 2) // operand_size 16
        // decode_mov_E2G
        // exec_mov
    default:
        exec_inv(s);
    }
}

int times_loop = 0, first_pc = 0, dish = 0;
char c_list[] = "abcdef";
void transform(int pc, char *res)
{
    memset(res, '\0', 200);
    int len = 0;
    while (pc)
    {
        int x = pc % 16;
        pc /= 16;
        int m_bool = (x >= 10);
        switch (m_bool)
        {
        case 1:
            res[len] = c_list[x - 10];
            len++;
            break;
        case 0:
            res[len] = x + '0';
            len++;
            break;
        default:
            break;
        }
    }
    for (int i = 0, j = len - 1; i < j; i++, j--)
    {
        char c;
        c = res[j];
        res[j] = res[i];
        res[i] = c;
    }
}
/*



 */
int times = 0;
void printpc(DecodeExecState *s)
{
    times_loop++;
    // exec_or
    if (times_loop == 1)
    {
        first_pc = s->seq_pc;
        // if(s->opcode==0xbd){
        //     printf("no!");
        //     // _exit(0);
        // }
    }
    times_loop++;
    int true_pc;
    true_pc = s->seq_pc - first_pc;

    if (true_pc == 0x6d)
    {
        // times++;
        isa_reg_display();
        // exec_test
        char pc[200];
        transform(true_pc, pc);
        printf("pc: %s\n", pc);
        // printf("loop times is %d\n", times_loop);
        printf("cpu.eflags.ZF is %d\n", cpu.eflags.ZF);
        printf("cpu.eflags.SF is %d\n", cpu.eflags.SF);
        printf("cpu.eflags.OF is %d\n", cpu.eflags.OF);
        // printf("s0 is: %d\n", *s0);

        // uint32_t addr = 0x1001b0;
        // int paddr = 0;
        // rtl_lm(s, (uint32_t *)&paddr, &addr, 0, 4);
        // printf("Mem here is: %d\n", paddr);
        // if (times == 1)
        _exit(1);
    }
    // if(true_pc == 0x37){
    //     isa_reg_display();
    //     // _exit(1);
    //     sleep(1);
    // }
}
// killlist:addlong
static inline void fetch_decode_exec(DecodeExecState *s)
{
    uint8_t opcode;
again:
    // s.pc == cpu.pc now
    // printpc(s);//has done

    opcode = instr_fetch(&s->seq_pc,
                         1);
    s->opcode = opcode;
    // printpc(s); // about to
    // rtl_push
    switch (opcode)
    {
        IDEX(0x01, G2E, add) // bug
        // exec_add
        // exec_test
        IDEX(0x03, E2G, add)
        IDEX(0x09, G2E, or)
        IDEXW(0x0a, E2G, or, 1)
        EX(0x0f, 2byte_esc)//
        IDEX(0x11, G2E, adc)
        IDEX(0x13, E2G, adc)
        IDEX(0x1b, E2G, sbb)
        IDEX(0x29, G2E, sub)
        IDEX(0x2b, E2G, sub) // sub
        IDEXW(0x30, G2E, xor, 1)
        IDEX(0x31, G2E, xor)
        IDEX(0x33, E2G, xor)
        IDEX(0x35, I2a, xor)
        IDEXW(0x38, G2E, cmp, 1)
        IDEX(0x39, G2E, cmp)
        // decode_G2E
        IDEX(0x3b, E2G, cmp)
        IDEXW(0x3c, I2a, cmp, 1)
        IDEX(0x3d, I2a, cmp)
        IDEX(0x40, r, inc)
        IDEX(0x41, r, inc)
        IDEX(0x42, r, inc)
        IDEX(0x43, r, inc)
        IDEX(0x46, r, inc)
        IDEX(0x47, r, inc)
        IDEX(0x48, r, dec)
        IDEX(0x49, r, dec)
        IDEX(0x4a, r, dec)
        IDEX(0x4b, r, dec)
        IDEX(0x50, r, push)
        IDEX(0x51, r, push)
        IDEX(0x52, r, push)
        IDEX(0x53, r, push)
        IDEX(0x54, r, push)
        IDEX(0x55, r, push)
        IDEX(0x56, r, push)
        IDEX(0x57, r, push)
        IDEX(0x58, r, pop) // eax  or  ax
        IDEX(0x59, r, pop) //
        IDEX(0x5a, r, pop)
        // rtl_push
        IDEX(0x5b, r, pop)
        IDEX(0x5c, r, pop)
        IDEX(0x5d, r, pop)
        IDEX(0x5e, r, pop)
        IDEX(0x5f, r, pop)

        IDEX(0x68, push_SI, push) // pushl SI//debuging
        IDEXW(0x6a, push_SI, push, 1)
        IDEXW(0x74, J, jcc, 1) // width=?1
        IDEXW(0x75, J, jcc, 1)
        IDEXW(0x76, J, jcc, 1) // jbe,jna
        IDEXW(0x79, J, jcc, 1)
        IDEXW(0x7c, J, jcc, 1)
        IDEXW(0x7d, J, jcc, 1)
        IDEXW(0x7e, J, jcc, 1)
        IDEXW(0x7f, J, jcc, 1)
        IDEXW(0x80, I2E, gp1, 1) //

        IDEX(0x81, I2E, gp1) //
        IDEX(0x83, SI2E, gp1)
        IDEXW(0x84, G2E, test, 1)
        IDEX(0x85, G2E, test)
        // decode_G2E
        // exec_dec
        // exec_add
        IDEXW(0x88, mov_G2E, mov, 1)
        IDEX(0x89, mov_G2E, mov)
        IDEXW(0x8a, mov_E2G, mov, 1)
        IDEX(0x8b, mov_E2G, mov)
        IDEX(0x8d, lea_M2G, lea) // doing
        EX(0x90, xchg);
        EX(0x99, cltd)
        IDEXW(0xa0, O2a, mov, 1)
        IDEX(0xa1, O2a, mov) // move failed
        IDEXW(0xa2, a2O, mov, 1)
        IDEX(0xa3, a2O, mov)
        IDEXW(0xa8, I2a, test, 1)
        IDEXW(0xb0, mov_I2r, mov, 1)
        IDEXW(0xb1, mov_I2r, mov, 1)
        IDEXW(0xb2, mov_I2r, mov, 1)
        IDEXW(0xb3, mov_I2r, mov, 1)
        IDEXW(0xb4, mov_I2r, mov, 1)
        IDEXW(0xb5, mov_I2r, mov, 1)
        IDEXW(0xb6, mov_I2r, mov, 1)
        IDEXW(0xb7, mov_I2r, mov, 1)
        IDEX(0xb8, mov_I2r, mov) // exp
        IDEX(0xb9, mov_I2r, mov)
        IDEX(0xba, mov_I2r, mov)
        IDEX(0xbb, mov_I2r, mov)
        IDEX(0xbc, mov_I2r, mov)
        IDEX(0xbd, mov_I2r, mov)
        IDEX(0xbe, mov_I2r, mov)
        IDEX(0xbf, mov_I2r, mov)
        IDEXW(0xc0, gp2_Ib2E, gp2, 1)
        IDEX(0xc1, gp2_Ib2E, gp2)
        EX(0xc3, ret) // ret//debuging
        IDEXW(0xc6, mov_I2E, mov, 1)
        IDEX(0xc7, mov_I2E, mov)
        EX(0xc9, leave)
        // exec_leave
        IDEXW(0xd0, gp2_1_E, gp2, 1)
        IDEX(0xd1, gp2_1_E, gp2)
        IDEXW(0xd2, gp2_cl2E, gp2, 1)
        IDEX(0xd3, gp2_cl2E, gp2)
        EX(0xd6, nemu_trap)
        IDEX(0xe8, J, call) // call 4byte_addr//debuging
        IDEXW(0xeb, J, jmp, 1)
        IDEXW(0xf6, E, gp3, 1)
        IDEX(0xf7, E, gp3)
        IDEXW(0xfe, E, gp4, 1)
        IDEX(0xff, E, gp5) //
        // decode_E
    case 0x66 // movw
        :
        s->isa.is_operand_size_16 = true;
        goto again;
    default:
        exec_inv(s);
    };
}

vaddr_t isa_exec_once()
{
    DecodeExecState s;
    s.is_jmp = 0;
    s.isa = (ISADecodeInfo){
        0}; // the struct ISADecodeInfo is in nemu/include/isa/x86.h
    s.seq_pc = cpu.pc;

    fetch_decode_exec(&s);
    update_pc(&s);

    return s.seq_pc;
}
