#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_primitives.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <math.h>
#include <cmath>
#include <string.h>

#define DISPLAY_WIDTH 800
#define DISPLAY_HEIGHT 600
#define ESPACO_ENTRE_GRADE 50
#define NUM_CELULAS_GRID ((DISPLAY_WIDTH / ESPACO_ENTRE_GRADE) * (DISPLAY_HEIGHT / ESPACO_ENTRE_GRADE))
#define NUM_COLUNAS (DISPLAY_WIDTH / ESPACO_ENTRE_GRADE)
#define NUM_LINHAS (DISPLAY_HEIGHT / ESPACO_ENTRE_GRADE)

//variaveis usadas para fazer um array dos textos do dialogo

#define MAX_TEXTO 100 // Tamanho máximo de cada texto
#define NUM_TEXTO 5   // Número de textos que você deseja armazenar

#define GAME_OVER 0
#define TELA_LOADING 1
#define TELA_INICIAL 2
#define SELETOR_FASE 3
#define ATAQUE_MOSQUITO 4
#define FAGOCITOSE 5
#define VIREMIA 6
#define VENCEU_VIREMIA 7
#define TUTORIAL_ESTROFULO 8
#define TUTORIAL_FAGOCITOSE 9
#define TUTORIAL_VIREMIA 10
#define HQ 11

#define PI 3.14159265358979323846

typedef enum {
    NORTE = 1,
    SUL = 2,
    LESTE = 3,
    OESTE = 4
} posicao_texto;

//geral
typedef struct {
    int pontuacao[3];
    int pontuacao_total;
    bool venceu_jogo;
} controle_geral;

typedef struct {
    float x_loading, y_loading;
    int w_loading, h_loading;
} Retangulo; //retângulo da tela de loading

//picada mosquito (estrofulo)
typedef struct {
    int x;
    int y;
    int raio;
    int pontuacao;
    int vivo;
} player_estrofulo;

typedef struct {
    float x;
    float y;
    float raio;
    float velocidade;
} obstaculo_estrofulo;

