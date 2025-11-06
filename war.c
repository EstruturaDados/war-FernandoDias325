#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NUM_TERRITORIOS 6
#define NUM_MISSOES 5

// Estrutura de territorio
typedef struct {
    char nome[30];
    char cor[10];
    int tropas;
} Territorio;

// Prototipos
void atribuirMissao(char* destino, const char* missoes[], int total, const char* corJogador);
int verificarMissao(const char* missao, const char* corJogador, Territorio* mapa, int tamanho);
void exibirMissao(const char* missao);
void atacar(Territorio* atacante, Territorio* defensor);
void exibirMapa(Territorio* mapa, int tamanho);
void liberarMemoria(Territorio* mapa, char* missao1, char* missao2);
void limparBuffer();


int main() {
    srand(time(NULL));
    
    const char* COR_JOGADOR1 = "Azul";
    const char* COR_JOGADOR2 = "Vermelho";

    // vetor com as missoes possiveis
    const char* missoes[NUM_MISSOES] = {
        "Conquistar 4 territorios no total",
        "Eliminar todas as tropas do jogador Vermelho",
        "Eliminar todas as tropas do jogador Azul",
        "Conquistar o territorio 'Brasil'",
        "Ter 10 tropas em um unico territorio"
    };

    // aloca memoria para o mapa
    Territorio* mapa = (Territorio*) malloc(NUM_TERRITORIOS * sizeof(Territorio));
    if (mapa == NULL) {
        printf("ERRO: Falha ao alocar memoria!\n");
        return 1;
    }

    // inicializa os territorios
    strcpy(mapa[0].nome, "Brasil");
    strcpy(mapa[0].cor, "Azul");
    mapa[0].tropas = 5;
    
    strcpy(mapa[1].nome, "Argentina");
    strcpy(mapa[1].cor, "Azul");
    mapa[1].tropas = 3;
    
    strcpy(mapa[2].nome, "Peru");
    strcpy(mapa[2].cor, "Azul");
    mapa[2].tropas = 2;
    
    strcpy(mapa[3].nome, "Alemanha");
    strcpy(mapa[3].cor, "Vermelho");
    mapa[3].tropas = 5;
    
    strcpy(mapa[4].nome, "Franca");
    strcpy(mapa[4].cor, "Vermelho");
    mapa[4].tropas = 3;
    
    strcpy(mapa[5].nome, "Espanha");
    strcpy(mapa[5].cor, "Vermelho");
    mapa[5].tropas = 2;
    
    // aloca memoria para as missoes dos jogadores
    char* missaoJ1 = (char*) malloc(100 * sizeof(char));
    char* missaoJ2 = (char*) malloc(100 * sizeof(char));
    
    if (missaoJ1 == NULL || missaoJ2 == NULL) {
        printf("ERRO: Falha ao alocar memoria para missoes!\n");
        free(mapa);
        return 1;
    }

    // sorteia as missoes
    atribuirMissao(missaoJ1, missoes, NUM_MISSOES, COR_JOGADOR1);
    atribuirMissao(missaoJ2, missoes, NUM_MISSOES, COR_JOGADOR2);

    // exibe as missoes no inicio do jogo
    printf("=======================================================\n");
    printf("         BEM-VINDO AO WAR ESTRUTURADO\n");
    printf("=======================================================\n\n");
    
    printf("Jogador 1 (%s), sua missao secreta:\n", COR_JOGADOR1);
    exibirMissao(missaoJ1);
    printf("\n");

    printf("Jogador 2 (%s), sua missao secreta:\n", COR_JOGADOR2);
    exibirMissao(missaoJ2);
    
    printf("\n=======================================================\n");
    printf("                 O JOGO COMECOU!\n");
    printf("=======================================================\n\n");

    int turno = 1;
    int vencedor = 0;

    // loop principal
    while (vencedor == 0) {
        exibirMapa(mapa, NUM_TERRITORIOS);
        
        // define quem joga neste turno
        const char* corAtual = (turno % 2 != 0) ? COR_JOGADOR1 : COR_JOGADOR2;
        int numJogador = (turno % 2 != 0) ? 1 : 2;

        printf("\n--- TURNO %d: Jogador %d (%s) ---\n", turno, numJogador, corAtual);

        int idAtacante, idDefensor;
        printf("ID do territorio atacante: ");
        scanf("%d", &idAtacante);
        limparBuffer();

        printf("ID do territorio defensor: ");
        scanf("%d", &idDefensor);
        limparBuffer();

        // validacoes
        if (idAtacante < 0 || idAtacante >= NUM_TERRITORIOS || 
            idDefensor < 0 || idDefensor >= NUM_TERRITORIOS) {
            printf("\nID invalido! Tente novamente.\n");
            continue;
        }
        
        if (strcmp(mapa[idAtacante].cor, corAtual) != 0) {
            printf("\nVoce nao pode atacar com territorio de outro jogador!\n");
            continue;
        }
        
        if (strcmp(mapa[idDefensor].cor, corAtual) == 0) {
            printf("\nVoce nao pode atacar seu proprio territorio!\n");
            continue;
        }
        
        if (mapa[idAtacante].tropas <= 1) {
            printf("\nTropas insuficientes! Precisa de mais de 1 tropa.\n");
            continue;
        }

        // realiza o ataque
        atacar(&mapa[idAtacante], &mapa[idDefensor]);

        // verifica se o jogador cumpriu a missao
        if (numJogador == 1) {
            if (verificarMissao(missaoJ1, COR_JOGADOR1, mapa, NUM_TERRITORIOS)) {
                vencedor = 1;
            }
        } else {
            if (verificarMissao(missaoJ2, COR_JOGADOR2, mapa, NUM_TERRITORIOS)) {
                vencedor = 2;
            }
        }
        
        turno++;
        printf("\n");
    }

    // anuncia o vencedor
    printf("\n=======================================================\n");
    printf("                   FIM DE JOGO!\n");
    printf("=======================================================\n");
    printf("\nJogador %d (%s) cumpriu sua missao e VENCEU!\n\n", 
           vencedor, (vencedor == 1) ? COR_JOGADOR1 : COR_JOGADOR2);
    
    exibirMapa(mapa, NUM_TERRITORIOS);
    
    // libera memoria
    liberarMemoria(mapa, missaoJ1, missaoJ2);

    return 0;
}


