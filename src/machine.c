#include "machine.h"

processor_status_t mov(machine* m, uint8_t value, processor_reg_t reg) {
    if(reg == RA) { m->a = value; }
    if(reg == RB) { m->b = value; }
    if(reg == RC) { m->c = value; }

    m->la = value;
    m->status = CONTINUE;
    return CONTINUE;
}

processor_status_t cmov(machine* m, char value, processor_reg_t reg) {
    m->ch = value;
    m->la = value;
    m->status = CONTINUE;
    return CONTINUE;
}

uint8_t push(machine* m, uint8_t value) {
    m->lp = value;
    //printf("push %d\n", m->lp);
    return m->lp;
} 

processor_status_t ret(machine* m) {
    m->status = RET;
    return RET;
}

uint8_t pop(machine* m) {
    m->lp = 0;
    //printf("pop\n", m->lp);
    return m->lp;
}

machine* init_machine(program_t prg) {
    machine* m = malloc(sizeof(machine));
        
    // clear out the stack
    for(int i = 0; i < 256; i++) {
        m->stack[i] = 0;
    }

    m->status = CONTINUE;

    m->a = 0;

    m->b = 0;

    m->c = 0;

    m->la = 0;

    m->lp = 0;

    m->prg = prg;

    return m;
}

char* timeandate() {
    time_t timer;
    char *buffer = malloc(sizeof(char)*100);
    struct tm* tm_info;

    timer = time(NULL);
    tm_info = localtime(&timer);

    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    return buffer;
}

void program_run(machine* m) {
    while(m->status != RET) {
        m->prg(m);
    }
}

processor_status_t jmp(machine* m, program_t prog) {
    m->prg = prog;
    program_run(m);
    //printf("jmp #%d\n", m->prg);
    m->status = CONTINUE;
    return CONTINUE;
}

processor_status_t jnz(machine* m, program_t prog) {
    if(m->lp != 0) {
        //printf("jnz\0");
        return jmp(m, prog);
    }
}