//fagocitose
typedef struct {
    bool morto;
    bool venceu;
    short int tentativas;
    short int pontuacao;
    short int cont_vezes_crescidas;
    bool desenhou_fundo;
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


//viremia
typedef struct {
    ALLEGRO_BITMAP* celula_viremia;
    float x;
    float y;
    float diferenca;
} player_viremia;

typedef struct {
    ALLEGRO_BITMAP* cd8_viremia;
    float x, y, velocidade;
    int direcao; // -1 para cima, 1 para baixo
} obstaculo_viremia;

ALLEGRO_DISPLAY* display;

//fontes
ALLEGRO_FONT* fonte_HUD;
ALLEGRO_FONT* font;
ALLEGRO_FONT* fonte_20;
ALLEGRO_FONT* fonte_titulo;

//imagens 
ALLEGRO_BITMAP* loading;
ALLEGRO_BITMAP* background_desktop;
ALLEGRO_BITMAP* botao_play;
ALLEGRO_BITMAP* nextlevel;
ALLEGRO_BITMAP* background_fases;
ALLEGRO_BITMAP* tela_perdeu;
ALLEGRO_BITMAP* tela_tutorial;
ALLEGRO_BITMAP* mouse_tutorial;
ALLEGRO_BITMAP* spray_tutorial;
ALLEGRO_BITMAP* iconmosquitao;
ALLEGRO_BITMAP* teia_img;
ALLEGRO_BITMAP* background_estrofulo;
ALLEGRO_BITMAP* background_viremia;
ALLEGRO_BITMAP* celula_viremia;
ALLEGRO_BITMAP* bg_fagocitose_bitmap;
ALLEGRO_BITMAP* bg_pausa;

ALLEGRO_BITMAP* quadrinhos_HQ[3][4]{};

ALLEGRO_BITMAP* mosquitao;
ALLEGRO_BITMAP* spray_img;
ALLEGRO_BITMAP* mao_img;
ALLEGRO_BITMAP* bonus_img;

ALLEGRO_BITMAP* virus_viremia;
ALLEGRO_BITMAP* virus_PB;
ALLEGRO_BITMAP* cd8_viremia;

ALLEGRO_BITMAP* dialogo_virus;
ALLEGRO_BITMAP* dialogo_mosquito;

ALLEGRO_BITMAP* paintogenico;
ALLEGRO_BITMAP* bg_venceu_jogo;

//Audio
ALLEGRO_SAMPLE* colisao;
ALLEGRO_SAMPLE* gameover;
ALLEGRO_SAMPLE* morreu;
ALLEGRO_SAMPLE* next_level;
ALLEGRO_AUDIO_STREAM* trilha_sonora;
ALLEGRO_AUDIO_STREAM* mosquito;
ALLEGRO_AUDIO_STREAM* viremia_som;

//fontes de texto
ALLEGRO_FONT* fonte_texto;

//cores
ALLEGRO_COLOR BLACK = al_map_rgb(0, 0, 0);
ALLEGRO_COLOR WHITE = al_map_rgb(255, 255, 255);
ALLEGRO_COLOR GRAY = al_map_rgb(200, 200, 200);
ALLEGRO_COLOR RED = al_map_rgb(255, 0, 0);
ALLEGRO_COLOR GREEN = al_map_rgb(0, 255, 0);
ALLEGRO_COLOR BLUE = al_map_rgb(0, 0, 255);
ALLEGRO_COLOR YELLOW = al_map_rgb(255, 255, 0);
ALLEGRO_COLOR PINK = al_map_rgb(221, 160, 221);
//input e output
ALLEGRO_MOUSE_STATE mState;
ALLEGRO_KEYBOARD_STATE kState;
//timers
ALLEGRO_TIMER* timer;
ALLEGRO_TIMER* countdown_timer;

ALLEGRO_EVENT ev;

controle_geral controle;
int tela;
bool pode_pausar;
short int index_HQ = -1;
static float opacidade_HQs[4] = { 0, 0, 0, 0 }; // Armazena as opacidades dos quadrinhos
static int quadrinho_atual = 0;                 // Índice do quadrinho que está animando

int tempo_perdeu;
bool dialogo = true;

//ATAQUE MOSQUITO
obstaculo_estrofulo teia;
obstaculo_estrofulo mao;
obstaculo_estrofulo spray;
obstaculo_estrofulo bonus_estrofulo;

player_estrofulo player_mosquito;

float velocidade_x; // Define uma velocidade aleatória inicial entre 5 e 10
float suavidade; // Define uma velocidade a qual sera usada para efeito de suavização
float amplitude; // Amplitude da parábola
float offset; // Deslocamento Y inicial
float tempo_segundos; // Variavel de tempo para adicionar na barra de progresso e para aumentar a velocidade
float tempo_max; // Variavel de tempo para ditar o tempo maximo para o jogo "Estrofulo"
int indice_padroes; // Índice do padrão de movimento

float largura_barra;
float barra_progresso;
float icone_mosquito;
int cont_bonus;

//FAGOCITOSE
controle_fagocitose control_fago; // variáveis gerais como vida, pontuação, etc
nutriente* nutrientes; // nutrientes que são comidos pro player crescer
player_fagocitose player_fago;
fagocito fago_pong; // inimigo que se move igual a bola do jogo Pong
const int num_nutrientes = 100; // quantos nutrientes tem no mapa
const int raio_nutriente = 10;
int timer_invencibilidade;
short int grid_map[NUM_CELULAS_GRID][10];  // grade de fundo onde cada nutriente será atribuido
short int qtd_nutrientes_grid[NUM_CELULAS_GRID]; // quantos nutrientes há em cada espaço na grid
int pong_velX; //controla se o fagocito pong vai p/ direita ou esquerda
int pong_velY; //controla se o fagocito pong vai p/ cima ou baixo

//VIREMIA
//Setando os structs
player_viremia player_vire;
ALLEGRO_BITMAP* vidas_viremia[6]{};

int x_checkpoint1; int y_checkpoint1; //Coordenadas pré estabelecidas 
int x_checkpoint2; int y_checkpoint2; //Coordenadas pré estabelecidas 

int x_chegada;
int y_chegada;

bool pode_seguir_mouse;

bool startJogo;
bool mudou_de_nivel_viremia;
int nivel_viremia;

int tamanho;
int espessura_linha;
bool dentro_da_linha;

bool invulneravel;
int tempo_invulnerabilidade;
int contador_invulnerabilidade;

int vidas_disponiveis_viremia;
int vida_viremia_x;
int vida_viremia_y;

//para o cronometro
int cronometro;
int cronometro_reset;
char cronometro_str[10];
int pontos_viremia;

int* coordenada_X;
int* coordenada_Y;

// Variáveis para o movimento circular
float angulo_viremia;
float raio_viremia;
const int quantidade_CD8 = 5;
obstaculo_viremia* linfocito_CD8;



// textos dentro da caixa de dialogo
char textos[NUM_TEXTO][MAX_TEXTO] = {
      "Array sendo utilizada.",
      "Outro texto grande que pode ser usado no seu programa.",
      "Você pode armazenar até 5 textos aqui.",
      "Cada string pode ter até 99 caracteres.",
      "Isso é útil para gerenciar mensagens ou diálogos."
};




// FUNÇÕES GERAIS

void cria_paintogenico_bitmap() {
    ALLEGRO_BITMAP* paintogenico_fundo = al_load_bitmap("img/menus/msPaint98.png");
    ALLEGRO_BITMAP* desenho_paint = al_load_bitmap("img/menus/menu_paint_img.png");
    al_set_target_bitmap(paintogenico);

    al_draw_bitmap(paintogenico_fundo, 0, 0, 0);
    al_draw_bitmap(desenho_paint, 59, 47, 0);


    al_set_target_bitmap(al_get_backbuffer(display));
}

void criar_bitmap_botao(ALLEGRO_BITMAP* imagem, ALLEGRO_FONT* fonte, const char* txt, ALLEGRO_COLOR cor,int margem, posicao_texto posicao, float x, float y) {
    int largura_botao = al_get_bitmap_width(imagem);
    int altura_botao = al_get_bitmap_height(imagem);


    al_draw_bitmap(imagem, x, y, 0); // desenha o bitmap do botão na posição dado por x e y

    // variáveis para as posições do texto
    float pos_texto_x = x;
    float pos_texto_y = y;

    switch (posicao) {        // cada valor de 'posicao' simboliza uma area diferente a qual o texto sera gerado

    case NORTE:  // NORTE = 1 - define a posicao do texto para CIMA da imagem
        pos_texto_x = x + largura_botao / 2;
        pos_texto_y = y - margem - al_get_font_ascent(fonte);
        al_draw_text(fonte, cor, pos_texto_x, pos_texto_y, ALLEGRO_ALIGN_CENTER, txt);
        break;

    case SUL: // SUL = 2 - define a posicao do texto para BAIXO da imagem
        pos_texto_x = x + largura_botao / 2;
        pos_texto_y = y + altura_botao + margem;
        al_draw_text(fonte, cor, pos_texto_x, pos_texto_y, ALLEGRO_ALIGN_CENTER, txt);
        break;

    case LESTE: // LESTE = 3 - define a posicao do texto a DIREITA da imagem
        pos_texto_x = x + largura_botao + margem;
        pos_texto_y = y + altura_botao / 2 - al_get_font_ascent(fonte) / 2;
        al_draw_text(fonte, cor, pos_texto_x, pos_texto_y, ALLEGRO_ALIGN_LEFT, txt);
        break;

    case OESTE: // OESTE = 4 - define a posicao do texto a ESQUERDA da imagem
        pos_texto_x = x - margem;
        pos_texto_y = y + altura_botao / 2 - al_get_font_ascent(fonte) / 2;
        al_draw_text(fonte, cor, pos_texto_x, pos_texto_y, ALLEGRO_ALIGN_RIGHT, txt);
        break;
    }

}

void pause() {
    al_stop_timer(timer);
    al_stop_timer(countdown_timer);
}
void unpause() {
    al_resume_timer(timer);
    al_resume_timer(countdown_timer);
}

bool mouse_click() {
    return (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && ev.mouse.button == 1);
}

bool colisao_mouse(ALLEGRO_MOUSE_STATE mouse, int x, int y, int width, int height) {
    bool colisao_x = x <= mouse.x && mouse.x <= x + width;
    bool colisao_y = y <= mouse.y && mouse.y <= y + height;

    return colisao_x && colisao_y;
}

//se os valores da instância A estiverem dentro da instância B, retorna verdadeiro
//inserir coord A PARTIR DO CANTO SUPERIOR ESQUERDO, igual no p5js
bool colisao_quadrado_dentro(int xA, int yA, int widthA, int height_A, int xB, int yB, int widthB, int heightB) {
    bool colisao_X = xA < xB + widthB && xA + widthA > xB;
    bool colisao_Y = yA < yB + heightB && yA + height_A > yB;

    return colisao_X && colisao_Y;
}

//retorna verdadeiro se o círculo A estiver dentro do círculo B
bool colisao_circulo_dentro(int xA, int yA, int rA, int xB, int yB, int rB) {
    //geometria analítica para descobrir o tamanho de uma reta (norma)
    int dx = xA - xB;
    int dy = yA - yB;
    int norma_AB = sqrt(dx * dx + dy * dy);

    if (norma_AB + rA <= rB)
        return true;
    return false;

}

int posicao_aleatoria(int tam_display, int tam_objeto) {
    return rand() % (tam_display - tam_objeto) + tam_objeto / 2;
}

//desenha o que sobrepõe a tela ao pausar jogo
void cria_fundo_pausa(ALLEGRO_DISPLAY* disp, ALLEGRO_BITMAP* bitmap) {
    al_set_target_bitmap(bitmap);

    int width = al_get_display_width(disp);
    int height = al_get_display_height(disp);
    ALLEGRO_COLOR pretoTransparente = al_map_rgba(0, 0, 0, 900);
    al_draw_filled_rectangle(0, 0, width, height, pretoTransparente);

    al_set_target_bitmap(al_get_backbuffer(disp));
}

void desenhar_caixa_dialogo(int caixaX, int caixaY, int caixaLargura, int caixaAltura,
    ALLEGRO_FONT* font, const char textos[NUM_TEXTO][MAX_TEXTO],
    int* indice_texto, ALLEGRO_EVENT_QUEUE* event_queue,
    bool* dialogo, ALLEGRO_BITMAP* img) {

    // Limita o valor de indice_texto
    if (*indice_texto >= NUM_TEXTO) {
        *indice_texto = NUM_TEXTO - 1;
    }



    // Desenha a caixa de diálogo preenchida
    al_draw_filled_rectangle(caixaX, caixaY, caixaX + caixaLargura, caixaY + caixaAltura, al_map_rgb(192, 192, 192));

    // Desenha a borda
    al_draw_rectangle(caixaX, caixaY, caixaX + caixaLargura, caixaY + caixaAltura, al_map_rgb(255, 255, 255), 2);

    // Desenha o header
    al_draw_filled_rectangle(caixaX, caixaY, caixaX + caixaLargura, caixaY + 30, al_map_rgb(23, 0, 209));

    // Coordenadas da imagem
    int imgX = caixaX + 20;
    int imgY = caixaY + 40;

    // Desenha a imagem
    al_draw_scaled_bitmap(
        img, 0, 0,
        al_get_bitmap_width(img), al_get_bitmap_height(img),
        imgX, imgY, 100, 100, 0
    );

    // Texto atual
    const char* texto_atual = textos[*indice_texto];

    // Ajuste de linha para o texto
    int linhaY = imgY + 50;
    int espacoVertical = 10;  // Espaço entre as linhas do texto

    // Quebra de linha automática
    int largura_maxima = caixaLargura - (imgX + 120); // Largura disponível para o texto
    char texto_buffer[MAX_TEXTO];
    int pos_inicial = 0;

    while (pos_inicial < strlen(texto_atual)) {
        int comprimento_texto = 0;

        // Encontra a posição do texto que cabe na linha
        while (comprimento_texto < largura_maxima && texto_atual[pos_inicial + comprimento_texto] != '\0') {
            comprimento_texto++;
        }

        // Copia o texto que cabe na linha

        strncpy_s(texto_buffer, sizeof(texto_buffer), texto_atual + pos_inicial, comprimento_texto);


        // Desenha o texto
        al_draw_text(fonte_texto, al_map_rgb(0, 0, 0), imgX + 120, linhaY, 0, texto_buffer);
        linhaY += al_get_font_line_height(fonte_texto) + espacoVertical;
        pos_inicial += comprimento_texto;
    }

    // Botão "Avançar"
    int largura_botao = 80;
    int altura_botao = 30;
    int botaoX = caixaX + caixaLargura - largura_botao - 20;
    int botaoY = caixaY + caixaAltura - altura_botao - 10;

    al_draw_filled_rectangle(botaoX, botaoY, botaoX + largura_botao, botaoY + altura_botao, al_map_rgb(100, 100, 100));
    al_draw_rectangle(botaoX, botaoY, botaoX + largura_botao, botaoY + altura_botao, al_map_rgb(255, 255, 255), 2);

    const char* texto_botao = "Avançar";
    al_draw_text(font, al_map_rgb(255, 255, 255), botaoX + largura_botao / 2, botaoY + altura_botao / 2 - al_get_font_line_height(font) / 2, ALLEGRO_ALIGN_CENTER, texto_botao);

    // Processa eventos de clique no botão
    while (al_get_next_event(event_queue, &ev)) {
        if (mouse_click()) {
            if (ev.mouse.x > botaoX && ev.mouse.x < botaoX + largura_botao &&
                ev.mouse.y > botaoY && ev.mouse.y < botaoY + altura_botao) {
                *indice_texto += 1;

                // Se o índice exceder o número de textos disponíveis, finaliza o diálogo
                if (*indice_texto >= NUM_TEXTO) {
                    *dialogo = false;
                    return;
                }
            }
        }
    }
}





// Funções de movimento da mão - Ataque Mosquito
float movimento_senoidal(float x, float amplitude) {
    return (DISPLAY_HEIGHT / 2) + amplitude * sin((x / DISPLAY_WIDTH) * (2 * PI)) * 0.5; // Reduz a amplitude para não sair da tela
}
float movimento_cosenoidal(float x, float amplitude) {
    return (DISPLAY_HEIGHT / 2) + amplitude * cos((x / DISPLAY_WIDTH) * (2 * PI)) * 0.5; // Reduz a amplitude para não sair da tela
}
float movimento_inverso_senoidal(float x, float amplitude) {
    return (DISPLAY_HEIGHT / 2) - amplitude * sin((x / DISPLAY_WIDTH) * (2 * PI)) * 0.5;
}
float movimento_inverso_cosenoidal(float x, float amplitude) {
    return (DISPLAY_HEIGHT / 2) - amplitude * cos((x / DISPLAY_WIDTH) * (2 * PI)) * 0.5;
}

float (*padrao_movimento[4])(float, float) = {
    movimento_senoidal,
    movimento_cosenoidal,
    movimento_inverso_senoidal,
    movimento_inverso_cosenoidal
};



//FUNÇÕES FAGOCITOSE


//recebe um par ordenado e retorna em qual posição ele está na grade de fundo (como se fosse um array unidimensional)
int pega_posicao_grid(int x, int y) {
    return ((x - (x % ESPACO_ENTRE_GRADE)) / ESPACO_ENTRE_GRADE) + ((y - (y % ESPACO_ENTRE_GRADE)) / ESPACO_ENTRE_GRADE) * NUM_COLUNAS;
}

//define uma posição aleatória para cada nutriente
void mapear_nutrientes(nutriente* pNutrientes, const int raio_nutriente, int iNutriente, short int(*pGrid_map)[10], short int* pQtd_nutrientes) {
    pNutrientes->x = posicao_aleatoria(DISPLAY_WIDTH, raio_nutriente * 2);
    pNutrientes->y = posicao_aleatoria(DISPLAY_HEIGHT, raio_nutriente * 2);

    //mapeia cada posição nova de nutriente para um espaço na grid
    int posicao_na_grid = pega_posicao_grid(pNutrientes->x, pNutrientes->y);
    pQtd_nutrientes[posicao_na_grid]++;
    pGrid_map[posicao_na_grid][pQtd_nutrientes[posicao_na_grid]] = iNutriente; //no espaço X na grid, envia o index do nutriente
}

void colisao_player_nutriente(player_fagocitose* pPlayer_fago, nutriente* pNutrientes, int raio_nutriente, short int* pQtd_nutrientes, short int(*pGrid_map)[10], int index_nutriente, controle_fagocitose* pControl_fago) {
    if ((index_nutriente >= 0 && index_nutriente <= 100) && colisao_circulo_dentro(pNutrientes[index_nutriente].x, pNutrientes[index_nutriente].y, raio_nutriente, pPlayer_fago->x, pPlayer_fago->y, pPlayer_fago->raio)) {
        pPlayer_fago->raio++; //aumenta tamanho do player
        pControl_fago->pontuacao += 10;

        // descobre qual espaço o nutriente  está na grid e o remove de lá
        int posicao_nutriente_grid = pega_posicao_grid(pNutrientes[index_nutriente].x, pNutrientes[index_nutriente].y);
        pQtd_nutrientes[posicao_nutriente_grid]--;
        // remove o index do nutriente do gridMap
        for (int l = 0; l <= pQtd_nutrientes[posicao_nutriente_grid]; l++) {
            if (pGrid_map[posicao_nutriente_grid][l] == index_nutriente) {
                // move todos os elementos daquele espaço na grid um pra esquerda
                for (int m = l; m < pQtd_nutrientes[posicao_nutriente_grid] + 1; m++) {
                    pGrid_map[posicao_nutriente_grid][m] = pGrid_map[posicao_nutriente_grid][m + 1];
                }
                break;
            }
        }
        // Atualiza a posição do nutriente na grid
        mapear_nutrientes(&pNutrientes[index_nutriente], raio_nutriente, index_nutriente, pGrid_map, pQtd_nutrientes);
    }
}

void checa_colisao_vizinhos(short int(*pGrid_map)[10], short int* pQtd_nutrientes, nutriente* pNutrientes, int raio_nutriente, player_fagocitose* pPlayer_fago, int posicao_player_grid, controle_fagocitose* pControl_fago) {
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
            int pos = posicao_player_grid + i + j * NUM_COLUNAS;
            //garante que nunca vai checar um espaço nulo (out of bounds)
            if (pos >= 0 && pos < NUM_CELULAS_GRID) {
                //se houver nutrientes em um espaço da grid
                if (pQtd_nutrientes[pos] > 0) {
                    //para cada nutriente em um espaço na grid
                    for (int k = 0; k <= pQtd_nutrientes[pos]; k++) {
                        int index_nutriente = pGrid_map[pos][k];
                        colisao_player_nutriente(pPlayer_fago, pNutrientes, raio_nutriente, pQtd_nutrientes, pGrid_map, index_nutriente, pControl_fago);
                    }
                }
            }
        }
    }
}

//tudo que for "desenhado" aqui dentro vai ser transformado em um novo bitmap
void criar_bg_fagocitose(ALLEGRO_DISPLAY* display, ALLEGRO_BITMAP* bitmap, ALLEGRO_COLOR cor_fundo, int espacamento, ALLEGRO_COLOR cor_grade, int grossura_linha) {
    al_set_target_bitmap(bitmap);
    al_clear_to_color(cor_fundo);
    for (int i = 1; i < DISPLAY_WIDTH / espacamento; i++) {
        al_draw_line(i * espacamento, 0, i * espacamento, DISPLAY_HEIGHT, cor_grade, grossura_linha);    //desenha a linha
        for (int j = 1; j < DISPLAY_HEIGHT / espacamento; j++) {
            al_draw_line(0, j * espacamento, DISPLAY_WIDTH, j * espacamento, cor_grade, grossura_linha); //desenha a coluna
        }
    }
    al_set_target_bitmap(al_get_backbuffer(display));
}

//FUNÇÕES VIREMIA
/*Função que gera coordenadas de X aleatorias com base no tamanho do display,
  preenchendo um array. Onde a primeira e a última posição já tem coordenadas
  pré estabelecidas*/