// Funcao que sorteia e atribui uma missao para o jogador
// Garante que o jogador nao receba missao de eliminar a si mesmo
void atribuirMissao(char* destino, const char* missoes[], int total, const char* corJogador) {
    int valida = 0;
    
    while (!valida) {
        int indice = rand() % total;
        strcpy(destino, missoes[indice]);

        // verifica se a missao nao e impossivel (eliminar a si mesmo)
        if (strcmp(destino, "Eliminar todas as tropas do jogador Azul") == 0 && 
            strcmp(corJogador, "Azul") == 0) {
            valida = 0;
        } else if (strcmp(destino, "Eliminar todas as tropas do jogador Vermelho") == 0 && 
                   strcmp(corJogador, "Vermelho") == 0) {
            valida = 0;
        } else {
            valida = 1;
        }
    }
}


// Exibe a missao do jogador
void exibirMissao(const char* missao) {
    printf("  >> %s\n", missao);
}


// Mostra o estado atual do mapa
void exibirMapa(Territorio* mapa, int tamanho) {
    printf("\n--- MAPA ATUAL ---\n");
    for (int i = 0; i < tamanho; i++) {
        printf("[%d] %-12s | Cor: %-9s | Tropas: %d\n",
               i, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
}


// Simula um ataque entre dois territorios usando dados
void atacar(Territorio* atacante, Territorio* defensor) {
    printf("\n*** ATAQUE ***\n");
    printf("%s (%d tropas) vs %s (%d tropas)\n",
           atacante->nome, atacante->tropas, defensor->nome, defensor->tropas);
           
    // rola os dados (1 a 6)
    int dadoAtk = rand() % 6 + 1;
    int dadoDef = rand() % 6 + 1;

    printf("Dados: Atacante [%d] | Defensor [%d]\n", dadoAtk, dadoDef);

    if (dadoAtk > dadoDef) {
        // atacante venceu
        printf("VITORIA! %s conquistou %s!\n", atacante->nome, defensor->nome);
        
        strcpy(defensor->cor, atacante->cor);
        
        // move metade das tropas
        int tropas_mover = (atacante->tropas - 1) / 2;
        if (tropas_mover < 1) tropas_mover = 1;
        
        defensor->tropas = tropas_mover;
        atacante->tropas -= tropas_mover;
        
    } else {
        // defensor venceu
        printf("DERROTA! %s perdeu uma tropa.\n", atacante->nome);
        atacante->tropas--;
    }
}


// Verifica se o jogador cumpriu sua missao
// Retorna 1 se cumpriu, 0 se nao
int verificarMissao(const char* missao, const char* corJogador, Territorio* mapa, int tamanho) {
    
    // Missao 1: Conquistar 4 territorios
    if (strcmp(missao, "Conquistar 4 territorios no total") == 0) {
        int contador = 0;
        for (int i = 0; i < tamanho; i++) {
            if (strcmp(mapa[i].cor, corJogador) == 0) {
                contador++;
            }
        }
        return contador >= 4;
    }
    
    // Missao 2: Eliminar jogador Vermelho
    if (strcmp(missao, "Eliminar todas as tropas do jogador Vermelho") == 0) {
        for (int i = 0; i < tamanho; i++) {
            if (strcmp(mapa[i].cor, "Vermelho") == 0) {
                return 0;  // ainda existe territorio vermelho
            }
        }
        return 1;  // nao tem mais vermelho
    }

    // Missao 3: Eliminar jogador Azul
    if (strcmp(missao, "Eliminar todas as tropas do jogador Azul") == 0) {
        for (int i = 0; i < tamanho; i++) {
            if (strcmp(mapa[i].cor, "Azul") == 0) {
                return 0;
            }
        }
        return 1;
    }

    // Missao 4: Conquistar Brasil
    if (strcmp(missao, "Conquistar o territorio 'Brasil'") == 0) {
        for (int i = 0; i < tamanho; i++) {
            if (strcmp(mapa[i].nome, "Brasil") == 0 && 
                strcmp(mapa[i].cor, corJogador) == 0) {
                return 1;
            }
        }
        return 0;
    }

    // Missao 5: Ter 10 tropas em um territorio
    if (strcmp(missao, "Ter 10 tropas em um unico territorio") == 0) {
        for (int i = 0; i < tamanho; i++) {
            if (strcmp(mapa[i].cor, corJogador) == 0 && mapa[i].tropas >= 10) {
                return 1;
            }
        }
        return 0;
    }

    return 0;
}


// Libera toda memoria alocada dinamicamente
void liberarMemoria(Territorio* mapa, char* missao1, char* missao2) {
    printf("\nLiberando memoria...\n");
    free(mapa);
    free(missao1);
    free(missao2);
    printf("Memoria liberada.\n");
}


// Limpa o buffer de entrada
void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}