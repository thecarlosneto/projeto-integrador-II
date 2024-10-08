/*
TO-DO
checar a colisao p cada bolinha dentro das 9 celulas em volta do player

limpar código:

-arrumar colisão (simplificar pra círculos)
-ajustar comentários
-apagar código de debug
-criar um array par quantidade de bolinhas por célula qtdBolinhasCelula[nCelulasGrid]

*/

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
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
    int x;
    int y;
    ALLEGRO_COLOR cor;
} bolinha;

typedef struct  player_fagocitose{
    float x;
    float y;
    const int vel = 5;
    float raio;
};


//função pra calcular colisão. (mais especificamente quando um objeto está INTEIRAMENTE dentro de outro)
//recebe as coordenadas de duas instâncias (inserir coord A PARTIR DO CANTO SUPERIOR ESQUERDO, igual no p5js)
//e sua largura e altura. Se os valores da instância 1 estiverem dentro da instância 2, retorna verdadeiro.
bool dentro(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2) {

    if ((x2 < x1 && x1 < x2 + width2) && (y2 < y1 && y1 < y2 + height2)
        &&
        (x2 < x1 + width1 && x1 + width1 < x2 + width2) && (y2 < y1 + height1 && y1 + height1 < y2 + height2))
        return true;
    return false;
}

//recebe um par ordenado e retorna em qual posição ele está na grade de fundo (como se fosse um array unidimensional)
int pegaPosicaoGrid(int x, int y) {
    return ((x - (x % espacoEntreGrade)) / espacoEntreGrade) + ((y - (y % espacoEntreGrade)) / espacoEntreGrade) * nColunas;
}

void mapearBolinhas(int* x, int* y, const int raioBolinha, int iBolinha, int (*gridMap)[11]) {
    //define uma posição aleatória para cada bolinha
    //e garante que a bolinha vai ficar inteiramente dentro da tela
    do {
        *x = rand() % (displayWidth - 2 * raioBolinha) + raioBolinha * 2;
    } while (*x > displayWidth - raioBolinha);
    do {
        *y = rand() % (displayHeight - 2 * raioBolinha) + raioBolinha * 2;
    } while (*y > displayWidth - raioBolinha);

    //mapeia cada posição nova de bolinha para um espaço na grid
    int posicaoNaGrid = pegaPosicaoGrid(*x, *y);
    gridMap[posicaoNaGrid][0]++; //quantas bolinhas tem naquele espaço
    gridMap[posicaoNaGrid][gridMap[posicaoNaGrid][0]] = iBolinha; //qual bolinha está naquele espaço

    printf("a bolinha %d esta na casa (%d, %d)\n", iBolinha, posicaoNaGrid % nColunas, (int)floor(posicaoNaGrid / nColunas));
}

void checaColisaoVizinhos(int (*gridMap)[11], bolinha* bolinhas, int raioBolinha, player_fagocitose* player_fago, int posicaoPlayerGrid) {
    
    //se houver bolinhas em um espaço da grid
    if (gridMap[posicaoPlayerGrid][0] > 0) {

        //para cada bolinha em um espaço na grid
        for (int i = 1; i <= gridMap[posicaoPlayerGrid][0]; i++) {

            int indexBolinha = gridMap[posicaoPlayerGrid][i];

            //se o index da bolinha existir (!= -1), checa colisão com ela
            if (indexBolinha != -1) {
                //checa colisão
                if (dentro(bolinhas[indexBolinha].x - raioBolinha, bolinhas[indexBolinha].y - raioBolinha, raioBolinha * 2, raioBolinha * 2,
                    player_fago->x - player_fago->raio + raioBolinha, player_fago->y - player_fago->raio + raioBolinha, player_fago->raio * 2 - raioBolinha, player_fago->raio * 2 - raioBolinha)) {
                    
                    player_fago->raio++; //aumenta tamanho do player

                    // descobre qual espaço a bolinha está na grid e a remove de lá
                    int posicaoBolinhaGrid = pegaPosicaoGrid(bolinhas[indexBolinha].x, bolinhas[indexBolinha].y);
                    gridMap[posicaoBolinhaGrid][0]--;

                    // remove o index da bolinha do gridMap
                    for (int j = 1; j <= gridMap[posicaoBolinhaGrid][0]; j++) {
                        if (gridMap[posicaoBolinhaGrid][j] == indexBolinha) {
                            // move todos os elementos daquele espaço na grid um pra esquerda
                            for (int k = j; k < gridMap[posicaoBolinhaGrid][0] + 1; k++) {
                                gridMap[posicaoBolinhaGrid][k] = gridMap[posicaoBolinhaGrid][k + 1];
                            }
                            break;
                        }
                    }
                    // Atualiza a posição da bolinha na grid
                    mapearBolinhas(&bolinhas[indexBolinha].x, &bolinhas[indexBolinha].y, raioBolinha, indexBolinha, gridMap);
                }
            }
        }
    }
}

