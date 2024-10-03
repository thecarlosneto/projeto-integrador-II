/*
TO-DO

atribuir um espaço na grid p cada bolinha

incrementar o numero de bolinhas naquela celula da grid

checar a colisao p cada bolinha dentro das 9 celulas em volta do player

*/

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h> // Para criar um circulo
#include <cmath> // Para utilizar o sqrt
#include <stdlib.h>
#include <stdio.h>

#define displayWidth 800
#define displayHeight 600

// bolinhas que são comidas pro player crescer
typedef struct {
    int x;
    int y;
}bolinha;

/*
função pra calcular colisão. (mais especificamente quando um objeto está INTEIRAMENTE dentro de outro)
recebe as coordenadas de duas instâncias (inserir coord A PARTIR DO CANTO SUPERIOR ESQUERDO, igual no p5js)
e sua largura e altura. Se os valores da instância 1 estiverem dentro da instância 2, retorna verdadeiro
(estão colidindo), caso contrário retorna falso (não houve colisão).
*/
bool isInside(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2) {
    if ((x2 < x1 && x1 < x2 + width2) && (y2 < y1 && y1 < y2 + height2)
        &&
        (x2 < x1 + width1 && x1 + width1 < x2 + width2) && (y2 < y1 + height1 && y1 + height1 < y2 + height2))
        return true;
    return false;
}

void randomizarBolinhas(bolinha* pBolinhas, const int* praioBolinha, const int* pnumBolinhas, int i) {
        do {
            pBolinhas[i].x = rand() % displayWidth;
        } while (pBolinhas[i].x > displayWidth + *praioBolinha || pBolinhas[i].x < 0 - *praioBolinha); //garante que a bolinha vai ficar inteiramente dentro da tela
        do {
            pBolinhas[i].y = rand() % displayHeight;
        } while (pBolinhas[i].y > displayHeight + *praioBolinha || pBolinhas[i].y < 0 - *praioBolinha);
}

//randomizarBolinhas(&raioBolinha);


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
    float circle_x = displayWidth / 2;  // Posição inicial do círculo (centro da tela)
    float circle_y = displayHeight / 2;
    int circle_r = 15;                  // Tamanho inicial do círculo
    const float MOVE_VELO = 5.0;        // Velocidade do movimento do círculo

    const int numBolinhas = 100;    //quantas bolinhas há no mapa
    const int raioBolinha = 5;

    #define espacoEntreLinhas 50
    #define nCelulasGrid (displayWidth / espacoEntreLinhas) * (displayHeight / espacoEntreLinhas)
    int gridMap[nCelulasGrid][11];     

    /*****SETUP DO JOGO*****/

    //define um valor aleatório para cada bolinha no mapa
    bolinha* bolinhas = (bolinha*)malloc(numBolinhas * sizeof(bolinha));
    for (int i = 0; i < numBolinhas; i++) {
        randomizarBolinhas(bolinhas, &raioBolinha, &numBolinhas, i);
    }
    while (true) {
        ALLEGRO_EVENT event;
        // Espera por um evento
        al_wait_for_event(event_queue, &event);

        // Verifica se o usuário clicou no X de fechar a janela
        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            break;
        }

        // Verifica se o evento é do temporizador
        if (event.type == ALLEGRO_EVENT_TIMER) {
            // Estado do mouse
            ALLEGRO_MOUSE_STATE mState;
            al_get_mouse_state(&mState);

            // Calcula a direção e a distância até a posição do mouse
            // Diferença na coordenada x entre o círculo e o mouse (mState.x - circle_x)
            float dx = mState.x - circle_x;
            // Diferença na coordenada y entre o círculo e o mouse (mState.y - circle_y).
            float dy = mState.y - circle_y;
            // Distância entre o círculo e o mouse, calculada como a raiz quadrada da soma dos quadrados de dx e dy
            float distancia = sqrt(dx * dx + dy * dy);

            // Move o círculo em direção ao mouse
            // apenas se a distancia > velocidade do circulo
            if (distancia > MOVE_VELO) {
                float move_x = (dx / distancia) * MOVE_VELO;
                float move_y = (dy / distancia) * MOVE_VELO;
                circle_x += move_x;
                circle_y += move_y;
            }
            

            /*****DESENHO*****/

            // Define a cor de fundo (BRANCO neste caso)
            al_clear_to_color(WHITE);

            // Desenha a grade de fundo
            for (int i = 1; i < displayWidth / espacoEntreLinhas; i++) {
                al_draw_line(i * espacoEntreLinhas, 0, i * espacoEntreLinhas, 600, GRAY, 2);     //desenha a linha
                for (int j = 1; j < displayHeight / espacoEntreLinhas; j++) {
                    al_draw_line(0 ,j * espacoEntreLinhas, 800, j * espacoEntreLinhas, GRAY, 2); //desenha a coluna
                }
            }
           
            //desenha bolinhas
            for (int i = 0; i < numBolinhas; i++) {
                al_draw_filled_circle(bolinhas[i].x, bolinhas[i].y, raioBolinha * 2, GREEN);
            }
            // Desenha o círculo na nova posição
            al_draw_filled_circle(circle_x, circle_y, circle_r, RED);



            //calcula colisão entre player-bolinhas
                //código não muito eficiente (mas funcional!!!), é provisório e deve ser consertado com urgência    -joao
            for (int i = 0; i < numBolinhas; i++) {
                if (isInside(bolinhas[i].x- raioBolinha, bolinhas[i].y - raioBolinha, raioBolinha * 2, raioBolinha * 2, //se as coordenadas das bolinhas ....
                                circle_x-circle_r + raioBolinha, circle_y-circle_r + raioBolinha, circle_r*2 - raioBolinha, circle_r*2 - raioBolinha))                                    //...estiverem dentro das coordenadas do player
                {
                    circle_r ++; //aumenta player
                    randomizarBolinhas(bolinhas, &raioBolinha, &numBolinhas, i);
                }
            }
            //reseta tamanho do player
            if (circle_r >= 75) {
                circle_r = 20;
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
