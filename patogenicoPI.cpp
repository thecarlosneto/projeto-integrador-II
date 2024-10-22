#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <math.h>
#include <cmath>

#define displayWidth 800
#define displayHeight 600
#define espacoEntreGrade 50
#define nCelulasGrid ((displayWidth / espacoEntreGrade) * (displayHeight / espacoEntreGrade))
#define nColunas (displayWidth / espacoEntreGrade)
#define nLinhas (displayHeight / espacoEntreGrade)

#define gameOver 0
#define telaLoading 1
#define telaInicial 2
#define seletorFase 3
#define ataqueMosquito 4
#define fagocitose 5
#define viremia 6
#define venceuViremia 7

#define PI 3.14159265358979323846

typedef struct {
    ALLEGRO_BITMAP* cd8Viremia;
    float x, y, velocidade;
    int direcao; // -1 para cima, 1 para baixo
} obstaculoViremia;

typedef struct {
    bool morto;
    bool venceu;
    bool gameover;
    short int tentativas;
    short int pontuacao;
    bool desenhouFundo;
} controle_fagocitose;

typedef struct {
    float x;
    float y;
    int vel;
    float raio;
    float raio_min;
    float raio_max;
} player_fagocitose;

typedef struct {
    short int x;
    short int y;
} nutriente;

typedef struct {
    float x;
    float y;
    int raio;
    int vel;
} fagocito;


//FUNÇÕES FAGOCITOSE

//Se os valores da instância A estiverem dentro da instância B, retorna verdadeiro.
//inserir coord A PARTIR DO CANTO SUPERIOR ESQUERDO, igual no p5js
bool colisaoQuadradoDentro(int xA, int yA, int widthA, int heightA, int xB, int yB, int widthB, int heightB) {
    if ((xB < xA && xA < xB + widthA) && (yB < yA && yA < yB + heightB)
        &&
        (xB < xA + widthA && xA + widthA < xB + widthB) && (yB < yA + heightA && yA + heightA < yB + heightB))
        return true;
    return false;
}

//retorna verdadeiro se o círculo A estiver dentro do círculo B
bool colisaoCirculoDentro(int xA, int yA, int rA, int xB, int yB, int rB) {
    //geometria analítica para descobrir o tamanho de uma reta (norma)
    int dx = xA - xB;
    int dy = yA - yB;
    int normaAB = sqrt(dx * dx + dy * dy);

    if (normaAB + rA <= rB)
        return true;
    return false;

}

int posicaoAleatoria(int tamDisplay, int tamObjeto) {
    return rand() % (tamDisplay - tamObjeto) + tamObjeto / 2;
}

//recebe um par ordenado e retorna em qual posição ele está na grade de fundo (como se fosse um array unidimensional)
int pegaPosicaoGrid(int x, int y) {
    return ((x - (x % espacoEntreGrade)) / espacoEntreGrade) + ((y - (y % espacoEntreGrade)) / espacoEntreGrade) * nColunas;
}

//define uma posição aleatória para cada nutriente
void mapearNutrientes(nutriente* pNutrientes, const int raioNutriente, int iNutriente, short int (*pGridMap)[10], short int* pQtdNutrientes) {
    pNutrientes->x = posicaoAleatoria(displayWidth, raioNutriente * 2);
    pNutrientes->y = posicaoAleatoria(displayHeight, raioNutriente * 2);

    //mapeia cada posição nova de nutriente para um espaço na grid
    int posicaoNaGrid = pegaPosicaoGrid(pNutrientes->x, pNutrientes->y);
    pQtdNutrientes[posicaoNaGrid]++;
    pGridMap[posicaoNaGrid][pQtdNutrientes[posicaoNaGrid]] = iNutriente; //no espaço X na grid, envia o index do nutriente 
}

