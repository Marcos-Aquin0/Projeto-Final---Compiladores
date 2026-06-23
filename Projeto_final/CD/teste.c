
int processos [ 10 ]; 
int processoAtual; 
int processosCarregados; 
int programas [ 10 ];
int halts [ 10 ];
int qtdHalt;
int processosExecucao [ 10 ];
int salto;
int memdados;
int dados [ 10 ];
int opcao;
int sinalsyscall;
int rodouumavez [ 10 ];

void mapeamento(){
    int i;
    int desvio;
    int desviodados;
    
    i = 0;
    desviodados = 11000;
    desvio = 1901;
    while(i <= 6){
        nop();
        dados[i] = desviodados;
        halts[i] = 0;
        rodouumavez[i] = 0;
        programas[i] = i;
        processos[i] = desvio;
        desvio = desvio + 300;
        desviodados = desviodados - 1000;
        i = i+1;    
        nop();
    }
}

void limpaVar(){
    int i;
    i = 0;
    while(i <= 9){
        nop();
        halts[i] = 0;
        rodouumavez[i] = 0;
        processosExecucao[i] = 0;
        i = i+1;    
        nop();
    }
    qtdHalt = 0;
    sinalsyscall = 0;
    opcao = 0;
    processosCarregados = 0;
}

void menuShell(){
    msgLcd(26); 
    opcao = input();
}

void votenaoPreemptivo(){
    processoAtual = 6;
    nop();
    msgLcd(31); 
    salto = processos[processoAtual];
    memdados = dados[processoAtual];
    dispatchersavenp(); 
    dispatcherloadnp(); 
    
    nop();
    salto = 0;
    nop();
}

void naoPreemptivo(){
    msgLcd(17);
    processoAtual = input();
    processoAtual = processoAtual - 1;
    nop();
    nop();
    msgLcd(12);
    salto = processos[processoAtual];
    memdados = dados[processoAtual];
    dispatchersavenp(); 
    dispatcherloadnp(); 
    
    nop();
    salto = 0;
    nop();
}

void initDados(){
    int temp;
    int combustivel;
    int altura;

    temp = 25;
    combustivel = 255;
    altura = 0;

    saveword(2000, temp);
    saveword(2001, combustivel);
    saveword(2002, altura);
}

void initUART(){
    msgLcd(32);
    nop();
}

void receiveUART(){
    int pronto;
    int aux;
    int header;
    int flag; 
    int comando; 
    int valor; 
    int checksum; 
   
    msgLcd(33);
    
    aux = 1000;
    pronto = 0;

    while(pronto == 0){
        pronto = loadword(aux);
    }

    saveword(aux+1, 1);

    header = loadword(aux+2);
    output(header);
    
    flag = loadword(aux+3);
    output(flag);
    
    comando = loadword(aux+4);
    output(comando);
    
    valor = loadword(aux+5);
    output(valor);

    checksum = loadword(aux+6);
    output(checksum);

    saveword(500, header);
    saveword(501, flag);
    saveword(502, comando);
    saveword(503, valor);
    saveword(504, checksum);
    
    nop();
}

int checkFlags(int flag){
    int valorAns;

    if (flag == 1) {
        valorAns = loadword(2000); 
    }
    else if (flag == 2) {
        valorAns = loadword(2001); 
    }
    else if (flag == 3) {
        valorAns = loadword(2002); 
    }
    else {
        valorAns = 0;
        nop();
    }
    return valorAns;
}

void sendUART(){
    int comando;
    int flag;
    int valorRec;
    int valorAns;
    int header;
    int checksum;

    msgLcd(34); 

    flag = loadword(501);
    comando = loadword(502);
    valorRec = loadword(503);

    if (comando == 1) { 
        header = 124; 
        valorAns = valorRec;
    }
    else if (comando == 2) { 
        header = 124; 
        valorAns = valorRec;
    }
    else if (comando == 3) { 
        header = 124 + flag; 
        valorAns = checkFlags(flag);
    }
    else {
        nop();
    }

    saveword(1008, header);
    saveword(1009, comando);
    saveword(1010, valorAns);
    
    saveword(1011, 1);

    nop();
}

void main(void){
    mapeamento();
    msgLcd(0);
    nop();
    salto = 0;
    processosCarregados = 3;
    nop();
    nop();
    menuShell();
    nop();

    salto = 0;
    nop();

    while(opcao != 6){
        initDados();
        initUART();
        receiveUART();
        naoPreemptivo();
        nop();
        naoPreemptivo();
        nop();
        naoPreemptivo();
        nop();
        votenaoPreemptivo();
        sendUART();
        nop();
        salto = 0;
        menuShell();
    }
    nop();
    nop();
    nop();
    msgLcd(28);
    nop();
    nop();
    nop();
    nop();
    halt();
}