void gera_coordenadas_X(int* coordenadaX, int tamanho, int x1, int x2) {
    coordenadaX[0] = x1;
    coordenadaX[tamanho - 1] = x2;
    for (int i = 1; i < tamanho - 1; i++) {
        coordenadaX[i] = 20 + rand() % (DISPLAY_WIDTH - 40);
    }
}

/*Função que gera coordenadas de Y aleatorias com base no tamanho do display,
  preenchendo um array. Onde a primeira e a última posição já tem coordenadas
  pré estabelecidas*/
void gera_coordenadas_Y(int* coordenadaY, int tamanho, int y1, int y2) {
    coordenadaY[0] = y1;
    coordenadaY[tamanho - 1] = y2;
    for (int i = 1; i < tamanho - 1; i++) {
        coordenadaY[i] = 20 + rand() % (DISPLAY_HEIGHT - 40);
    }
}

//Função que gera linhas curvadas e colineares de acordo com as coordenadas que confam o caminho 
void gerar_Linhas(int* coordenadaX, int* coordenadaY, int tamanho, int espessuraLinha) {
    for (int i = 0; i < tamanho - 1; i++) {
        al_draw_rounded_rectangle(coordenadaX[i], coordenadaY[i],
            coordenadaX[i + 1], coordenadaY[i + 1],
            espessuraLinha / 2, espessuraLinha / 2,
            al_map_rgb(119, 25, 15), espessuraLinha);
    }
}


// Função para verificar se o círculo está sobre uma linha com espessura
bool fora_Da_Linha(float x, float y, float x1, float y1, float x2, float y2, float espessura) {
    // Calcular a distância perpendicular do ponto à reta
    float A = y2 - y1;
    float B = x1 - x2;
    float C = x2 * y1 - x1 * y2;
    float distancia = fabs(A * x + B * y + C) / sqrt(A * A + B * B);

    return distancia > espessura / 2;
}