void colisaoPlayerNutriente(player_fagocitose* pPlayer_fago, nutriente* pNutrientes, int raioNutriente, short int* pQtdNutrientes, short int (*pGridMap)[10], int indexNutriente) {
    if ((indexNutriente >= 0 && indexNutriente <= 100) && colisaoCirculoDentro(pNutrientes[indexNutriente].x, pNutrientes[indexNutriente].y, raioNutriente, pPlayer_fago->x, pPlayer_fago->y, pPlayer_fago->raio)) {
        pPlayer_fago->raio++; //aumenta tamanho do player
        // descobre qual espaço o nutriente  está na grid e o remove de lá
        int posicaoNutrienteGrid = pegaPosicaoGrid(pNutrientes[indexNutriente].x, pNutrientes[indexNutriente].y);
        pQtdNutrientes[posicaoNutrienteGrid]--;
        // remove o index do nutriente do gridMap
        for (int l = 0; l <= pQtdNutrientes[posicaoNutrienteGrid]; l++) {
            if (pGridMap[posicaoNutrienteGrid][l] == indexNutriente) {
                // move todos os elementos daquele espaço na grid um pra esquerda
                for (int m = l; m < pQtdNutrientes[posicaoNutrienteGrid] + 1; m++) {
                    pGridMap[posicaoNutrienteGrid][m] = pGridMap[posicaoNutrienteGrid][m + 1];
                }
                break;
            }
        }
        // Atualiza a posição do nutriente na grid
        mapearNutrientes(&pNutrientes[indexNutriente], raioNutriente, indexNutriente, pGridMap, pQtdNutrientes);
    }
}

void checaColisaoVizinhos(short int (*pGridMap)[10], short int* pQtdNutrientes, nutriente* pNutrientes, int raioNutriente, player_fagocitose* pPlayer_fago, int posicaoPlayerGrid) {
    /*
    percorre os 9 espaços ao redor do player:

       pos-1-nCol |pos-nCol |pos+1-nCol
       -----------|---------|----------
       pos-1      |pos      |pos+1
       -----------|---------|----------
       pos-1+nCol |pos+nCol |pos+1+nCol
    */
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            int pos = posicaoPlayerGrid + i + j * nColunas;
            //garante que nunca vai checar um espaço nulo (out of bounds)
            if (pos >= 0 && pos < nCelulasGrid) {
                //se houver nutrientes em um espaço da grid
                if (pQtdNutrientes[pos] > 0) {
                    //para cada nutriente em um espaço na grid
                    for (int k = 0; k <= pQtdNutrientes[pos]; k++) {
                        int indexNutriente = pGridMap[pos][k];
                        //printf("index nutriente: %d\n", indexNutriente);
                        colisaoPlayerNutriente(pPlayer_fago, pNutrientes, raioNutriente, pQtdNutrientes, pGridMap, indexNutriente);
                    }
                }
            }
        }
    }
}

void desenharFundo(ALLEGRO_DISPLAY* display, ALLEGRO_BITMAP* bitmap, ALLEGRO_COLOR corFundo, int espacamento, ALLEGRO_COLOR corGrade, int grossuraLinha) {
    al_set_target_bitmap(bitmap);
    al_clear_to_color(corFundo);
    for (int i = 1; i < displayWidth / espacamento; i++) {
        al_draw_line(i * espacamento, 0, i * espacamento, displayHeight, corGrade, grossuraLinha);    //desenha a linha
        for (int j = 1; j < displayHeight / espacamento; j++) {
            al_draw_line(0, j * espacamento, displayWidth, j * espacamento, corGrade, grossuraLinha); //desenha a coluna
        }
    }
    al_set_target_bitmap(al_get_backbuffer(display));
}



//FUNÇÕES VIREMIA
/*Função que gera coordenadas de X aleatorias com base no tamanho do display,
  preenchendo um array. Onde a primeira e a última posição já tem coordenadas
  pré estabelecidas*/
void geracoordenadasX(int* coordenadaX, int tamanho, int x1, int x2) {
    coordenadaX[0] = x1;
    coordenadaX[tamanho - 1] = x2;
    for (int i = 1; i < tamanho - 1; i++) {
        coordenadaX[i] = 20 + rand() % (displayWidth - 40);
    }
}

/*Função que gera coordenadas de Y aleatorias com base no tamanho do display,
  preenchendo um array. Onde a primeira e a última posição já tem coordenadas
  pré estabelecidas*/
void geracoordenadasY(int* coordenadaY, int tamanho, int y1, int y2) {
    coordenadaY[0] = y1;
    coordenadaY[tamanho - 1] = y2;
    for (int i = 1; i < tamanho - 1; i++) {
        coordenadaY[i] = 20 + rand() % (displayHeight - 40);
    }
}