void desenharGradeFundo(int espacamento, ALLEGRO_COLOR cor, int grossuraLinha) {
    for (int i = 1; i < displayWidth / espacamento; i++) {
        al_draw_line(i * espacamento, 0, i * espacamento, 600, cor, grossuraLinha);     //desenha a linha
        for (int j = 1; j < displayHeight / espacamento; j++) {
            al_draw_line(0, j * espacamento, 800, j * espacamento, cor, grossuraLinha); //desenha a coluna
        }
    }
}



int main() {

    ALLEGRO_COLOR BLACK = al_map_rgb(0, 0, 0);
    ALLEGRO_COLOR WHITE = al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR GRAY = al_map_rgb(200, 200, 200);
    ALLEGRO_COLOR RED = al_map_rgb(255, 0, 0);
    ALLEGRO_COLOR GREEN = al_map_rgb(0, 255, 0);
    ALLEGRO_COLOR BLUE = al_map_rgb(0, 0, 255);
    ALLEGRO_COLOR PINK = al_map_rgb(221, 160, 221);

    /*****SETUP DO ALLEGRO*****/
    al_init();                  //para inicializar
    al_init_primitives_addon(); //para desenhar figuras
    al_install_mouse();         //para detectar mouse

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

    // Inicia o temporizador
    al_start_timer(timer);


    /*****VARIAVEIS LOCAIS*****/

    // bolinhas que são comidas pro player crescer

    bolinha* bolinhas;

    player_fagocitose player_fago;

    const int numBolinhas = 100;     //quantas bolinhas há no mapa
    const int raioBolinha = 5;

    int gridMap[nCelulasGrid][11];  //grade de fundo onde cada bolinha será atribuida
    for (int i = 0; i < nCelulasGrid; i++) {
        gridMap[i][0] = 0;
    }

    /*****SETUP DO JOGO*****/

    //define um valor aleatório para cada bolinha no mapa
    bolinhas = (bolinha*)malloc(numBolinhas * sizeof(bolinha)); //cria um array dinâmico de bolinhas
    for (int i = 0; i < numBolinhas; i++) {
        mapearBolinhas(&bolinhas[i].x,&bolinhas[i].y, raioBolinha, i, gridMap);
        bolinhas[i].cor = al_map_rgb(rand() % 255, rand() % 255, rand() % 255); //cor aleatória pra cada bolinha
    }
    //define como o círculo do player spawna no mapa
    player_fago.x = displayWidth / 2;
    player_fago.y = displayHeight / 2;
    player_fago.raio = 15;

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
            int posicaoPlayerGrid = pegaPosicaoGrid(player_fago.x, player_fago.y);

            /*****DESENHO*****/
            al_clear_to_color(WHITE);
            desenharGradeFundo(espacoEntreGrade, GRAY, 2);
           
            //desenha bolinhas
            for (int i = 0; i < numBolinhas; i++) {
                al_draw_filled_circle(bolinhas[i].x, bolinhas[i].y, raioBolinha * 2, bolinhas[i].cor);
            }

            // Desenha player
            al_draw_filled_circle(player_fago.x, player_fago.y, player_fago.raio, RED);


            // COLISÃO PLAYER X BOLINHA

            //checa os 9 espaços em volta do player
            for (int i = -1; i <= 1; i++) {
                for(int j = -1; j <= 1; j++){
                    int novaPos = posicaoPlayerGrid + i + j * nColunas;
                    //garante que nunca vai checar um espaço nulo (out of bounds)
                    if(novaPos >= 0 && novaPos <= nCelulasGrid)
                        checaColisaoVizinhos(gridMap, bolinhas, raioBolinha, &player_fago, novaPos);
                }
            }

            //reseta tamanho do player
            if (player_fago.raio >= 75) {
                player_fago.raio = 20;
            }

            // Atualiza a tela
            al_flip_display();
        }
    }

    // Limpa os recursos alocados
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
    al_destroy_timer(timer);

    free(bolinhas);
    return 0;
}