//função que cria linhas onduladas por meio do calculo da Função Seno
void linhas_Onduladas(float x1, float y1, float x2, float y2, int qtdOndas) {
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

int pontuacao_viremia(int tempo) {
    return 250000 / tempo;
}

//Função que desenha uma série de imagens na tela, substituindo-as por outras, de forma sequencial, de acordo com um valor de "vida"
void atualiza_vidas(ALLEGRO_BITMAP* img1, ALLEGRO_BITMAP* img2, ALLEGRO_BITMAP* img3,
    ALLEGRO_BITMAP* img1_subs, ALLEGRO_BITMAP* img2_subs, ALLEGRO_BITMAP* img3_subs,
    int vida, int x, int y) {

    // Verifica se a terceira imagem deve ser substituída
    if (vida <= 2) {
        al_draw_bitmap(img3_subs, x, y, 0);
    }
    else {
        al_draw_bitmap(img3, x, y, 0);
    }

    // Verifica se a segunda imagem deve ser substituída e se a terceira já foi substituída
    if (vida <= 1) {
        al_draw_bitmap(img2_subs, x - 50, y, 0);
    }
    else {
        al_draw_bitmap(img2, x - 50, y, 0);
    }

    // Verifica se a primeira imagem deve ser substituída e se a segunda já foi substituída
    if (vida == 0) {
        al_draw_bitmap(img1_subs, x - 100, y, 0);
    }
    else {
        al_draw_bitmap(img1, x - 100, y, 0);
    }
}

void init_sample(){
    //Audio
    colisao = al_load_sample("sound/colisao.wav");
    gameover = al_load_sample("sound/gameover.wav");
    morreu = al_load_sample("sound/morreu.wav");
    next_level = al_load_sample("sound/next_level.wav");
    trilha_sonora = NULL;
    viremia_som = NULL;
    mosquito = NULL;
}

void init_bitmaps() {
    loading = al_load_bitmap("img/menus/telaLoading.png");
    background_desktop = al_load_bitmap("img/menus/bgDesktop.png");
    botao_play = al_load_bitmap("img/menus/Controle jogar.png");
    nextlevel = al_load_bitmap("img/menus/nextlevel.png");
    background_fases = al_load_bitmap("img/menus/bgFases.png");
    virus_viremia = al_load_bitmap("img/menus/virus.png");
    tela_perdeu = al_load_bitmap("img/menus/telaPerdeu.png");
    tela_tutorial = al_load_bitmap("img/menus/telaTutorial.png");


    //QUADRINHOS
        //ATAQUE MOSQUITO
    quadrinhos_HQ[0][0] = al_load_bitmap("img/HQ/ataque_mosquito1.png");
    quadrinhos_HQ[0][1] = al_load_bitmap("img/HQ/ataque_mosquito2.png");
    quadrinhos_HQ[0][2] = al_load_bitmap("img/HQ/ataque_mosquito3.png");
    quadrinhos_HQ[0][3] = al_load_bitmap("img/HQ/ataque_mosquito4.png");
        //FAGOCITOSE
    quadrinhos_HQ[1][0] = al_load_bitmap("img/HQ/fagocitose1.png");
    quadrinhos_HQ[1][1] = al_load_bitmap("img/HQ/fagocitose2.png");
    quadrinhos_HQ[1][2] = al_load_bitmap("img/HQ/fagocitose3.png");
    quadrinhos_HQ[1][3] = al_load_bitmap("img/HQ/fagocitose4.png");
        //VIREMIA
    quadrinhos_HQ[2][0] = al_load_bitmap("img/HQ/viremia1.png");
    quadrinhos_HQ[2][1] = al_load_bitmap("img/HQ/viremia2.png");
    quadrinhos_HQ[2][2] = al_load_bitmap("img/HQ/viremia3.png");
    quadrinhos_HQ[2][3] = al_load_bitmap("img/HQ/viremia4.png");

    mouse_tutorial = al_load_bitmap("img/menus/mouse.png");
    spray_tutorial = al_load_bitmap("img/menus/spray_tutorial.png");
    virus_PB = al_load_bitmap("img/menus/virusP&B.png");
    mosquitao = al_load_bitmap("img/estrofulo/mosquitao.png");
    iconmosquitao = al_load_bitmap("img/estrofulo/iconmosquitao.png");
    teia_img = al_load_bitmap("img/estrofulo/teia.png");
    spray_img = al_load_bitmap("img/estrofulo/spray.png");
    background_estrofulo = al_load_bitmap("img/estrofulo/fundoestrofulo.png");
    background_viremia = al_load_bitmap("img/viremia/backgroundViremia.png");
    cd8_viremia = al_load_bitmap("img/viremia/cd8Viremia.png");
    celula_viremia = al_load_bitmap("img/viremia/player_viremia.png");
    mao_img = al_load_bitmap("img/estrofulo/maofrente.png");
    bonus_img = al_load_bitmap("img/estrofulo/raio.png");
   
    bg_venceu_jogo = al_load_bitmap("img/menus/menu_vitoria.png");
    dialogo_virus = al_load_bitmap("img/estrofulo/viruss.png");
    dialogo_mosquito = al_load_bitmap("img/estrofulo/moquitoo.png");

    bg_pausa = al_create_bitmap(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    cria_fundo_pausa(display, bg_pausa);
    //cria "imagem" pra desenhar no fundo do jogo (fagocitose)
    bg_fagocitose_bitmap = al_create_bitmap(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    criar_bg_fagocitose(display, bg_fagocitose_bitmap, WHITE, ESPACO_ENTRE_GRADE, GRAY, 2);
    paintogenico = al_create_bitmap(271, 260);
    cria_paintogenico_bitmap();
}

//SETUP DAS TELAS

void init_ataque_mosquito() {
    for(int i = 0; i < 4; i++)
        controle.pontuacao[i] = 0;
    controle.pontuacao_total = 0;
    
    player_mosquito.x = 600;
    player_mosquito.y = 300;
    player_mosquito.raio = al_get_bitmap_width(mosquitao) / 2;
    player_mosquito.vivo = true;
    player_mosquito.pontuacao = 0;

    teia.x = DISPLAY_WIDTH;
    teia.y = rand() % (DISPLAY_HEIGHT - 20) + 10;
    teia.raio = 5;

    mao.x = 0; // Posição fixa em X
    mao.y = rand() % (DISPLAY_HEIGHT - 20) + 10; // Posição inicial aleatória em Y
    mao.raio = al_get_bitmap_width(mao_img)/2;
    
    timer_invencibilidade = 0;

   // VARIAVEIS BONUS, NAO ESTA ADICIONANDO AO SISTEMA DE PONTOS, DE RESTO TUDO OK 
    bonus_estrofulo.x = DISPLAY_WIDTH + 50;
    bonus_estrofulo.y = rand() % (DISPLAY_HEIGHT - 30) + 10;
    bonus_estrofulo.raio = 5;
    
    cont_bonus = 0; //conta qnts bonus foram pegos pra calcular no final a pontuacao

    spray.x = 50; // posicao inicial e fixa de X do spray
    spray.y = 220; //posicao inicial Y do spray
    spray.raio = al_get_bitmap_width(spray_img) / 2;

    velocidade_x = (rand() % 6 + 5);
    suavidade = 0.020;
    tempo_segundos = 0.0;
    tempo_max = 20.0;

    // Inicializar a amplitude e o padrão de movimento
    amplitude = rand() % (DISPLAY_HEIGHT / 2) + (DISPLAY_HEIGHT / 2); // Amplitude
    indice_padroes = 0;

}

void init_fagocitose() {
    //inicializa array q marca quantos nutrientes tem em cada espaço na grid
    for (int i = 0; i < NUM_CELULAS_GRID; i++) {
        qtd_nutrientes_grid[i] = 0;
    }

    nutrientes = (nutriente*)malloc(num_nutrientes * sizeof(nutriente)); // array dinâmico de nutrientes
    for (int i = 0; i < num_nutrientes; i++) {
        mapear_nutrientes(&nutrientes[i], raio_nutriente, i, grid_map, qtd_nutrientes_grid);  //define um valor aleatório para cada nutriente no mapa
    }

    //setup dos dados do minigame
    control_fago.pontuacao = 0;
    control_fago.morto = false;
    control_fago.venceu = false;
    control_fago.desenhou_fundo = false;
    control_fago.tentativas = 3;
    control_fago.cont_vezes_crescidas = 0; //marca qnts vezes o player chegou no tamanho máx
    timer_invencibilidade = 0;
    //define como o círculo do player spawna no mapa
    player_fago.x = DISPLAY_WIDTH / 2;
    player_fago.y = DISPLAY_HEIGHT / 2;
    player_fago.vel = 5;
    player_fago.raio_min = raio_nutriente * 2;
    player_fago.raio_max = 75;
    player_fago.raio = player_fago.raio_min;

    //define como o círculo do fagocito pong spawna no mapa
    fago_pong.raio = player_fago.raio_max;
    fago_pong.x = posicao_aleatoria(DISPLAY_WIDTH, fago_pong.raio * 2);
    fago_pong.y = posicao_aleatoria(DISPLAY_HEIGHT, fago_pong.raio * 2);
    fago_pong.vel = 7;
    pong_velX = 1;
    pong_velY = 1;
}

void init_viremia() {
    controle.pontuacao[2] = 0;
    x_checkpoint1 = 50; y_checkpoint1 = 535;
    x_checkpoint2 = 740; y_checkpoint2 = 50;

    // Posição inicial do círculo player
    player_vire.x = x_checkpoint1 - 5;
    player_vire.y = y_checkpoint1 - 5;
    player_vire.diferenca = 15;

    pode_seguir_mouse = false;
    startJogo = false;
    mudou_de_nivel_viremia = false;
    nivel_viremia = 1;
    
    tamanho = 4;
    espessura_linha = 18.0;
    dentro_da_linha = false;

    //vidas
    invulneravel = false;
    tempo_invulnerabilidade = 120; // invulnerabilidade de acordo com a duração desejada (em frames)
    contador_invulnerabilidade = 0;

    // Inicializar as imagens
    for (int i = 0; i <= 2; i++) {
        vidas_viremia[i] = al_clone_bitmap(virus_viremia);
    }
    for (int i = 3; i <= 5; i++) {
        vidas_viremia[i] = al_clone_bitmap(virus_PB);
    }

    vidas_disponiveis_viremia = 3;
    vida_viremia_x = 150;
    vida_viremia_y = 5;

    cronometro = 0;
    cronometro_reset = 0; // para o cronometro do viremia 
    pontos_viremia = 0;


    // Aloca memória para as coordenadas
    coordenada_X = (int*)malloc(tamanho * sizeof(int));
    coordenada_Y = (int*)malloc(tamanho * sizeof(int));

    // Chama a função para preencher o array com números aleatórios
    gera_coordenadas_X(coordenada_X, tamanho, x_checkpoint1, x_checkpoint2);
    gera_coordenadas_Y(coordenada_Y, tamanho, y_checkpoint1, y_checkpoint2);

    angulo_viremia = 0; //variável de ângulo será incrementada a cada frame para simular o movimento circular
    raio_viremia = 50;

    // Criar um array para armazenar as informações das imagens
    linfocito_CD8 = (obstaculo_viremia*)malloc(quantidade_CD8 * sizeof(obstaculo_viremia));

    // Inicializar as imagens
    for (int i = 0; i < quantidade_CD8; i++) {
        linfocito_CD8[i].cd8_viremia = al_clone_bitmap(cd8_viremia);
        linfocito_CD8[i].x = -((2 * al_get_bitmap_width(cd8_viremia)) * i);
        // Coordenada Y inicial entre 340 e 600
        linfocito_CD8[i].y = 340 + rand() % (600 - 340 + 1);
        linfocito_CD8[i].velocidade = 0.5 + (float)i / 10.0;
        // Direção inicial para baixo (para garantir que as imagens comecem descendo)
        linfocito_CD8[i].direcao = 1;
    }

}


void ir_para_tela(int jogoDestino) {
    switch (jogoDestino) {
    case HQ:
        index_HQ++;
        for (int i = 0; i < 4; i++)
            opacidade_HQs[i] = 0;
        quadrinho_atual = 0;
        tela = HQ;
        break;

    case TUTORIAL_ESTROFULO:
        tela = TUTORIAL_ESTROFULO;
        break;

    case TUTORIAL_FAGOCITOSE:
        tela = TUTORIAL_FAGOCITOSE;
        break;

    case TUTORIAL_VIREMIA:
        tela = TUTORIAL_VIREMIA;
        break;

    case ATAQUE_MOSQUITO:
        tempo_perdeu = 0;
        init_ataque_mosquito();
        tela = ATAQUE_MOSQUITO;
        break;

    case FAGOCITOSE:
        tempo_perdeu = 0;
        init_fagocitose();
        tela = FAGOCITOSE;
        
        break;

    case VIREMIA:
        tempo_perdeu = 0;
        init_viremia();
        tela = VIREMIA;
        break;

    case TELA_INICIAL:
        index_HQ = -1;
        al_set_timer_count(timer, 0);
        tela = TELA_INICIAL;
        break;

    default:
        tela = VENCEU_VIREMIA;
        break;
    }
}

// chamada de exemplo: popup_vitoria(fonte_HUD, nextlevel, &tela);
void popup_vitoria(ALLEGRO_FONT* fonte, ALLEGRO_BITMAP* botao, int* tela) {
    pause();
    pode_pausar = false;
    int proxTela;
    int index_pontuacao;
    switch (*tela) {
    case ATAQUE_MOSQUITO:
        index_pontuacao = 0;
        proxTela = HQ;
        break;
    case FAGOCITOSE:
        index_pontuacao = 1;
        proxTela = HQ;
        break;
    case VIREMIA:
        index_pontuacao = 2;
        proxTela = VENCEU_VIREMIA;
        break;
        //garantia de que o jogo não vai crashar, mas em tese esse caso nunca vai acontecer
    default:
        proxTela = *tela;
    }
    if (ev.type == ALLEGRO_EVENT_TIMER) {
        controle.pontuacao_total += controle.pontuacao[index_pontuacao];
    }

    static int popup_width = 300;
    static int popup_height = 300;
    static int popup_x = DISPLAY_WIDTH / 2 - popup_width / 2;
    static int popup_y = DISPLAY_HEIGHT / 2 - popup_height / 2;

    static int margem = 30;

    static int botao_width = al_get_bitmap_width(botao);
    static int botao_height = al_get_bitmap_height(botao);
    static int botao_x = popup_x + (popup_width - botao_width) / 2 + 10;
    static int botao_y = popup_y + popup_height - botao_height - margem;

    if (colisao_mouse(mState, botao_x, botao_y, botao_width, botao_height) && mouse_click()) {
        ir_para_tela(proxTela);
        controle.venceu_jogo = false;
        unpause();
    }

    //
    //DESENHO
    //
   // fundo popup
    al_draw_filled_rectangle(popup_x, popup_y, popup_x + popup_width, popup_y + popup_height, al_map_rgb(192, 192, 192));

    // borda popup
    al_draw_rectangle(popup_x, popup_y, popup_x + popup_width, popup_y + popup_height, al_map_rgb(255, 255, 255), 2);

    // cabeçalho popup
    al_draw_filled_rectangle(popup_x, popup_y, popup_x + popup_width, popup_y + 50, al_map_rgb(23, 0, 209));

    // título do popup
    al_draw_textf(fonte, al_map_rgb(255, 255, 255), popup_x + popup_width / 2, popup_y + 5, ALLEGRO_ALIGN_CENTER, "Fase Concluída!");

    // pontuação e etc
    al_draw_textf(fonte, al_map_rgb(0, 0, 0), popup_x + margem, popup_y + 60, ALLEGRO_ALIGN_LEFT, "Pontuação: %d", controle.pontuacao[index_pontuacao]);
    al_draw_textf(fonte, al_map_rgb(0, 0, 0), popup_x + margem, popup_y + 100, ALLEGRO_ALIGN_LEFT, "Pontuação total: %d", controle.pontuacao_total);

    // botão
    al_draw_bitmap(botao, botao_x, botao_y, 0);
}


//cria um botão que, ao ser clicado, volta para o menu
void voltarTelaEscolha(ALLEGRO_EVENT ev, int* tela, ALLEGRO_FONT* fonte_20, bool* jogo_pausa) {

    char msg[] = { "SAIR" };
    char botao_voltar[] = { "VOLTAR" };

    int margem_menu = 20;

    // Coordenadas e dimensões do texto "VOLTAR"
    int larguraTexto = al_get_text_width(fonte_20, msg);
    int larguraTexto2 = al_get_text_width(fonte_20, botao_voltar);
    int alturaTexto = al_get_font_line_height(fonte_20);
    int botao_voltar_x = DISPLAY_WIDTH / 2 - larguraTexto2 / 2;
    int botao_voltar_y = DISPLAY_HEIGHT / 2 - alturaTexto / 2 - margem_menu;
    int textoX = DISPLAY_WIDTH / 2 - larguraTexto / 2;
    int textoY = DISPLAY_HEIGHT / 2 + alturaTexto / 2 + margem_menu;
    
    //desenha botao de "VOLTAR"
    al_draw_text(fonte_20, WHITE, botao_voltar_x, botao_voltar_y, 0, botao_voltar);
    // Desenha o texto "SAIR"
    al_draw_text(fonte_20, WHITE, textoX, textoY, 0, msg);


    // Verifica se houve um clique do mouse
    if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
        if (ev.mouse.button == 1) { // Verifica se o botão esquerdo foi clicado
            // Verifica se o clique foi dentro da área do texto "SAIR"
            if (colisao_mouse(mState, textoX, textoY, larguraTexto, alturaTexto)) {
                ir_para_tela(2);
                *jogo_pausa = false;
                unpause();
            }
            if (colisao_mouse(mState, botao_voltar_x, botao_voltar_y, larguraTexto2, alturaTexto)) {
                *jogo_pausa = false;
                unpause();
            }
            // Exibe as coordenadas do clique no console
            printf("Clique detectado na coordenada escolha (%d, %d)\n", ev.mouse.x, ev.mouse.y);
        }
    }
}

void mostra_HQ() {
    int hq_size = al_get_bitmap_width(quadrinhos_HQ[0][0]);
    int hq_margem = 15; //levar em consideração que a margem é sempre aplicada 2 vezes
    int x1 = DISPLAY_WIDTH / 2 - hq_size - hq_margem;
    int y1 = DISPLAY_HEIGHT / 2 - hq_size - hq_margem;
    int x2 = DISPLAY_WIDTH / 2 + hq_margem;
    int y2 = DISPLAY_HEIGHT / 2 + hq_margem;

    int quadrinhos_x[2];
    int quadrinhos_y[2];
    quadrinhos_x[0] = DISPLAY_WIDTH / 2 - hq_size - hq_margem;
    quadrinhos_x[1] = DISPLAY_WIDTH / 2 + hq_margem;
    quadrinhos_y[0] = DISPLAY_HEIGHT / 2 - hq_size - hq_margem;
    quadrinhos_y[1] = DISPLAY_HEIGHT / 2 + hq_margem;

    int borda_grossura = 7;
    int borda_x;
    int borda_y;
    int borda_size = hq_size + borda_grossura / 2;

    char nome_fase[3][30] = { "FASE %d - ESTROFULO",
                              "FASE %d - FAGOCITOSE",
                              "FASE %d - VIREMIA" };

    //controla qual será a próx tela/ qual HQ mostrar
    int prox_tela = 0;
    switch (index_HQ) {
    case 1:
        prox_tela = TUTORIAL_FAGOCITOSE;
        break;
    case 2:
        prox_tela = TUTORIAL_VIREMIA;
        break;
    default:
        index_HQ = 0;
        prox_tela = TUTORIAL_ESTROFULO;
        break;
    }
    //botao de prosseguir
    int botao_continuar_margem = 30;
    int botao_continuar_size = al_get_bitmap_width(botao_play);
    int botao_continuar_x = DISPLAY_WIDTH - botao_continuar_margem - botao_continuar_size;
    int botao_continuar_y = DISPLAY_HEIGHT - botao_continuar_margem - botao_continuar_size;

    //LOGICA
    if (colisao_mouse(mState, botao_continuar_x, botao_continuar_y, botao_continuar_size, botao_continuar_size) && //botou mouse em cima do botao
        (ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && ev.mouse.button == 1)) {                                               //clicou
        ir_para_tela(prox_tela);
    }

    if (ev.type == ALLEGRO_EVENT_TIMER) {

        //controla animação de cada quadrinho surgindo
        float velocidade_opacidade = 0.02;
        if (quadrinho_atual < 4) { //garante q nao vai ultrapassar limite de quadrinhos (0 a 3)
            opacidade_HQs[quadrinho_atual] += velocidade_opacidade;

            if (opacidade_HQs[quadrinho_atual] > 1.0) { //quando atingir 100%
                opacidade_HQs[quadrinho_atual] = 1.0;    //garante que nao ultrapassa 100%
                quadrinho_atual++;
            }
        }

        //DESENHO
        al_clear_to_color(BLACK);

        //al_draw_text(fonte_texto, WHITE, DISPLAY_WIDTH / 2, quadrinhos_y[0] - 40, ALLEGRO_ALIGN_CENTER, "Fase %d !", index_HQ+1);
        al_draw_textf(fonte_texto, WHITE, DISPLAY_WIDTH / 2, quadrinhos_y[0] - 40, ALLEGRO_ALIGN_CENTER, nome_fase[index_HQ], index_HQ + 1);

        // desenha quadrinhos e bordas
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                int indice = i + (2 * j); // calcula o índice do quadrinho (0 a 3)
                borda_x = quadrinhos_x[i];
                borda_y = quadrinhos_y[j];

                //bordas
                al_draw_rectangle(
                    borda_x - borda_grossura / 2, // x inicial
                    borda_y - borda_grossura / 2, // y inicial
                    borda_x + borda_size,         // x final
                    borda_y + borda_size,         // y final
                    WHITE, borda_grossura
                );

                //quadrinhos
                al_draw_tinted_bitmap(
                    quadrinhos_HQ[index_HQ][indice],
                    al_map_rgba_f(opacidade_HQs[indice], opacidade_HQs[indice], opacidade_HQs[indice], opacidade_HQs[indice]),
                    quadrinhos_x[i],
                    quadrinhos_y[j],
                    0
                );
            }
        }
        //botao p/ continuar
        criar_bitmap_botao(botao_play, fonte_texto, "Continuar", WHITE,0, SUL, botao_continuar_x, botao_continuar_y - 20);
    }
}