//Função que gera retas colineares de acordo com as coordenadas 
void gerarLinhas(int* coordenadaX, int* coordenadaY, int tamanho, int espessuraLinha) {
    for (int i = 0; i < tamanho - 1; i++) {
        al_draw_line(coordenadaX[i], coordenadaY[i], coordenadaX[i + 1], coordenadaY[i + 1], al_map_rgb(119, 25, 15), espessuraLinha);
    };
}

// Função para verificar se o círculo está sobre uma linha com espessura
bool foraDaLinha(float x, float y, float x1, float y1, float x2, float y2, float espessura) {
    // Calcular a distância perpendicular do ponto à reta
    float A = y2 - y1;
    float B = x1 - x2;
    float C = x2 * y1 - x1 * y2;
    float distancia = fabs(A * x + B * y + C) / sqrt(A * A + B * B);

    return distancia > espessura / 2;
}

//função que cria linhas onduladas por meio do calculo da Função Seno
void linhasOnduladas(float x1, float y1, float x2, float y2, int qtdOndas) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float comprimentoDaOnda = dx / qtdOndas;
    float amplitude = 20; // Ajuste a amplitude para controlar a altura das ondas

    for (int i = 0; i <= qtdOndas; ++i) {
        float x = x1 + i * comprimentoDaOnda;
        float y = y1 + dy * i / qtdOndas + amplitude * sin(i * 2 * PI / qtdOndas);
        al_draw_line(x, y, x + comprimentoDaOnda, y, al_map_rgb(252, 186, 3), 5.0);
    }
}




