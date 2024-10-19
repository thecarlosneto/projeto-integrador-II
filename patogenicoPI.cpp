#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <cmath>
#include <stdlib.h>
#include <stdio.h>
#define displayWidth 800
#define displayHeight 600
#define espacoEntreGrade 50
#define nCelulasGrid ((displayWidth / espacoEntreGrade) * (displayHeight / espacoEntreGrade))
#define nColunas (displayWidth / espacoEntreGrade)
#define nLinhas (displayHeight / espacoEntreGrade)

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
   
    if (colisaoCirculoDentro(pNutrientes[indexNutriente].x, pNutrientes[indexNutriente].y, raioNutriente, pPlayer_fago->x, pPlayer_fago->y, pPlayer_fago->raio)) {
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

void checaColisaoVizinhos(short int (*pGridMap)[10], short int* pQtdNutrientes,nutriente* pNutrientes, int raioNutriente, player_fagocitose* pPlayer_fago, int posicaoPlayerGrid) {
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
                        colisaoPlayerNutriente(pPlayer_fago, pNutrientes, raioNutriente, pQtdNutrientes, pGridMap, indexNutriente);
                    }
                }
            }
        }
    }
}

void desenharFundo(ALLEGRO_DISPLAY* display, ALLEGRO_BITMAP* bitmap,ALLEGRO_COLOR corFundo, int espacamento, ALLEGRO_COLOR corGrade, int grossuraLinha) {
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



int main() {

    ALLEGRO_COLOR BLACK = al_map_rgb(0, 0, 0);
    ALLEGRO_COLOR WHITE = al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR GRAY = al_map_rgb(200, 200, 200);
    ALLEGRO_COLOR RED = al_map_rgb(255, 0, 0);
    ALLEGRO_COLOR GREEN = al_map_rgb(0, 255, 0);
    ALLEGRO_COLOR BLUE = al_map_rgb(0, 0, 255);
    ALLEGRO_COLOR YELLOW = al_map_rgb(255, 255, 0);
    ALLEGRO_COLOR PINK = al_map_rgb(221, 160, 221);

    /*****SETUP DO ALLEGRO*****/
    al_init();
    al_init_primitives_addon();
    al_init_font_addon();
    al_init_ttf_addon();
    al_install_mouse();

    // Cria uma janela
    ALLEGRO_DISPLAY* display = al_create_display(displayWidth, displayHeight);

    // define o FPS
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);

    // Cria uma fila de eventos
    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();

    // Registra a fila de eventos com as fontes de eventos (display, timer, mouse)
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_mouse_event_source());

    al_start_timer(timer);

    ALLEGRO_FONT* font = al_create_builtin_font();
    ALLEGRO_FONT* fonteHUD = al_load_ttf_font("font/fonteWindowsRegular.ttf", 30, 0);
    ALLEGRO_BITMAP* fundoBitmap = al_create_bitmap(displayWidth, displayHeight);

    /*****VARIAVEIS LOCAIS*****/
    
    controle_fagocitose control_fago; // controlador geral do fagocitose
    nutriente* nutrientes; // nutrientes que são comidos pro player crescer
    player_fagocitose player_fago;
    fagocito fago_pong; // inimigo que se move igual a bola do jogo Pong

    const int numNutrientes = 100;     // quantos nutrientes há no mapa
    const int raioNutriente = 10;

    int timerInvencibilidade = 0;

    short int gridMap[nCelulasGrid][10];  // grade de fundo onde cada nutriente será atribuido
    short int qtdNutrientesGrid[nCelulasGrid]; // quantos nutrientes há em cada espaço na grid

    /***** FIM VARIAVEIS LOCAIS*****/


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
    int pong_velY= 1;  //vai subir ou descer

    /*****FIM SETUP DO JOGO*****/



    while (true) {
        ALLEGRO_EVENT event;
        al_wait_for_event(event_queue, &event);

        // Verifica se o usuário clicou no fechar a janela
        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            break;
        }

        // Verifica se o evento é do temporizador
        if (event.type == ALLEGRO_EVENT_TIMER) {
            // Estado do mouse
            ALLEGRO_MOUSE_STATE mState;
            al_get_mouse_state(&mState);

            // Calcula a direção e a distância até a posição do mouse
            //diferença entre o mouse e o player
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
            //PLAYER X INIMIGO
            if (!control_fago.morto && colisaoCirculoDentro(player_fago.x, player_fago.y, player_fago.raio, fago_pong.x, fago_pong.y, fago_pong.raio)){
                control_fago.morto = true;
                al_clear_to_color(RED);
                player_fago.raio = player_fago.raio_min;
                control_fago.tentativas--;
            }
            //se as vidas acabarem
            if (control_fago.tentativas <= 0)
                control_fago.gameover = true;
            if (control_fago.pontuacao >= 10 * 100)
                control_fago.venceu = true;

            //deixa o player invencível por alguns segundos após morrer
            if (control_fago.morto) {
                timerInvencibilidade++;
                if(timerInvencibilidade / 60 == 1){
                    control_fago.morto = false;
                    timerInvencibilidade = 0;
                }
            }
            
            //reseta tamanho do player
            if (player_fago.raio >= player_fago.raio_max) {
                player_fago.raio = player_fago.raio_min;
                control_fago.pontuacao+= 100;
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
        al_draw_textf(fonteHUD,  BLACK, 10, 0, ALLEGRO_ALIGN_LEFT,"pontos = %d", control_fago.pontuacao);
        al_draw_textf(fonteHUD, BLACK, 10, 34, ALLEGRO_ALIGN_LEFT, "vidas = %d", control_fago.tentativas);

        /*****FIM DESENHO*****/
        al_flip_display();
    }

    // Limpa os recursos alocados
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
    al_destroy_timer(timer);
    al_destroy_bitmap(fundoBitmap);
    al_destroy_font(font);
    al_destroy_font(fonteHUD);

    free(nutrientes);
    return 0;
}