void parse(machine* m, char* bufin) {
    printf("Compiling started at %s...\n", timeandate());
    FILE* f = fopen(bufin, "r");
    char* buf = malloc(sizeof(char)*PROGRAM_MAX_CHAR-100);

    m->status = CONTINUE;

    char* allchars = malloc(sizeof(char)*256);

    for(int o = 0; o < 255; o++) {
        allchars[o] = (unsigned char)o;
    }

    size_t len = 0;
    ssize_t read;

    char (*bf2)[PROGRAM_MAX_CHAR] = malloc(PROGRAM_MAX_CHAR*sizeof(*bf2));

    int i = 0;

    while (fgets(buf, PROGRAM_MAX_CHAR, f)) {
        strcpy(bf2[i], buf);
        i++;
    }

    bool isok = false;

    int j = 0;

    char (*bak)[PROGRAM_MAX_CHAR] = malloc(PROGRAM_MAX_CHAR*sizeof(*bf2));
    for(int f = 0; f < i; f++) {
        strcpy(bak[f], bf2[f]);
    }

    printf("Compiling ended at %s...\n", timeandate());
    printf("Start program...\n");

    while(m->status != RET) {
        for(int f = 0; f < i; f++) {
            strcpy(bf2[f], bak[f]);
        }
        int res = 0;
        if(!strstr(bf2[j], ";")) {
            if(strstr(bf2[j], "main:")) {
                isok = true;
                j++;
                continue;
            }
            if(strstr(bf2[j], "mov")) {
                if(strstr(bf2[j], "ra,")) {
                    char *bufof = strstr(bf2[j], ",");
                    bufof[0] = '0';
                    if(strstr(bufof, "rb")) { mov(m, m->b, RA); }
                    else if(strstr(bufof, "rc")) { mov(m, m->c, RA); }
                    else {res = strtol(bufof, NULL, 10); mov(m, res, RA); }
                }
                else if(strstr(bf2[j], "rb,")) {
                    char *bufof = strstr(bf2[j], ",");
                    bufof[0] = '0';
                    if(strstr(bufof, "ra")) { mov(m, m->a, RB); }
                    else if(strstr(bufof, "rc")) { mov(m, m->c, RB); }
                    else {res = strtol(bufof, NULL, 10); mov(m, res, RB); }
                }
                else if(strstr(bf2[j], "rc,")) {
                    char *bufof = strstr(bf2[j], ",");
                    bufof[0] = '0';
                    if(strstr(bufof, "rb")) { mov(m, m->b, RC); }
                    else if(strstr(bufof, "ra")) { mov(m, m->a, RC); }
                    else {res = strtol(bufof, NULL, 10); mov(m, res, RC); }
                }
                else if(strstr(bf2[j], "rch,")) {
                    char *bufof = strstr(bf2[j], ",");
                    memmove(&bufof[0], &bufof[0 + 1], strlen(bufof) - 0);
                    cmov(m, bufof[0], RC);
                }
                j++;
            }
            else if(strstr(bf2[j], "push")) {
                char *bufof = strstr(bf2[j], "h");
                bufof[0] = '0';
                bufof[1] = '0';
                if(strstr(bufof, "00ra")) {
                    push(m, m->a);
                    j++;
                    continue;
                } else if(strstr(bufof, "00rb")) {
                    push(m, m->b);
                    j++;
                    continue;
                } else if(strstr(bufof, "00rc")) {
                    push(m, m->c);
                    j++;
                    continue;
                }
                res = strtol(bufof, NULL, 10);
                push(m, res);
                j++;
            }
            else if(strstr(bf2[j], "pop")) {
                pop(m);
                j++;
            }
            else if(strstr(bf2[j], "out")) {
                char *bufof = strstr(bf2[j], "out");
                bufof[0] = '0';
                bufof[1] = '0';
                bufof[2] = '0';
                bufof[3] = '0';
                if(strstr(bufof, "0000ra")) {
                    printf("%d", m->a);
                } else if(strstr(bufof, "0000rb")) {
                    printf("%d", m->b);   
                } else if(strstr(bufof, "0000rc")) {
                    printf("%d", m->c);   
                } else {
                    res = strtol(bufof, NULL, 10);
                    printf("%d", res);
                }
                j++;
            } else if(strstr(bf2[j], "ouc")) {
                printf("%c", m->ch);
                j++;
            } else if(strstr(bf2[j], "opsh")) {
                printf("%d", m->lp);
                j++;
            } else if(strstr(bf2[j], "ret")) {
                ret(m);
                j++;
            } else if(strstr(bf2[j], "jmp")) {
                char *bufof = strstr(bf2[j], "jmp");
                bufof[0] = '0';
                bufof[1] = '0';
                bufof[2] = '0';
                bufof[3] = '0';
                res = strtol(bufof, NULL, 10);
                j = res-1;
                continue;
            } else if(strstr(bf2[j], "jnz")) {
                if(m->lp != 0) {
                    char *bufof = strstr(bf2[j], "jnz");
                    bufof[0] = '0';
                    bufof[1] = '0';
                    bufof[2] = '0';
                    bufof[3] = '0';
                    res = strtol(bufof, NULL, 10);
                    j = res-1;
                    continue;
                } else {
                    j++;
                }
            } else if(strstr(bf2[j], "jz")) {
                if(m->lp == 0) {
                    char *bufof = strstr(bf2[j], "jz");
                    bufof[0] = '0';
                    bufof[1] = '0';
                    bufof[2] = '0';
                    res = strtol(bufof, NULL, 10);
                    j = res-1;
                    continue;
                } else {
                    j++;
                }
            } else if(strstr(bf2[j], "add")) {
                if(strstr(bf2[j], "ra,")) {
                    char *bufof = strstr(bf2[j], ",");
                    bufof[0] = '0';
                    if(strstr(bufof, "rc")) { m->a+=m->c; }
                    else if(strstr(bufof, "rb")) { m->a+=m->b; }
                    else { res = strtol(bufof, NULL, 10); m->a+=res; }
                }
                else if(strstr(bf2[j], "rb,")) {
                    char *bufof = strstr(bf2[j], ",");
                    bufof[0] = '0';
                    if(strstr(bufof, "rc")) { m->b+=m->c; }
                    else if(strstr(bufof, "ra")) { m->b+=m->a; }
                    else { res = strtol(bufof, NULL, 10); m->b+=res; }
                }
                else if(strstr(bf2[j], "rc,")) {
                    char *bufof = strstr(bf2[j], ",");
                    bufof[0] = '0';
                    if(strstr(bufof, "ra")) { m->c+=m->a; }
                    else if(strstr(bufof, "rb")) { m->c+=m->b; }
                    else { res = strtol(bufof, NULL, 10); m->c+=res; }
                }
                j++;
            } else if(strstr(bf2[j], "sub")) {
                if(strstr(bf2[j], "ra,")) {
                    char *bufof = strstr(bf2[j], ",");
                    bufof[0] = '0';
                    if(strstr(bufof, "rc")) { m->a-=m->c; }
                    else if(strstr(bufof, "rb")) { m->a-=m->b; }
                    else { res = strtol(bufof, NULL, 10); m->a-=res; }
                }
                else if(strstr(bf2[j], "rb,")) {
                    char *bufof = strstr(bf2[j], ",");
                    bufof[0] = '0';
                    if(strstr(bufof, "rc")) { m->b-=m->c; }
                    else if(strstr(bufof, "ra")) { m->b-=m->a; }
                    else { res = strtol(bufof, NULL, 10); m->b-=res; }
                }
                else if(strstr(bf2[j], "rc,")) {
                    char *bufof = strstr(bf2[j], ",");
                    bufof[0] = '0';
                    if(strstr(bufof, "ra")) { m->c-=m->a; }
                    else if(strstr(bufof, "rb")) { m->c-=m->b; }
                    else { res = strtol(bufof, NULL, 10); m->c-=res; }
                }
                j++;
            } else if(strstr(bf2[j], "mul")) {
                if(strstr(bf2[j], "ra,")) {
                    char *bufof = strstr(bf2[j], ",");
                    bufof[0] = '0';
                    if(strstr(bufof, "rc")) { m->a*=m->c; }
                    else if(strstr(bufof, "rb")) { m->a*=m->b; }
                    else { res = strtol(bufof, NULL, 10); m->a*=res; }
                }
                else if(strstr(bf2[j], "rb,")) {
                    char *bufof = strstr(bf2[j], ",");
                    bufof[0] = '0';
                    if(strstr(bufof, "rc")) { m->b*=m->c; }
                    else if(strstr(bufof, "ra")) { m->b*=m->a; }
                    else { res = strtol(bufof, NULL, 10); m->b*=res; }
                }
                else if(strstr(bf2[j], "rc,")) {
                    char *bufof = strstr(bf2[j], ",");
                    bufof[0] = '0';
                    if(strstr(bufof, "ra")) { m->c*=m->a; }
                    else if(strstr(bufof, "rb")) { m->c*=m->b; }
                    else { res = strtol(bufof, NULL, 10); m->c*=res; }
                }
                j++;
            } else if(strstr(bf2[j], "div")) {
                if(strstr(bf2[j], "ra,")) {
                    char *bufof = strstr(bf2[j], ",");
                    bufof[0] = '0';
                    if(strstr(bufof, "rc")) { m->a/=m->c; }
                    else if(strstr(bufof, "rb")) { m->a/=m->b; }
                    else { res = strtol(bufof, NULL, 10); m->a/=res; }
                }
                else if(strstr(bf2[j], "rb,")) {
                    char *bufof = strstr(bf2[j], ",");
                    bufof[0] = '0';
                    if(strstr(bufof, "rc")) { m->b/=m->c; }
                    else if(strstr(bufof, "ra")) { m->b/=m->a; }
                    else { res = strtol(bufof, NULL, 10); m->b/=res; }
                }
                else if(strstr(bf2[j], "rc,")) {
                    char *bufof = strstr(bf2[j], ",");
                    bufof[0] = '0';
                    if(strstr(bufof, "ra")) { m->c/=m->a; }
                    else if(strstr(bufof, "rb")) { m->c/=m->b; }
                    else { res = strtol(bufof, NULL, 10); m->c/=res; }
                }
                j++;
            } else if(strstr(bf2[j], "mod")) {
                if(strstr(bf2[j], "ra,")) {
                    char *bufof = strstr(bf2[j], ",");
                    bufof[0] = '0';
                    if(strstr(bufof, "rc")) { m->a%=m->c; }
                    else if(strstr(bufof, "rb")) { m->a%=m->b; }
                    else { res = strtol(bufof, NULL, 10); m->a%=res; }
                }
                else if(strstr(bf2[j], "rb,")) {
                    char *bufof = strstr(bf2[j], ",");
                    bufof[0] = '0';
                    if(strstr(bufof, "rc")) { m->b%=m->c; }
                    else if(strstr(bufof, "ra")) { m->b%=m->a; }
                    else { res = strtol(bufof, NULL, 10); m->b%=res; }
                }
                else if(strstr(bf2[j], "rc,")) {
                    char *bufof = strstr(bf2[j], ",");
                    bufof[0] = '0';
                    if(strstr(bufof, "ra")) { m->c%=m->a; }
                    else if(strstr(bufof, "rb")) { m->c%=m->b; }
                    else { res = strtol(bufof, NULL, 10); m->c%=res; }
                }
                j++;
            } else if(strstr(bf2[j], "nop")) {
                asm("nop");
            } else if(strstr(bf2[j], "och")) {
                char *bufof = strstr(bf2[j], "och");
                bufof[0] = '0';
                bufof[1] = '0';
                bufof[2] = '0';
                bufof[3] = '0';
                if(bufof[4] == 'n' && bufof[5] == 'l') { printf("\n"); j++; continue; }
                else { printf("%c", bufof[4]); }
                j++;
            } else if(strstr(bf2[j], "arr")) {
                if(strstr(bf2[j], "ra")) {
                    m->ax[m->cnt] = m->a;
                }
                else if(strstr(bf2[j], "rb")) {
                    m->ax[m->cnt] = m->b;
                }
                else if(strstr(bf2[j], "rc")) {
                    m->ax[m->cnt] = m->c;                    
                } else {
                    char *bufof = strstr(bf2[j], "arr");
                    bufof[0] = '0';
                    bufof[1] = '0';
                    bufof[2] = '0';
                    bufof[3] = '0';
                    res = strtol(bufof, NULL, 10);
                    m->ax[m->cnt] = res;
                }
                m->cnt++;
                j++;
            } else if(strstr(bf2[j], "clr")) {
                for(int p = 0; p < m->cnt; p++) {
                    m->ax[p] = 0;
                }
                m->cnt = 0;
                j++;
            } else if(strstr(bf2[j], "oar")) {
                for(int p = m->cnt-1; p > -1; p--) {
                    printf("%d", m->ax[p]);
                }
                j++;
            } else {
                j++;
                continue;
            }
        } else {
            j++;
        }
        
    }

    printf("Program ended at %s...\n", timeandate());

    fclose(f);
}