int main() {
    //atribui uma seed aleatória. Caso o jogo crashe ou algo do tipo, essa é uma boa forma de debug
    //tutorial de como usar a seed:
        //ela sempre será imprimida no terminal no começo do jogo.
        //se crashar, pega a seed e coloca ela no lugar de time(NULL);
        //depois que debugar, volta a seed pro time(NULL);
    unsigned long seed = time(NULL);
    srand(seed);
    printf("Seed: %lu\n", seed);

    /*****SETUP DO ALLEGRO*****/
    al_init();
    al_init_primitives_addon();
    al_init_image_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    al_install_mouse();

    // Cria uma janela
    ALLEGRO_DISPLAY* display = al_create_display(displayWidth, displayHeight);

    // define o FPS
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);

    //fontes de texto
    ALLEGRO_FONT* fonteHUD = al_load_ttf_font("font/fonteWindowsRegular.ttf", 30, 0);
    ALLEGRO_FONT* font = al_create_builtin_font();
    // imagens
    ALLEGRO_BITMAP* loading = al_load_bitmap("img/telaLoading.png");
    ALLEGRO_BITMAP* background = al_load_bitmap("img/telaTeste.png");
    ALLEGRO_BITMAP* background2 = al_load_bitmap("img/tela2Teste.png");
    ALLEGRO_BITMAP* backgroundViremia = al_load_bitmap("img/backgroundViremia.png");
    ALLEGRO_BITMAP* virusViremia = al_load_bitmap("img/virus.png");
    ALLEGRO_BITMAP* cd8Viremia = al_load_bitmap("img/cd8Viremia.png");

    ALLEGRO_BITMAP* fundoBitmap = al_create_bitmap(displayWidth, displayHeight);
    //cores
    ALLEGRO_COLOR BLACK = al_map_rgb(0, 0, 0);
    ALLEGRO_COLOR WHITE = al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR GRAY = al_map_rgb(200, 200, 200);
    ALLEGRO_COLOR RED = al_map_rgb(255, 0, 0);
    ALLEGRO_COLOR GREEN = al_map_rgb(0, 255, 0);
    ALLEGRO_COLOR BLUE = al_map_rgb(0, 0, 255); 
    ALLEGRO_COLOR ROYAL = al_map_rgb(0, 0, 255); // loading
    ALLEGRO_COLOR YELLOW = al_map_rgb(255, 255, 0);
    ALLEGRO_COLOR PINK = al_map_rgb(221, 160, 221);
    //input e output
    ALLEGRO_MOUSE_STATE mState;

    // Cria uma fila de eventos
    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();

    // Registra a fila de eventos com as fontes de eventos (display, timer, mouse)
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_mouse_event_source());

    al_start_timer(timer);



    /*****FIM SETUP DO ALLEGRO*****/



    // - - - - - - - VARIÁVEIS GERAIS - - - - - - -
    int tela = 1;
    // - - - - - - -FIM DAS VARIÁVEIS GERAIS - - - - - - -

        // - - - - - - - VARIÁVEIS PARA A TELA LOADING - - - - - - -
    int contadorDeFrames = 0;
    const int frameSegundo = 60;
    const int intervaloSegundo = 2;

    int opacidadeTexto = 255;

    const int widthLoading = 8;
    const int heightLoading = 10;
    const int distanciaLoading = 2; // Distância entre os retângulos
    

    typedef struct {
        float xLoading, yLoading;
        int wLoading, hLoading;
    } Retangulo;

    // Lista de retângulos
    Retangulo retangulos[100];
    int num_retangulos = 1;

    // Primeiro retângulo
    retangulos[0].xLoading = 380;
    retangulos[0].yLoading = 295;
    retangulos[0].wLoading = widthLoading;
    retangulos[0].hLoading = heightLoading;
    //  - - - - - - - FIM DAS VARIÁVEIS PARA A TELA LOADING - - - - - - -

    // - - - - - - - VARIÁVEIS PARA FAGOCITOSE - - - - - - -
    controle_fagocitose control_fago; // controlador geral do fagocitose
    nutriente* nutrientes; // nutrientes que são comidos pro player crescer
    player_fagocitose player_fago;
    fagocito fago_pong; // inimigo que se move igual a bola do jogo Pong

    const int numNutrientes = 100;     // quantos nutrientes há no mapa
    const int raioNutriente = 10;

    int timerInvencibilidade = 0;

    short int gridMap[nCelulasGrid][10];  // grade de fundo onde cada nutriente será atribuido
    short int qtdNutrientesGrid[nCelulasGrid]; // quantos nutrientes há em cada espaço na grid

    /*****SETUP DO JOGO*****/

    desenharFundo(display, fundoBitmap, WHITE, espacoEntreGrade, GRAY, 2);

    //inicializa array q marca quantos nutrientes tem em cada espaço na grid
    for (int i = 0; i < nCelulasGrid; i++) {
        qtdNutrientesGrid[i] = 0;
    }

    nutrientes = (nutriente*)malloc(numNutrientes * sizeof(nutriente)); // array dinâmico de nutrientes
    for (int i = 0; i < numNutrientes; i++) {
        mapearNutrientes(&nutrientes[i], raioNutriente, i, gridMap, qtdNutrientesGrid);  //define um valor aleatório para cada nutriente no mapa
    }

    //setup dos dados do minigame
    control_fago.pontuacao = 0;
    control_fago.morto = false;
    control_fago.venceu = false;
    control_fago.gameover = false;
    control_fago.desenhouFundo = false;
    control_fago.tentativas = 3;

    //define como o círculo do player spawna no mapa
    player_fago.x = displayWidth / 2;
    player_fago.y = displayHeight / 2;
    player_fago.vel = 5;
    player_fago.raio_min = raioNutriente * 2;
    player_fago.raio_max = 75;
    player_fago.raio = player_fago.raio_min;

    //define como o círculo do fagocito pong spawna no mapa
    fago_pong.raio = player_fago.raio_max;
    fago_pong.x = posicaoAleatoria(displayWidth, fago_pong.raio * 2);
    fago_pong.y = posicaoAleatoria(displayHeight, fago_pong.raio * 2);
    fago_pong.vel = 7;
    int pong_velX = 1; //controla se o fagocito pong
    int pong_velY = 1;  //vai subir ou descer

    /*****FIM SETUP DO JOGO*****/


    // - - - - - - -FIM DAS VARIÁVEIS PARA FAGOCITOSE - - - - - - -


        // - - - - - - - VARIÁVEIS PARA VIREMIA - - - - - - -
    //Coordenadas pré estabelecidas 
    int x1 = 50, y1 = 535;
    int x2 = 740, y2 = 50;

    // Posição inicial do círculo
    int circle_x = x1 + 10;
    int circle_y = y1 + 10;

    int xChegada;
    int yChegada;

    bool startJogo = false;
    bool mudouDeNivel = false;
    int nivelViremia = 1;

    int tamanho = 4;
    int espessuraLinha = 18.0;
    bool dentroDaLinha = false;
    // Aloca memória para o coordenadaX
    int* coordenadaX = (int*)malloc(tamanho * sizeof(int));
    if (coordenadaX == NULL) {
        printf("Erro ao alocar memória.\n");
        return 1; // Sai em caso de falha na alocação
    }

    // Aloca memória para o coordenadaY
    int* coordenadaY = (int*)malloc(tamanho * sizeof(int));
    if (coordenadaY == NULL) {
        printf("Erro ao alocar memória.\n");
        return 1; // Sai em caso de falha na alocação
    }

    // Chama a função para preencher o array com números aleatórios
    geracoordenadasX(coordenadaX, tamanho, x1, x2);
    geracoordenadasY(coordenadaY, tamanho, y1, y2);

    // Variáveis para o movimento circular
    float angle = 0; //variável de ângulo será incrementada a cada frame para simular o movimento circular
    float radius = 50;

    const int qtdCd8 = 5;

    // Criar um array para armazenar as informações das imagens
    obstaculoViremia linfocitoCd8[qtdCd8];

    // Inicializar as imagens
    for (int i = 0; i < qtdCd8; i++) {
        linfocitoCd8[i].cd8Viremia = al_clone_bitmap(cd8Viremia);
        linfocitoCd8[i].x = -((2 * al_get_bitmap_width(cd8Viremia)) * i);
        // Coordenada Y inicial entre 340 e 600
        linfocitoCd8[i].y = 340 + rand() % (600 - 340 + 1);
        linfocitoCd8[i].velocidade = 0.5 + (float)i / 10.0;
        // Direção inicial para baixo (para garantir que as imagens comecem descendo)
        linfocitoCd8[i].direcao = 1;
    }

    // - - - - - - -FIM DAS VARIÁVEIS PARA VIREMIA - - - - - - -


    // Loop de eventos
    bool running = true;
    while (running) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        // Verifica se o evento foi o fechamento da janela
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;  // Sai do loop e encerra o programa
        }

        switch (tela) {
        case telaLoading:

            // Desenha a imagem de fundo na tela (na posição (0, 0))
            al_draw_bitmap(loading, 0, 0, 0);

            if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                if (ev.keyboard.keycode == ALLEGRO_KEY_SPACE) {
                    //Tecla espaço pressionada
                    tela = telaInicial;
                }
            }

            if (ev.type == ALLEGRO_EVENT_TIMER) {
                contadorDeFrames++;
                //Verifica o timer a cada meio segundo
                if (contadorDeFrames % (frameSegundo / intervaloSegundo) == 0) {
                    // Criar um novo retângulo
                    if (num_retangulos <= 7) {
                        /*Calcula a posição x do novo retângulo,
                        baseada na posição x do último,
                        adicionando a largura e a distância entre eles.*/
                        retangulos[num_retangulos].xLoading = retangulos[num_retangulos - 1].xLoading + widthLoading + distanciaLoading;
                        retangulos[num_retangulos].yLoading = retangulos[0].yLoading; // Manter a mesma posição
                        retangulos[num_retangulos].wLoading = widthLoading;
                        retangulos[num_retangulos].hLoading = heightLoading;
                        num_retangulos++;
                    }
                    opacidadeTexto = opacidadeTexto * (-1);
                }
            }

            // Desenha todos os retângulos armazenados no array retangulos
            for (int i = 0; i < num_retangulos; i++) {
                al_draw_filled_rectangle(retangulos[i].xLoading, retangulos[i].yLoading,
                    retangulos[i].xLoading + retangulos[i].wLoading,
                    retangulos[i].yLoading + retangulos[i].hLoading,
                    ROYAL);
            }
            if (num_retangulos >= 8) {
                // Desenha o texto alterando a cor entre preto e branco a cada 2 segundos
                al_draw_textf(font, al_map_rgb(opacidadeTexto, opacidadeTexto, opacidadeTexto), 400, 565, ALLEGRO_ALIGN_CENTER, "APERTE A TECLA 'ESPAÇO' PARA INICIAR");
            }

            al_flip_display();

            break;

        case telaInicial:
        {
            // Desenha a imagem de fundo na tela (na posição (0, 0))
            al_draw_bitmap(background, 0, 0, 0);

            if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
                if (ev.mouse.button == 1) { // Botão esquerdo do mouse

                    if (ev.mouse.x > 570 && ev.mouse.x < 630 && ev.mouse.y>280 && ev.mouse.y < 320) {
                        tela = seletorFase;
                    }

                    printf("Clique detectado na coordenada (%d, %d)\n", ev.mouse.x, ev.mouse.y);
                }
            }
            // Desenhar o texto na tela usando a fonte embutida
            al_draw_text(font, al_map_rgb(0, 100, 0), 600, 300, ALLEGRO_ALIGN_CENTER, "jogar");

            al_flip_display();
        }
        break;

        case seletorFase:
        {
            al_draw_bitmap(background2, 0, 0, 0);

            if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
                if (ev.mouse.button == 1) { // Botão esquerdo do mouse

                    if (ev.mouse.y > 240 && ev.mouse.y < 340) {

                        if (ev.mouse.x > 150 && ev.mouse.x < 230) {

                            tela = ataqueMosquito;
                        }

                        if (ev.mouse.x > 345 && ev.mouse.x < 437) {

                            tela = fagocitose;
                        }

                        if (ev.mouse.x > 544 && ev.mouse.x < 627) {

                            tela = viremia;
                        }
                    }

                    printf("Clique detectado na coordenada (%d, %d)\n", ev.mouse.x, ev.mouse.y);
                }
            }
            al_flip_display();
        }
        break;
        case fagocitose:
        {
            if (ev.type == ALLEGRO_EVENT_TIMER) {
                al_get_mouse_state(&mState);
                // Calcula a direção e a distância até a posição do mouse
                float dx = mState.x - player_fago.x;
                float dy = mState.y - player_fago.y;
                float distancia = sqrt(dx * dx + dy * dy);
                // Move o círculo em direção ao mouse
                if (distancia > player_fago.vel) {
                    float move_x = (dx / distancia) * player_fago.vel;
                    float move_y = (dy / distancia) * player_fago.vel;
                    player_fago.x += move_x;
                    player_fago.y += move_y;
                }

                //garante que o player vai ficar dentro da tela
                if (player_fago.x > displayWidth - player_fago.raio)
                    player_fago.x = displayWidth - player_fago.raio;
                else if (player_fago.x < 0 + player_fago.raio)
                    player_fago.x = player_fago.raio;
                if (player_fago.y > displayHeight - player_fago.raio)
                    player_fago.y = displayHeight - player_fago.raio;
                else if (player_fago.y < 0 + player_fago.raio)
                    player_fago.y = player_fago.raio;

                int posicaoPlayerGrid = pegaPosicaoGrid(player_fago.x, player_fago.y);

                //movimentação fagócito inimigo - Pong
                    //caso bata em uma das "paredes" muda de direção
                if (fago_pong.x >= displayWidth - fago_pong.raio || fago_pong.x <= 0 + fago_pong.raio)
                    pong_velX *= -1;
                if (fago_pong.y >= displayHeight - fago_pong.raio || fago_pong.y <= 0 + fago_pong.raio)
                    pong_velY *= -1;
                fago_pong.x += fago_pong.vel * pong_velX;
                fago_pong.y += fago_pong.vel * pong_velY;

                //COLISÕES
                //PLAYER X NUTRIENTE
                checaColisaoVizinhos(gridMap, qtdNutrientesGrid, nutrientes, raioNutriente, &player_fago, posicaoPlayerGrid);
                printf("checou colisao vizinhos\n");
                //PLAYER X INIMIGO
                if (!control_fago.morto && colisaoCirculoDentro(player_fago.x, player_fago.y, player_fago.raio, fago_pong.x, fago_pong.y, fago_pong.raio)) {
                    control_fago.morto = true;
                    al_clear_to_color(RED);
                    player_fago.raio = player_fago.raio_min;
                    control_fago.tentativas--;
                }
                //se as vidas acabarem
                if (control_fago.tentativas <= 0) {
                    control_fago.gameover = true;
                    tela = gameOver;
                }
                if (control_fago.pontuacao >= 10 * 100) {
                    control_fago.venceu = true;
                    tela = viremia;
                }

                //deixa o player invencível por alguns segundos após morrer
                if (control_fago.morto) {
                    timerInvencibilidade++;
                    if (timerInvencibilidade / 60 == 1) {
                        control_fago.morto = false;
                        timerInvencibilidade = 0;
                    }
                }

                //reseta tamanho do player
                if (player_fago.raio >= player_fago.raio_max) {
                    player_fago.raio = player_fago.raio_min;
                    control_fago.pontuacao += 100;
                }
                //diminui constantemente o tamanho do player
                if (player_fago.raio > player_fago.raio_min)
                    player_fago.raio -= 0.03;
                else
                    player_fago.raio = player_fago.raio_min;
            }
            /*****DESENHO*****/
            al_draw_bitmap(fundoBitmap, 0, 0, 0);
            //desenha nutrientes
            for (int i = 0; i < numNutrientes; i++) {
                int opcaoCor = i % 3; //numero de opcoes de cores
                ALLEGRO_COLOR corNutriente;
                if (opcaoCor == 0)
                    corNutriente = PINK;
                else if (opcaoCor == 1)
                    corNutriente = BLUE;
                else {
                    corNutriente = GREEN;
                }
                al_draw_filled_circle(nutrientes[i].x, nutrientes[i].y, raioNutriente, corNutriente);
            }
            // Desenha player
            al_draw_filled_circle(player_fago.x, player_fago.y, player_fago.raio, RED);

            // desenha inimigo (elipse)
            al_draw_filled_circle(fago_pong.x, fago_pong.y, fago_pong.raio, BLACK);

            // HUD
            al_draw_textf(fonteHUD, BLACK, 10, 0, ALLEGRO_ALIGN_LEFT, "pontos = %d", control_fago.pontuacao);
            al_draw_textf(fonteHUD, BLACK, 10, 34, ALLEGRO_ALIGN_LEFT, "vidas = %d", control_fago.tentativas);

            /*****FIM DESENHO*****/
            al_flip_display();
        }
        break;

        case viremia:
            // Desenha a imagem de fundo
            al_draw_bitmap(backgroundViremia, 0, 0, 0);

            // Verifica se o evento é do temporizador
            if (ev.type == ALLEGRO_EVENT_TIMER) {
                // Estado do mouse
                ALLEGRO_MOUSE_STATE mState;
                al_get_mouse_state(&mState);

                // Verifica se o mouse está sobre o círculo
                if (mState.x >= circle_x - 10 && mState.x <= circle_x + 10 &&
                    mState.y >= circle_y - 10 && mState.y <= circle_y + 10) {

                    startJogo = true;
                };
                // Reiniciar a cada iteração
                dentroDaLinha = false;

                //Fase viremia começou
                if (startJogo == true) {
                    circle_x = mState.x;
                    circle_y = mState.y;
                    //Verifica se o círculo está em cima de alguma linha ou dos quadrados brancos
                    for (int i = 0; i < tamanho - 1; i++) {
                        if (!foraDaLinha(circle_x, circle_y, coordenadaX[i], coordenadaY[i], coordenadaX[i + 1],
                            coordenadaY[i + 1], espessuraLinha)
                            || circle_x >= x1 && circle_x <= x1 + 20 && circle_y >= y1 && circle_y <= y1 + 20
                            || circle_x >= x2 && circle_x <= x2 + 20 && circle_y >= y2 && circle_y <= y2 + 20) {
                            dentroDaLinha = true;
                            break; // Se o cursor estiver sobre uma linha, não precisa verificar as demais
                        }
                    }

                    //Fora da linha
                    if (!dentroDaLinha) {
                        //deixar comentado por enquanto => tela = gameOver;
                        al_draw_text(font, al_map_rgb(255, 255, 255), 100, 200, ALLEGRO_ALIGN_CENTER, "GAME OVER");
                    }
                    if (nivelViremia == 1 || nivelViremia == 3) {
                        xChegada = x2;
                        yChegada = y2;
                    }
                    else if (nivelViremia == 2) {
                        xChegada = x1;
                        yChegada = y1;
                    }
                    if (nivelViremia < 3 && (circle_x >= xChegada && circle_x <= xChegada + 20 && circle_y >= yChegada && circle_y <= yChegada + 20)) {
                        mudouDeNivel = true;
                    }
                    if (mudouDeNivel == true) {
                        geracoordenadasX(coordenadaX, tamanho, x1, x2);
                        geracoordenadasY(coordenadaY, tamanho, y1, y2);
                        //Diminui a espessura da linha
                        espessuraLinha -= espessuraLinha * 0.12;
                        nivelViremia++;

                        //Reseta mudouDeNivel
                        mudouDeNivel = false;
                    }
                    if (nivelViremia == 3 && (circle_x >= x2 && circle_x <= x2 + 20 && circle_y >= y2 && circle_y <= y2 + 20)) {
                        tela = venceuViremia;
                    }
                }
                // Desenhar o texto na tela usando a fonte embutida
                al_draw_textf(font, al_map_rgb(255, 255, 255), 700, 10, ALLEGRO_ALIGN_CENTER, "Nível: %d/3", nivelViremia);



                //Desenha as linhas chamando a função
                gerarLinhas(coordenadaX, coordenadaY, tamanho, espessuraLinha);

                //Desenha os quadrados brancos
                al_draw_filled_rectangle(x1, y1, x1 + 20, y1 + 20, al_map_rgb(255, 255, 255));
                al_draw_filled_rectangle(x2, y2, x2 + 20, y2 + 20, al_map_rgb(255, 255, 255));

                // Calcula a posição da imagem no círculo
                float x = circle_x + radius * cos(angle);
                float y = circle_y + radius * sin(angle);

                //Desenha as linhas onduladas a partir da Função seno
                linhasOnduladas(circle_x, circle_y, x, y, 40);

                // Desenha a imagem
                al_draw_bitmap(virusViremia, x - al_get_bitmap_width(virusViremia) / 2, y - al_get_bitmap_height(virusViremia) / 2, 0);

                // Desenha o círculo na nova posição
                al_draw_filled_circle(circle_x, circle_y, 10, al_map_rgb(255, 0, 0));

                // Incrementa o ângulo
                angle += 0.03;

                // Desenhar as imagens e atualizar as posições
                for (int i = 0; i < qtdCd8; i++) {
                    linfocitoCd8[i].x += linfocitoCd8[i].velocidade;
                    linfocitoCd8[i].y += linfocitoCd8[i].velocidade * linfocitoCd8[i].direcao;

                    if (linfocitoCd8[i].y > displayHeight) {
                        linfocitoCd8[i].y = displayHeight; // Limitar a coordenada Y máxima
                        linfocitoCd8[i].direcao = -1; // Mudar a direção para baixo
                    }
                    // Verificar se a imagem atingiu a faixa vertical final
                    if (linfocitoCd8[i].y <= 250) {
                        // Se a imagem estiver subindo e atingiu a faixa final, inverter a direção
                        linfocitoCd8[i].direcao = -1;
                    }

                    al_draw_bitmap(linfocitoCd8[i].cd8Viremia, linfocitoCd8[i].x, linfocitoCd8[i].y, 0);

                    // Se a imagem atingir o topo, reinicializa
                    if (linfocitoCd8[i].y < -50 || linfocitoCd8[i].x > 850) {
                        linfocitoCd8[i].x = -al_get_bitmap_width(cd8Viremia);
                        linfocitoCd8[i].y = 340 + rand() % (600 - 340 + 1);
                        linfocitoCd8[i].direcao = 1;
                    }
                }

                // Atualiza a tela
                al_flip_display();
            }
        }
        break;
    }


    // Liberar a memória
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_bitmap(loading);
    al_destroy_bitmap(background);
    al_destroy_bitmap(background2);
    al_destroy_bitmap(backgroundViremia);
    al_destroy_bitmap(fundoBitmap);
    al_destroy_font(font);
    al_destroy_font(fonteHUD);
    al_destroy_bitmap(cd8Viremia);
    for (int i = 0; i < qtdCd8; i++) {
        al_destroy_bitmap(linfocitoCd8[i].cd8Viremia);
    }
    al_shutdown_image_addon();

    // Libera a memória alocada
    free(coordenadaX);
    free(coordenadaY);
    free(nutrientes);

    return 0; // Encerrar o programa corretamente 

}