int main() {
    //atribui uma seed aleatória. Caso o jogo crashe ou algo do tipo, essa é uma boa forma de debug
    //tutorial de como usar a seed:
        //ela sempre será imprimida no terminal no começo do jogo.
        //se crashar, pega a seed e coloca ela no lugar de time(NULL);
        //depois que debugar, volta a seed pro time(NULL);   -joao
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
    al_install_keyboard();
    al_install_audio();
    al_init_acodec_addon();

    // Cria uma janela
    display = al_create_display(DISPLAY_WIDTH, DISPLAY_HEIGHT);

    // define o FPS

    timer = al_create_timer(1.0 / 60.0);
    countdown_timer = al_create_timer(1.0);

    //fontes de texto
    fonte_HUD = al_load_ttf_font("font/fonteWindowsRegular.ttf", 30, 0);
    font = al_create_builtin_font();
    fonte_20 = al_load_font("font/fonte.TTF", 20, 0);
    fonte_texto = al_load_font("font/fonteWindowsRegular.ttf", 25, 0);
    fonte_titulo = al_load_ttf_font("font/fonteWindowsRegular.ttf", 50, 0);

    // imagens
    init_bitmaps();

    //audio
    init_sample();

    //cria o mixer (e torna ele o mixer padrao), e adciona 5 samples de audio nele
    if (!al_reserve_samples(7)) {
        printf("Falha ao reservar amostrar de audio");
        return 0;
    }
    
    viremia_som = al_load_audio_stream("sound/viremia.ogg", 4, 1024);
    trilha_sonora = al_load_audio_stream("sound/trilha_sonora.ogg", 4, 1024);
    mosquito = al_load_audio_stream("sound/mosquito.wav", 4, 1024);
    //liga o stream no mixer
    al_attach_audio_stream_to_mixer(viremia_som, al_get_default_mixer());
    al_attach_audio_stream_to_mixer(trilha_sonora, al_get_default_mixer());
    al_attach_audio_stream_to_mixer(mosquito, al_get_default_mixer());


    //define que o stream vai tocar no modo repeat
    al_set_audio_stream_playmode(trilha_sonora, ALLEGRO_PLAYMODE_LOOP);
    al_set_audio_stream_playmode(mosquito, ALLEGRO_PLAYMODE_LOOP);
    al_set_audio_stream_playmode(viremia_som, ALLEGRO_PLAYMODE_LOOP);
    
    //Volume do áudio
    al_set_audio_stream_gain(trilha_sonora, 0.5);
    al_set_audio_stream_gain(viremia_som, 5.0);
    al_set_audio_stream_gain(mosquito, 8.0);
    
    ALLEGRO_BITMAP* teclas_tutorial = al_load_bitmap("img/menus/teclas_tutorial.png");
    printf("\n\n");

    // Cria uma fila de eventos
    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();

    // Registra a fila de eventos com as fontes de eventos (display, timer, mouse)
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_mouse_event_source());
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(countdown_timer));

    al_start_timer(timer);
    al_start_timer(countdown_timer);

    /*****FIM SETUP DO ALLEGRO*****/

    // - - - - - - - VARIÁVEIS GERAIS - - - - - - -


    bool jogo_pausado = false;
    int tela_anterior = 0;
    tempo_perdeu = 0;
    pode_pausar = false;
    tela = TELA_LOADING;
    tela = VIREMIA;
    //setup do controle geral do jogo (struct q guarda pontuação, estado do jogo, etc)
    controle.venceu_jogo = false;
    controle.pontuacao_total = 0;
    for (int i = 0; i < 2; i++)
        controle.pontuacao[i] = 0;

    // - - - - - - -FIM DAS VARIÁVEIS GERAIS - - - - - - -


    // - - - - - - - VARIÁVEIS PARA A TELA LOADING - - - - - - -
    int contador_de_frames = 0;
    const int frame_segundo = 60;
    const int intervalo_segundo = 2;

    int opacidade_texto = 255;

    const int width_Loading = 8;
    const int height_Loading = 10;
    const int distancia_Loading = 2; // Distância entre os retângulos

    // Lista de retângulos
    Retangulo retangulos[100];
    int num_retangulos = 1;

    // Primeiro retângulo
    retangulos[0].x_loading = 380;
    retangulos[0].y_loading = 295;
    retangulos[0].w_loading = width_Loading;
    retangulos[0].h_loading = height_Loading;
    //  - - - - - - - FIM DAS VARIÁVEIS PARA A TELA LOADING - - - - - - -
    
    //SETUP DOS JOGOS
    init_ataque_mosquito();
    init_fagocitose();
    init_viremia();
    al_attach_audio_stream_to_mixer(trilha_sonora, al_get_default_mixer());
    al_set_audio_stream_playing(trilha_sonora, true);


    // Loop de eventos
    bool running = true;
    while (running) {
        al_wait_for_event(event_queue, &ev);

        // Verifica se o evento foi o fechamento da janela
        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;  // Sai do loop e encerra o programa
        }

        al_get_mouse_state(&mState);
        al_get_keyboard_state(&kState);

        switch (tela) {

        case TELA_LOADING:
        {
            al_set_audio_stream_playing(viremia_som, 0);
            al_set_audio_stream_playing(mosquito, 0);
            //se apertar espaço pula animação e vai direto pro menu
            if (al_key_down(&kState, ALLEGRO_KEY_SPACE)) {
                tela = TELA_INICIAL;

                // Reproduzir áudio
                al_play_sample(next_level, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
            }
               

            if (ev.type == ALLEGRO_EVENT_TIMER) {
                contador_de_frames++;
                //Verifica o timer a cada meio segundo'
                if (contador_de_frames % (frame_segundo / intervalo_segundo) == 0) {

                    // Criar um novo retângulo
                    if (num_retangulos <= 7) {
                        /*Calcula a posição x do novo retângulo,
                        baseada na posição x do último,
                        adicionando a largura e a distância entre eles.*/
                        retangulos[num_retangulos].x_loading = retangulos[num_retangulos - 1].x_loading + width_Loading + distancia_Loading;
                        retangulos[num_retangulos].y_loading = retangulos[0].y_loading; // Manter a mesma posição
                        retangulos[num_retangulos].w_loading = width_Loading;
                        retangulos[num_retangulos].h_loading = height_Loading;
                        num_retangulos++;
                    }
                    opacidade_texto = opacidade_texto * (-1);
                }
            }

            // Desenha a imagem de fundo na tela (na posição (0, 0))
            al_draw_bitmap(loading, 0, 0, 0);

            // Desenha todos os retângulos armazenados no array retangulos
            for (int i = 0; i < num_retangulos; i++) {
                al_draw_filled_rectangle(retangulos[i].x_loading, retangulos[i].y_loading,
                    retangulos[i].x_loading + retangulos[i].w_loading,
                    retangulos[i].y_loading + retangulos[i].h_loading,
                    BLUE);
            }
            if (num_retangulos >= 8) {
                // Desenha o texto alterando a cor entre preto e branco a cada 2 segundos
                al_draw_textf(fonte_texto, al_map_rgb(opacidade_texto, opacidade_texto, opacidade_texto), 420, 565, ALLEGRO_ALIGN_CENTER, "APERTE A TECLA 'ESPAÇO' PARA INICIAR");
            }
            

        }
        break;

        case TELA_INICIAL:
        {
            // ele faz a caixa de dialogo começar toda vez que vc entrar em uma fase
            tempo_perdeu = 0;

            if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
                if (ev.mouse.button == 1) { // Botão esquerdo do mouse

                    if (ev.mouse.x > 570 && ev.mouse.x < 630 && ev.mouse.y>280 && ev.mouse.y < 320) {
                        ir_para_tela(HQ);
                    }

                    printf("Clique detectado na coordenada (%d, %d)\n", ev.mouse.x, ev.mouse.y);
                }
            }

            // Desenha a imagem de fundo na tela (na posição (0, 0))
            al_draw_bitmap(background_desktop, 0, 0, 0);

            //DEMONSTRAÇÃO DA FUNÇÃO criar_bitmap_botao
            criar_bitmap_botao(botao_play, font, "reveba", WHITE,10, SUL, 300, 250); // chamando a função e desenhando o 'botao_play' na posicao_texto NORTE, passado os parametros de x e y

            al_draw_bitmap(paintogenico,100, 75, 0);

            // Desenhar o texto na tela usando a fonte embutida
            al_draw_text(fonte_20, WHITE, 600, 300, ALLEGRO_ALIGN_CENTER, "jogar");
        }
        break;

        case HQ:
        {
            // Reproduzir áudio
            al_set_audio_stream_playing(mosquito, 0); 

            mostra_HQ();
        }
        break;

        case SELETOR_FASE:
        {
            dialogo = true;

            if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
                if (ev.mouse.button == 1) { // Botão esquerdo do mouse

                    if (ev.mouse.y > 240 && ev.mouse.y < 340) {

                        if (ev.mouse.x > 150 && ev.mouse.x < 230) {

                            tela = TUTORIAL_ESTROFULO;
                        }

                        if (ev.mouse.x > 345 && ev.mouse.x < 437) {

                            tela = TUTORIAL_FAGOCITOSE;
                        }

                        if (ev.mouse.x > 544 && ev.mouse.x < 627) {

                            tela = TUTORIAL_VIREMIA;
                        }
                    }

                    printf("Clique detectado na coordenada (%d, %d)\n", ev.mouse.x, ev.mouse.y);
                }
            }
            al_draw_bitmap(background_fases, 0, 0, 0);
        }
        break;

        case TUTORIAL_ESTROFULO:
        {
            if (ev.type == ALLEGRO_EVENT_TIMER) {
                contador_de_frames++;
                //Verifica o timer a cada meio segundo'
                if (contador_de_frames % (frame_segundo / intervalo_segundo) == 0)
                    opacidade_texto = opacidade_texto * (-1);
            }
            //se apertar espaço pula para o jogo
            if (al_key_down(&kState, ALLEGRO_KEY_SPACE))
                ir_para_tela(ATAQUE_MOSQUITO);
            //Desenhar
            // Desenha a imagem de fundo na tela (na posição (0, 0))
            al_draw_bitmap(background_estrofulo, 0, 0, 0);
            al_draw_bitmap(bg_pausa, 0, 0, 0);
            // Desenha o texto alterando a cor entre preto e branco a cada 2 segundos
            al_draw_textf(fonte_texto, al_map_rgb(opacidade_texto, opacidade_texto, opacidade_texto), 400, 555, ALLEGRO_ALIGN_CENTER, "APERTE A TECLA 'ESPAÇO' PARA CONTINUAR");

            al_draw_bitmap(mosquitao, 372, 213, 0);
            al_draw_line(319, 275, 460, 275, WHITE, 3.0);
            al_draw_bitmap(mao_img, 460, 415, 0);
            al_draw_bitmap(teclas_tutorial, 352, 285, 0);
            al_draw_bitmap(teia_img, 372, 415, 0);
            al_draw_bitmap(spray_tutorial, 292, 415, 0);

        

            //Título
            al_draw_text(fonte_titulo, WHITE, 400, 75, ALLEGRO_ALIGN_CENTER, "COMO JOGAR?");

            //Texto 
            al_draw_text(fonte_texto, WHITE, 50, 167, ALLEGRO_ALIGN_LEFT, "- MISSÃO: DOMINAR A CASA! Controle o mosquito com W e S ou as setas. ");
            al_draw_text(fonte_texto, WHITE, 50, 368, ALLEGRO_ALIGN_LEFT, "- FUJA DOS PERIGOS ABAIXO e explore cada canto da casa!");

            dialogo = true;
        }
        break;

        case ATAQUE_MOSQUITO:
        {
            al_set_audio_stream_playing(mosquito, 1);
            tela_anterior = tela;
            ////   caixa de dialogo
            if (dialogo == true) {

                al_draw_bitmap(bg_pausa, 0, 0, 0);

                char textos[NUM_TEXTO][MAX_TEXTO] = {

                  " Bzzz...",
                  "Se eu não pegar o sangue desse humano..... ",
                  "meus filhos vão morrer...",
                  "Precizzzzzo evitar ser morta até lá!..",
                  " Bzzz.",


                };

                desenhar_caixa_dialogo(100, 200, 600, 200, font, textos, &tempo_perdeu, event_queue, &dialogo, mosquitao);


                // Atualiza a tela e faz outras operações, se necessário
                al_flip_display();

                // Espera um pouco para dar tempo para o tempo ser incrementado
                al_rest(0.1);
            }
            pode_pausar = true;
            if (ev.type == ALLEGRO_EVENT_TIMER) {
                
                if (dialogo == false) {
                    tempo_segundos += 1.0 / 60;

                    player_mosquito.pontuacao = (int)(tempo_segundos * 5); // 5 pontos por segundo
                    controle.pontuacao[0] = (int)(tempo_segundos * 5);
                    printf("Tempo: %.1f segundos\n Pontuacao: %d\n ", tempo_segundos, player_mosquito.pontuacao); //verificando func segundos e pontuacoa
                    static int vel_mosquito = 10;
                    // Atualiza a posição do mosquito com base nas teclas W e S
                    if ((al_key_down(&kState, ALLEGRO_KEY_W) || al_key_down(&kState, ALLEGRO_KEY_UP)) && timer_invencibilidade == 0) {
                        player_mosquito.y -= vel_mosquito; // Move para cima
                    }
                    if ((al_key_down(&kState, ALLEGRO_KEY_S) || al_key_down(&kState, ALLEGRO_KEY_DOWN)) && timer_invencibilidade == 0) {
                        player_mosquito.y += vel_mosquito; // Move para baixo
                    }

                    // Limita a posição do mosquito dentro da tela
                    if (player_mosquito.y < 0) {
                        player_mosquito.y = 0;
                    }
                    if (player_mosquito.y > DISPLAY_HEIGHT - al_get_bitmap_height(mosquitao)) {
                        player_mosquito.y = DISPLAY_HEIGHT - al_get_bitmap_height(mosquitao);
                    }

                    // Atualiza a posição da mão
                    mao.x -= rand() % 6 + 6;
                    // Atualiza a posição da teia
                    teia.x -= rand() % 6 + 6;

                    bonus_estrofulo.x -= rand() % 5 + 1;

                    // Verifica se a mão saiu da tela
                    if (mao.x < 0) {
                        // Resetando a mão para o início
                        mao.x = DISPLAY_WIDTH;

                        // Alterna para o próximo padrão
                        indice_padroes = (indice_padroes + 1) % 4;

                        // Atualiza a amplitude para o novo padrão
                        amplitude = rand() % (DISPLAY_HEIGHT / 2) + (DISPLAY_HEIGHT / 2);

                        // Nova posição Y aleatória
                        mao.y = rand() % (DISPLAY_HEIGHT - 20) + 10;

                    }
                    if (teia.x < -30) {
                        // Resetando a teia para o início
                        teia.x = DISPLAY_WIDTH;

                        // Nova posição Y aleatória
                        teia.y = rand() % (DISPLAY_HEIGHT - 20) + 10;

                    }
                    if (bonus_estrofulo.x < -30) {
                        bonus_estrofulo.x = (DISPLAY_WIDTH + 50);
                        bonus_estrofulo.y = rand() % (DISPLAY_HEIGHT - 60) + 10;

                    }

                    // Calcula a posição Y da mão usando o padrão atual
                    mao.y = padrao_movimento[indice_padroes](mao.x, amplitude);

                    // Verifica colisão com a mao
                    if (colisao_quadrado_dentro(mao.x, mao.y, mao.raio, mao.raio, player_mosquito.x, player_mosquito.y, al_get_bitmap_width(mosquitao), al_get_bitmap_height(mosquitao)) && player_mosquito.vivo) {
                        player_mosquito.x -= 100; // Lógica de colisão com a mão
                        player_mosquito.vivo = false;
                    }
                    // Verifica colisão com a teia
                    if (colisao_quadrado_dentro(teia.x, teia.y, al_get_bitmap_width(teia_img), al_get_bitmap_height(teia_img), player_mosquito.x, player_mosquito.y, al_get_bitmap_width(mosquitao) / 2, al_get_bitmap_height(mosquitao)) && player_mosquito.vivo) {
                        player_mosquito.x -= 100; // Lógica de colisão com a teia
                        player_mosquito.vivo = false;
                    }

                    if (!player_mosquito.vivo) {
                        timer_invencibilidade++;
                        if (timer_invencibilidade / 30 == 1) {
                            player_mosquito.vivo = true;
                            timer_invencibilidade = 0;
                        }
                    }

                    // Verifica colisão com o bonus
                    if (colisao_quadrado_dentro(bonus_estrofulo.x, bonus_estrofulo.y, al_get_bitmap_width(bonus_img), al_get_bitmap_height(bonus_img), player_mosquito.x, player_mosquito.y, al_get_bitmap_width(mosquitao) / 2, al_get_bitmap_height(mosquitao))) {
                        bonus_estrofulo.x = (DISPLAY_WIDTH + 50);
                        bonus_estrofulo.y = rand() % (DISPLAY_HEIGHT - 60) + 10;
                        cont_bonus++;
                        printf("BONUS COLETADOS: %d\n", cont_bonus);
                        
                    }


                    // Verifica colisao com o spray
                    if (colisao_quadrado_dentro(spray.x, spray.y, al_get_bitmap_width(spray_img), al_get_bitmap_height(spray_img), player_mosquito.x, player_mosquito.y, al_get_bitmap_width(mosquitao) / 2, al_get_bitmap_height(mosquitao)) || player_mosquito.x < 0/*garante q caso saia da tela morre*/) {
                        tela = GAME_OVER;
                        player_mosquito.x = 600;
                        tempo_segundos = 0;
                        player_mosquito.pontuacao = 0;

                    }
                    spray.y += (player_mosquito.y - 50 - spray.y) * suavidade; // o y do spray segue o y do mosquito com uma velocidade certa para suavidade
                }

                largura_barra = 760.0 - 40.0; // comprimento total da barra de progresso
                barra_progresso = (tempo_segundos / tempo_max) * largura_barra; // calculo do progresso da barra baseada no tempo de jogo
                barra_progresso = fmin(barra_progresso, largura_barra); // comando que GARANTE que a barra não ultrapasse a largura máxima
                icone_mosquito = 40 + barra_progresso; // calculo para posicao do iconmosquitao com base no progresso da barra
                icone_mosquito = fmin(icone_mosquito, 760); // comando que GARANTE que o icone_mosquito nao ultrapasse a largura máxima da barra

                if (barra_progresso >= largura_barra) {
                    controle.pontuacao[0] = player_mosquito.pontuacao + cont_bonus * 15 * 1000;
                    controle.venceu_jogo = true;
                    printf("bonus pegos = %d * 15 = %d\npontuacao total: %d", cont_bonus, cont_bonus*15,controle.pontuacao[0]);
                }
            }


            // Desenha a imagem de fundo
            al_draw_bitmap(background_estrofulo, 0, 0, 0);

            // Desenha a mão
            al_draw_rotated_bitmap(mao_img, al_get_bitmap_width(mao_img) / 2, al_get_bitmap_height(mao_img) / 2, mao.x + al_get_bitmap_width(mao_img) / 2, mao.y + al_get_bitmap_height(mao_img) / 2, ALLEGRO_PI / 2, ALLEGRO_FLIP_VERTICAL);

            //Desenha a teia
            al_draw_bitmap(teia_img, teia.x, teia.y, 0);

            // Desenha o mosquito
            al_draw_bitmap(mosquitao, player_mosquito.x, player_mosquito.y, 0);

            // Desenha o bonus
            al_draw_bitmap(bonus_img, bonus_estrofulo.x, bonus_estrofulo.y, 0);

            //Desenha o spray
            al_draw_bitmap(spray_img, spray.x, spray.y, 0);

            al_draw_rectangle(40 - 2, 18, 760 + 2, 42, BLACK, 2); // desenha as bordas da barra de progresso

            al_draw_filled_rectangle(40, 20, 40 + barra_progresso, 40, RED); // desenha a barra de progresso

            al_draw_bitmap(iconmosquitao, icone_mosquito - al_get_bitmap_width(iconmosquitao) / 2, 5, 0); // desenha o iconmosquitao sobre a barra de progresso, acompanhando seu movimento
            
        } 
        break;

        case TUTORIAL_FAGOCITOSE:
        {
            dialogo = true;

            if (ev.type == ALLEGRO_EVENT_TIMER) {
                contador_de_frames++;
                //Verifica o timer a cada meio segundo'
                if (contador_de_frames % (frame_segundo / intervalo_segundo) == 0)
                    opacidade_texto = opacidade_texto * (-1);
            }
            //se apertar espaço pula para o jogo
            if (al_key_down(&kState, ALLEGRO_KEY_SPACE))
                ir_para_tela(FAGOCITOSE);
            //Desenhar
            // Desenha a imagem de fundo na tela (na posição (0, 0))
            al_draw_bitmap(tela_tutorial, 0, 0, 0);
            al_draw_bitmap(bg_pausa, 0, 0, 0);
            // Desenha o texto alterando a cor entre preto e branco a cada 2 segundos
            al_draw_textf(fonte_texto, al_map_rgb(opacidade_texto, opacidade_texto, opacidade_texto), 400, 555, ALLEGRO_ALIGN_CENTER, "APERTE A TECLA 'ESPAÇO' PARA CONTINUAR");

            al_draw_bitmap(mosquitao, 622, 203, 0);
            al_draw_line(569, 265, 710, 265, WHITE, 3.0);
            al_draw_bitmap(mouse_tutorial, 632, 275, 0);


            //Título
            al_draw_text(fonte_titulo, WHITE, 400, 75, ALLEGRO_ALIGN_CENTER, "COMO JOGAR?");

            //Texto
            al_draw_text(fonte_texto, WHITE, 50, 167, ALLEGRO_ALIGN_LEFT, "- DOMINE O CORPO! Mova célula infectada com o mouse");
            al_draw_text(fonte_texto, WHITE, 50, 197, ALLEGRO_ALIGN_LEFT, "- COMA OS CÍRCULOS, para acumular pontos!");
            al_draw_text(fonte_texto, WHITE, 50, 368, ALLEGRO_ALIGN_LEFT, "- FUJA DO FAGÓCITO! Seja mais rápido que o sistema imunológico");
            al_draw_text(fonte_texto, WHITE, 50, 398, ALLEGRO_ALIGN_LEFT, "  para sobreviver!");

            dialogo = true;
        }
        break;

        case FAGOCITOSE:
        {
            tela_anterior = tela;
            pode_pausar = true;

            ////   caixa de dialogo
            if (dialogo == true) {

                al_draw_bitmap(bg_pausa, 0, 0, 0);

                char textos[NUM_TEXTO][MAX_TEXTO] = {

                  " DOMINAR CÉLULA... FEITO.",
                  "COMER NUTRIENTES... EM ANDAMENTO.",
                  "SE MULTIPLICAR... EM ANDAMENTO.",
                  "EVITAR FAGÓCITOS...",
                  "EM ANDAMENTO."
                };

                desenhar_caixa_dialogo(100, 200, 600, 200, font, textos, &tempo_perdeu, event_queue, &dialogo, virus_viremia);


                // Atualiza a tela e faz outras operações, se necessário
                al_flip_display();

                // Espera um pouco para dar tempo para o tempo ser incrementado
                al_rest(0.1);
            }

            if (ev.type == ALLEGRO_EVENT_TIMER && dialogo == false) {
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
                if (player_fago.x > DISPLAY_WIDTH - player_fago.raio)
                    player_fago.x = DISPLAY_WIDTH - player_fago.raio;
                else if (player_fago.x < 0 + player_fago.raio)
                    player_fago.x = player_fago.raio;
                if (player_fago.y > DISPLAY_HEIGHT - player_fago.raio)
                    player_fago.y = DISPLAY_HEIGHT - player_fago.raio;
                else if (player_fago.y < 0 + player_fago.raio)
                    player_fago.y = player_fago.raio;

                int posicaoPlayerGrid = pega_posicao_grid(player_fago.x, player_fago.y);

                //movimentação fagócito inimigo - Pong
                    //caso bata em uma das "paredes" muda de direção
                if (fago_pong.x >= DISPLAY_WIDTH - fago_pong.raio || fago_pong.x <= 0 + fago_pong.raio)
                    pong_velX *= -1;
                if (fago_pong.y >= DISPLAY_HEIGHT - fago_pong.raio || fago_pong.y <= 0 + fago_pong.raio)
                    pong_velY *= -1;
                fago_pong.x += fago_pong.vel * pong_velX;
                fago_pong.y += fago_pong.vel * pong_velY;


                //COLISÕES
                //PLAYER X NUTRIENTE
                checa_colisao_vizinhos(grid_map, qtd_nutrientes_grid, nutrientes, raio_nutriente, &player_fago, posicaoPlayerGrid, &control_fago);

                //PLAYER X INIMIGO
                if (!control_fago.morto && colisao_circulo_dentro(player_fago.x, player_fago.y, player_fago.raio, fago_pong.x, fago_pong.y, fago_pong.raio)) {
                    control_fago.morto = true;
                    player_fago.raio = player_fago.raio_min;
                    control_fago.tentativas--;
                }
                //se as vidas acabarem
                if (control_fago.tentativas <= 0) {
                    tela = GAME_OVER;
                }
                if (control_fago.cont_vezes_crescidas >= 3) {
                    controle.venceu_jogo = true;
                    controle.pontuacao[1] = control_fago.pontuacao;
                }

                //deixa o player invencível por alguns segundos após morrer
                if (control_fago.morto) {
                    timer_invencibilidade++;
                    if (timer_invencibilidade / 60 == 1) {
                        control_fago.morto = false;
                        timer_invencibilidade = 0;
                    }
                }

                //reseta tamanho do player
                if (player_fago.raio >= player_fago.raio_max) {
                    player_fago.raio = player_fago.raio_min;
                    //faz ganhar pontos
                    control_fago.cont_vezes_crescidas++;
                    control_fago.pontuacao += 50;
                }
                //diminui constantemente o tamanho do player
                if (player_fago.raio > player_fago.raio_min)
                    player_fago.raio -= 0.04;
                else
                    player_fago.raio = player_fago.raio_min;
                 
            }


            /*****DESENHO*****/
            al_draw_bitmap(bg_fagocitose_bitmap, 0, 0, 0);
            //desenha nutrientes
            for (int i = 0; i < num_nutrientes; i++) {
                int opcaoCor = i % 3; //numero de opcoes de cores
                ALLEGRO_COLOR corNutriente;
                if (opcaoCor == 0)
                    corNutriente = PINK;
                else if (opcaoCor == 1)
                    corNutriente = BLUE;
                else {
                    corNutriente = GREEN;
                }
                al_draw_filled_circle(nutrientes[i].x, nutrientes[i].y, raio_nutriente, corNutriente);
            }
            // Desenha player
            al_draw_filled_circle(player_fago.x, player_fago.y, player_fago.raio, RED);

            // desenha inimigo (elipse)
            al_draw_filled_circle(fago_pong.x, fago_pong.y, fago_pong.raio, BLACK);


            al_draw_filled_rounded_rectangle(5, 5, 190, 115, 5, 5, al_map_rgb(10,10, 10));

            // HUD
            al_draw_textf(fonte_HUD, WHITE, 10, 5, ALLEGRO_ALIGN_LEFT, "pontos = %d", control_fago.pontuacao);
            al_draw_textf(fonte_HUD, WHITE, 10, 40, ALLEGRO_ALIGN_LEFT, "vidas = %d", control_fago.tentativas);
            al_draw_textf(fonte_HUD, WHITE, 10, 75, ALLEGRO_ALIGN_LEFT, "infeccoes = %d/3", control_fago.cont_vezes_crescidas);
            /*****FIM DESENHO*****/
        }
        break;

        case TUTORIAL_VIREMIA:
        {

            dialogo = true;

            if (ev.type == ALLEGRO_EVENT_TIMER) {
                contador_de_frames++;
                //Verifica o timer a cada meio segundo'
                if (contador_de_frames % (frame_segundo / intervalo_segundo) == 0)
                    opacidade_texto = opacidade_texto * (-1);
            }
            //se apertar espaço pula para o jogo
            if (al_key_down(&kState, ALLEGRO_KEY_SPACE))
                ir_para_tela(VIREMIA);
            //Desenhar
            // Desenha a imagem de fundo na tela (na posição (0, 0))
            al_draw_bitmap(background_viremia, 0, 0, 0);
            al_draw_bitmap(bg_pausa, 0, 0, 0);
            // Desenha o texto alterando a cor entre preto e branco a cada 2 segundos
            al_draw_textf(fonte_texto, al_map_rgb(opacidade_texto, opacidade_texto, opacidade_texto), 400, 555, ALLEGRO_ALIGN_CENTER, "APERTE A TECLA 'ESPAÇO' PARA CONTINUAR");

            linhas_Onduladas(632 + 15, 203 + 15, 682 + 50 * cos(0.03), 183 + 50 * sin(0.03), 40);
            al_draw_bitmap(celula_viremia, 632, 203, 0);
            al_draw_bitmap(virus_viremia, 712, 183, 0);
            al_draw_line(569, 265, 710, 265, WHITE, 3.0);
            al_draw_bitmap(mouse_tutorial, 632, 275, 0);
            al_draw_bitmap(cd8_viremia, 390, 450, 0);


            //Título
            al_draw_text(fonte_titulo, WHITE, 400, 75, ALLEGRO_ALIGN_CENTER, "COMO JOGAR?");

            //Texto
            al_draw_text(fonte_texto, WHITE, 50, 167, ALLEGRO_ALIGN_LEFT, "- MISSÃO: DOMINAR O CORPO! Seu objetivo? Infectar tudo!");
            al_draw_text(fonte_texto, WHITE, 50, 197, ALLEGRO_ALIGN_LEFT, "- Utilize o mouse para manipular a célula patogênica.");
            al_draw_text(fonte_texto, WHITE, 50, 368, ALLEGRO_ALIGN_LEFT, "- FUJA DOS LINFÓCITOS CD8, para garantir a propagação da infecção.");
        }
        break;

        case VIREMIA:
        {
            al_set_audio_stream_playing(viremia_som,1);
            tela_anterior = tela;

            ////   caixa de dialogo
            if (dialogo == true) {

                al_draw_bitmap(bg_pausa, 0, 0, 0);

                char textos[NUM_TEXTO][MAX_TEXTO] = {
                  "NOVO OBJETIVO DETERMINADO.",
                  "SE ESPALHAR PELO SANGUE.",
                  "DIVIDIR.",
                  "CONQUISTAR.",
                  "DOMINAR."
                };

                desenhar_caixa_dialogo(100, 200, 600, 200, font, textos, &tempo_perdeu, event_queue, &dialogo, virus_viremia);


                // Atualiza a tela e faz outras operações, se necessário
                al_flip_display();

                // Espera um pouco para dar tempo para o tempo ser incrementado
                al_rest(0.1);
            }

            pode_pausar = true;
            // Verifica se o evento é do temporizador
            if (ev.type == ALLEGRO_EVENT_TIMER) {
                if (ev.timer.source == countdown_timer) {
                    if (cronometro >= 0) {
                        cronometro = cronometro + 1;
                    }
                }

                // Verifica se o mouse está sobre o círculo
                if (colisao_mouse(mState, player_vire.x, player_vire.y, al_get_bitmap_width(celula_viremia), al_get_bitmap_height(celula_viremia))) {
                    startJogo = true;
                }
                else {
                    startJogo = false;
                }

                // Reiniciar a cada iteração
                dentro_da_linha = false;

                //Fase viremia começou
                if (startJogo == true) {

                    pode_seguir_mouse = true;

                    //Verifica se o círculo está em cima de alguma linha ou dos quadrados brancos
                    for (int i = 0; i < tamanho; i++) {
                        if (!fora_Da_Linha(player_vire.x, player_vire.y, coordenada_X[i], coordenada_Y[i], coordenada_X[i + 1],
                            coordenada_Y[i + 1], espessura_linha)
                            || player_vire.x >= x_checkpoint1 && player_vire.x <= x_checkpoint1 + 20 && player_vire.y >= y_checkpoint1 && player_vire.y <= y_checkpoint1 + 20
                            || player_vire.x >= x_checkpoint2 && player_vire.x <= x_checkpoint2 + 20 && player_vire.y >= y_checkpoint2 && player_vire.y <= y_checkpoint2 + 20) {
                            dentro_da_linha = true;
                            printf("esta colidindo\n");
                            break; // Se o cursor estiver sobre uma linha, não precisa verificar as demais
                        }
                        else { printf("colisao is no more\n"); }
                    }

                    //PLAYER X INIMIGO
                    for (int i = 0; i < quantidade_CD8; i++) {
                        if (colisao_quadrado_dentro(player_vire.x, player_vire.y, al_get_bitmap_width(celula_viremia),
                            al_get_bitmap_height(celula_viremia), linfocito_CD8[i].x, linfocito_CD8[i].y, al_get_bitmap_width(cd8_viremia),
                            al_get_bitmap_height(cd8_viremia))) {
                            if (!invulneravel) {
                                vidas_disponiveis_viremia--;
                                invulneravel = true;
                                contador_invulnerabilidade = tempo_invulnerabilidade;
                            }
                        }
                    }

                    if (invulneravel) {
                        contador_invulnerabilidade--;
                        if (contador_invulnerabilidade <= 0) {
                            invulneravel = false;
                        }
                    }

                    if (vidas_disponiveis_viremia == 0) {
                        tela = GAME_OVER;
                    }

                    //niveis viremia
                    if (nivel_viremia == 1 || nivel_viremia == 3) {
                        x_chegada = x_checkpoint2;
                        y_chegada = y_checkpoint2;
                    }
                    else if (nivel_viremia == 2) {
                        x_chegada = x_checkpoint1;
                        y_chegada = y_checkpoint1;
                    }
                    if (nivel_viremia < 3 && (mState.x >= x_chegada && mState.x <= x_chegada + 20 && mState.y >= y_chegada && mState.y <= y_chegada + 20)) {
                        mudou_de_nivel_viremia = true;
                    }
                    if (mudou_de_nivel_viremia == true) {
                        gera_coordenadas_X(coordenada_X, tamanho, x_checkpoint1, x_checkpoint2);
                        gera_coordenadas_Y(coordenada_Y, tamanho, y_checkpoint1, y_checkpoint2);
                        //Diminui a espessura da linha
                        espessura_linha -= espessura_linha * 0.12;
                        nivel_viremia++;

                        //podeSeguirMouse = false;

                        //Reseta mudouDeNivel
                        mudou_de_nivel_viremia = false;
                    }
                    if (nivel_viremia == 3 && (mState.x >= x_checkpoint2 && mState.x <= x_checkpoint2 + 20 && mState.y >= y_checkpoint2 && mState.y <= y_checkpoint2 + 20)){
                        controle.pontuacao[2] = pontuacao_viremia(cronometro);
                        controle.venceu_jogo = true;
                    }
                }
                if (pode_seguir_mouse) {
                    player_vire.x = mState.x - al_get_bitmap_width(celula_viremia) / 2;
                    player_vire.y = mState.y - al_get_bitmap_height(celula_viremia) / 2;
                }
                // Incrementa o ângulo
                angulo_viremia += 0.03;
                // atualiza as posições dos linfócitos
                for (int i = 0; i < quantidade_CD8; i++) {
                    linfocito_CD8[i].x += linfocito_CD8[i].velocidade;
                    linfocito_CD8[i].y += linfocito_CD8[i].velocidade * linfocito_CD8[i].direcao;

                    if (linfocito_CD8[i].y > DISPLAY_HEIGHT) {
                        linfocito_CD8[i].y = DISPLAY_HEIGHT; // Limitar a coordenada Y máxima
                        linfocito_CD8[i].direcao = -1; // Mudar a direção para baixo
                    }
                    // Verificar se a imagem atingiu a faixa vertical final
                    if (linfocito_CD8[i].y <= 250) {
                        // Se a imagem estiver subindo e atingiu a faixa final, inverter a direção
                        linfocito_CD8[i].direcao = -1;
                    }

                    // Se a imagem atingir o topo, reinicializa
                    if (linfocito_CD8[i].y < -50 || linfocito_CD8[i].x > 850) {
                        linfocito_CD8[i].x = -al_get_bitmap_width(cd8_viremia);
                        linfocito_CD8[i].y = 340 + rand() % (600 - 340 + 1);
                        linfocito_CD8[i].direcao = 1;
                    }
                }
            }
            // - - - - - - - DESENHO - - - - - - -
            // Desenha a imagem de fundo
            al_draw_bitmap(background_viremia, 0, 0, 0);

            //Desenha as linhas chamando a função
            gerar_Linhas(coordenada_X, coordenada_Y, tamanho, espessura_linha);

            //Desenha os quadrados brancos
            al_draw_filled_rectangle(x_checkpoint1, y_checkpoint1, x_checkpoint1 + 20, y_checkpoint1 + 20, WHITE);
            al_draw_filled_rectangle(x_checkpoint2, y_checkpoint2, x_checkpoint2 + 20, y_checkpoint2 + 20, WHITE);

            // Calcula a posição da imagem no círculo
            float x_virus_viremia = player_vire.x + raio_viremia * cos(angulo_viremia);
            float y_virus_viremia = player_vire.y + raio_viremia * sin(angulo_viremia);

            //Desenha as linhas onduladas a partir da Função seno
            linhas_Onduladas(player_vire.x + player_vire.diferenca, player_vire.y + player_vire.diferenca, x_virus_viremia, y_virus_viremia, 40);

            // Desenha a imagem
            al_draw_bitmap(virus_viremia, x_virus_viremia - al_get_bitmap_width(virus_viremia) / 2, y_virus_viremia - al_get_bitmap_height(virus_viremia) / 2, 0);

            // Desenha o player na nova posição
            al_draw_bitmap(celula_viremia, player_vire.x, player_vire.y, 0);

            // desenha linfócitos
            for (int i = 0; i < quantidade_CD8; i++) {
                al_draw_bitmap(linfocito_CD8[i].cd8_viremia, linfocito_CD8[i].x, linfocito_CD8[i].y, 0);
            }

            // Desenhar o texto na tela usando a fonte embutida
            al_draw_textf(font, WHITE, 700, 10, ALLEGRO_ALIGN_CENTER, "Nível: %d/3", nivel_viremia);

            sprintf_s(cronometro_str, "0%d'", cronometro);
            al_draw_text(font, WHITE, 400, 10, ALLEGRO_ALIGN_CENTER, cronometro_str);

            atualiza_vidas(vidas_viremia[0], vidas_viremia[1], vidas_viremia[2], vidas_viremia[3], vidas_viremia[4], vidas_viremia[5], vidas_disponiveis_viremia, vida_viremia_x, vida_viremia_y);
            // - - - - - - - FIM DOS DESENHOS - - - - - - -
        }
        break;
        case VENCEU_VIREMIA:
        {
            // Reproduzir áudio
            al_set_audio_stream_playing(viremia_som, 0);
            static int num_linhas_popup_vitoria = 7; //quantas linhas vão ter no popup que aparece na tela de vitória do jogo 
            //linha 1 - PARABENS !! MISSAO CONCLUIDA !!
            //linha 2 - Voce conseguiu com sucesso infectar este ser humano com o vírus da dengue
            //linha 3 - pontuacao jogo 1 
            //linha 4 - pontuacao jogo 2
            //linha 5 - pontuacao jogo 3
            //linha 6 - pontuacao total
            //linha 7 - jogo concluído em ... segundos
            
            static int fonte_HUD_h = al_get_font_line_height(fonte_HUD);
            static int mensagem_vitoria_y = (DISPLAY_HEIGHT - (fonte_HUD_h * 7)) / 2;

            static int segundos_jogo_rodado = al_get_timer_count(timer) / 60;

            //botao voltar !!
            static int botao_voltar_inicio_x = 50;
            static int botao_voltar_inicio_y = DISPLAY_HEIGHT - 100;


            al_clear_to_color(WHITE);
            al_draw_bitmap(bg_venceu_jogo,0,0,0);
            al_draw_filled_rounded_rectangle(DISPLAY_WIDTH / 2 - 400, mensagem_vitoria_y + fonte_HUD_h, DISPLAY_WIDTH / 2 + 400, mensagem_vitoria_y + fonte_HUD_h * (num_linhas_popup_vitoria + 1), 5, 5, al_map_rgba(0, 0, 0, 0.5));
            al_draw_text(fonte_HUD, BLACK, DISPLAY_WIDTH / 2, mensagem_vitoria_y + fonte_HUD_h * 1, ALLEGRO_ALIGN_CENTER, "PARABENS !!! VOCE VENCEU !!!");
            al_draw_text(fonte_HUD, BLACK, DISPLAY_WIDTH / 2, mensagem_vitoria_y + fonte_HUD_h * 2, ALLEGRO_ALIGN_CENTER, "Conseguiu com sucesso infectar o humano com a dengue!");
            al_draw_textf(fonte_HUD, BLACK, DISPLAY_WIDTH / 2, mensagem_vitoria_y + fonte_HUD_h * 3, ALLEGRO_ALIGN_CENTER, "Pontos na fase ESTROFULO = %d", controle.pontuacao[0]);
            al_draw_textf(fonte_HUD, BLACK, DISPLAY_WIDTH / 2, mensagem_vitoria_y + fonte_HUD_h * 4, ALLEGRO_ALIGN_CENTER, "Pontos na fase FAGOCITOSE = %d", controle.pontuacao[1]);
            al_draw_textf(fonte_HUD, BLACK, DISPLAY_WIDTH / 2, mensagem_vitoria_y + fonte_HUD_h * 5, ALLEGRO_ALIGN_CENTER, "Pontos na fase VIREMIA = %d", controle.pontuacao[2]);
            al_draw_textf(fonte_HUD, BLACK, DISPLAY_WIDTH / 2, mensagem_vitoria_y + fonte_HUD_h * 6, ALLEGRO_ALIGN_CENTER, "Pontos no total = %d", controle.pontuacao_total);
            al_draw_textf(fonte_HUD, BLACK, DISPLAY_WIDTH / 2, mensagem_vitoria_y + fonte_HUD_h * 7, ALLEGRO_ALIGN_CENTER, "Jogo zerado em %d segundos.", segundos_jogo_rodado);
        
            criar_bitmap_botao(botao_play, fonte_texto, "Voltar ao inicio", BLACK,0, SUL, botao_voltar_inicio_x, botao_voltar_inicio_y);
            if (colisao_mouse(mState, botao_voltar_inicio_x, botao_voltar_inicio_y, al_get_bitmap_width(botao_play), al_get_bitmap_height(botao_play)) && mouse_click())
                ir_para_tela(TELA_INICIAL);
        }
        break;
        case GAME_OVER:
        { // Reproduzir áudio
            al_set_audio_stream_playing(mosquito, 0);
            al_play_sample(morreu, 5.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
            if (ev.type == ALLEGRO_EVENT_TIMER) {
               
                if (ev.type == ALLEGRO_EVENT_TIMER && ev.timer.source == countdown_timer) {


                    if (tempo_perdeu >= 0) {
                        tempo_perdeu = tempo_perdeu + 1;  // Decrementa o cronômetro
                        printf("%d\n", tempo_perdeu);
                    }
                }
            }

            if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                if (ev.keyboard.keycode == ALLEGRO_KEY_S) {
                    ir_para_tela(tela_anterior);
                }
                else if (ev.keyboard.keycode == ALLEGRO_KEY_N) {
                    tela = TELA_INICIAL;
                }
            }
            //DESENHO
            //fundo
            al_draw_bitmap(tela_perdeu, 0, 0, 0);
            //texto piscando pra continuar ("Deseja continuar - sim ou nao")
            if (tempo_perdeu % 2 == 1) {

                al_draw_text(fonte_20, WHITE, 370, 550, -20, "S/N");
            }
            else {
                al_draw_text(fonte_20, RED, 370, 550, 0, "S/N");
            }
        }
        break;
        default:
        {
            al_clear_to_color(WHITE);
            al_draw_text(fonte_HUD, BLACK, 50, 50, 0, "Erro 404: Tela nao encontrada");
        }
        break;
        } // <- fim switch

        //sistema de vitória
        if (controle.venceu_jogo)
            popup_vitoria(fonte_HUD, nextlevel, &tela);

        //sistema de pausa
        if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE && (tela == ATAQUE_MOSQUITO || (tela == FAGOCITOSE || tela == VIREMIA)) && pode_pausar) {
                jogo_pausado = !jogo_pausado;
                if (!jogo_pausado) {
                    unpause();
                }
            }
        }
        if (jogo_pausado) {
            pause();
            al_draw_bitmap(bg_pausa, 0, 0, 0);
            voltarTelaEscolha(ev, &tela, fonte_20, &jogo_pausado);
            al_draw_text(fonte_texto, WHITE, DISPLAY_WIDTH / 2, DISPLAY_HEIGHT - 50, ALLEGRO_ALIGN_CENTER, "ATALHO P/ DEBUG: ENTER - AVANÇA TELA");
            if (al_key_down(&kState, ALLEGRO_KEY_ENTER)) {
                controle.venceu_jogo = true;
                unpause();
                jogo_pausado = !jogo_pausado;
            }
        }
        pode_pausar = false;
        al_flip_display();
    }

    al_destroy_event_queue(event_queue);
    al_destroy_display(display);
    al_destroy_timer(timer);

    al_destroy_bitmap(loading);
    al_destroy_bitmap(background_desktop);
    al_destroy_bitmap(background_fases);
    al_destroy_bitmap(background_viremia);
    al_destroy_bitmap(bg_fagocitose_bitmap);
    al_destroy_bitmap(cd8_viremia);
    al_destroy_bitmap(virus_viremia);
    al_destroy_bitmap(mosquitao);
    al_destroy_bitmap(iconmosquitao);
    al_destroy_bitmap(teia_img);
    al_destroy_bitmap(spray_img);
    al_destroy_bitmap(background_estrofulo);
    al_destroy_bitmap(bonus_img);
    al_destroy_bitmap(tela_perdeu);
    al_destroy_bitmap(celula_viremia);
    al_destroy_bitmap(teclas_tutorial);
    al_destroy_bitmap(tela_tutorial);
    al_destroy_bitmap(mouse_tutorial);
    al_destroy_bitmap(spray_tutorial);
    al_destroy_bitmap(virus_PB);
    al_destroy_bitmap(botao_play);
    al_destroy_bitmap(nextlevel);
    al_destroy_bitmap(paintogenico);
    al_destroy_bitmap(bg_venceu_jogo);


    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            al_destroy_bitmap(quadrinhos_HQ[i][j]);
        }
    }
    al_destroy_bitmap(dialogo_mosquito);
    al_destroy_bitmap(dialogo_virus);

    for (int i = 0; i < sizeof(vidas_viremia) / 8; i++) { //divide por 8 pq é o tamanho de ALLEGRO_BITMAP (descobri isso de forma empírica)
        al_destroy_bitmap(vidas_viremia[i]);
    } 
    for (int i = 0; i < quantidade_CD8; i++) {
        al_destroy_bitmap(linfocito_CD8[i].cd8_viremia);
    }

    al_destroy_font(font);
    al_destroy_font(fonte_20);
    al_destroy_font(fonte_HUD);
    al_destroy_font(fonte_texto);
    al_destroy_font(fonte_titulo);

    al_destroy_audio_stream(viremia_som);
    al_destroy_audio_stream(trilha_sonora);
    al_destroy_audio_stream(mosquito);
    al_destroy_sample(colisao);
    al_destroy_sample(gameover);
    al_destroy_sample(morreu);
    al_destroy_sample(next_level);

    al_shutdown_image_addon();

    free(coordenada_X);
    free(coordenada_Y);
    free(nutrientes);
    free(linfocito_CD8);

    return 0;
}