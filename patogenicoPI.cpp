#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h> // Para criar um circulo
#include <cmath> // Para utilizar o sqrt
#include <stdlib.h>
#include <stdio.h>


/*
função *PROVISÓRIA* pra calcular colisão.
recebe as coordenadas de duas instâncias (inserir coord A PARTIR DO CANTO SUPERIOR ESQUERDO, igual no p5js)
e sua largura e altura. Se os valores da instância 1 estiverem dentro da instância 2, retorna verdadeiro
(estão colidindo), caso contrário retorna falso (não houve colisão).
*/
bool isColliding(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2) {
    if ((x2 < x1 && x1 < x2 + width2) && (y2 < y1 && y1 < y2 + height2))
        return true;
    return false;
}

// Cores
ALLEGRO_COLOR RED = al_map_rgb(255, 0, 0);
ALLEGRO_COLOR GREEN = al_map_rgb(0, 255, 0);
ALLEGRO_COLOR BLACK = al_map_rgb(0, 0, 0);
ALLEGRO_COLOR WHITE = al_map_rgb(255, 255, 255);




int main() {


    /*****SETUP DO ALLEGRO*****/
    al_init();                  //para inicializar
    al_init_primitives_addon(); //para desenhar figuras
    al_install_mouse();         //para detectar mouse


    // Cria uma janela
    int displayWidth = 800;
    int displayHeight = 600;
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
    float circle_x = displayWidth/2;  // Posição inicial do círculo (centro da tela)
    float circle_y = displayHeight/2;
    int CIRCUNFERENCIA = 30;         //como a gente quer que a circunferencia do player aumente, eu mudei de const float pra só int    -joao
    const float MOVE_VELO = 5.0;     // Velocidade do movimento do círculo

    // bolinhas que são comidas pro player crescer
    struct bolinha {
        int x;
        int y;
        int r = 5; //raio
    };


    /*****SETUP DO JOGO*****/

    //define um valor aleatório para cada bolinha no mapa
    bolinha bolinhas[30];
    for (int i = 0; i < 30; i++) {
        do {
            bolinhas[i].x = rand() % displayWidth;
        } while (bolinhas[i].x > displayWidth + bolinhas[0].r || bolinhas[i].x < 0-bolinhas[0].r); //garante que a bolinha vai ficar inteiramente dentro da tela
        do {
            bolinhas[i].y = rand() % displayHeight;
        } while (bolinhas[i].y > displayHeight + bolinhas[0].r || bolinhas[i].y < 0 - bolinhas[0].r);
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

            // Define a cor de fundo (preto neste caso)
            al_clear_to_color(WHITE);
            for (int i = 0; i < 30; i++) {
                al_draw_filled_circle(bolinhas[i].x, bolinhas[i].y, bolinhas[0].r * 2, GREEN);
            }

            // Desenha o círculo na nova posição
            al_draw_filled_circle(circle_x, circle_y, CIRCUNFERENCIA, RED);

            //calcula colisão entre player-bolinhas
                //código não muito eficiente (mas funcional!!!), é provisório e deve ser consertado com urgência    -joao
            for (int i = 0; i < 30; i++) {
                if (isColliding(bolinhas[i].x - bolinhas[0].r, bolinhas[i].y - bolinhas[0].r, bolinhas[0].r, bolinhas[0].r,   //"se as coordenadas de cada bolinha...
                                circle_x - CIRCUNFERENCIA / 2, circle_y - CIRCUNFERENCIA / 2, CIRCUNFERENCIA, CIRCUNFERENCIA))//...estiver dentro das coordenadas do player
                {                                                                                                             //então faça isso:"
                    CIRCUNFERENCIA +=2; //aumenta player
                    do {
                        bolinhas[i].x = rand() % displayWidth; //após "comida", randomiza novamente a posição da bolinha 
                    } while (bolinhas[i].x > displayWidth + bolinhas[0].r || bolinhas[i].x < 0 - bolinhas[0].r); //garante que a bolinha vai ficar inteiramente dentro da tela
                    do {
                        bolinhas[i].y = rand() % displayHeight;
                    } while (bolinhas[i].y > displayHeight + bolinhas[0].r || bolinhas[i].y < 0 - bolinhas[0].r);
                    printf("colidiu!!\n");
                }
            }
            // Atualiza a tela
            al_flip_display();
        }
    }

    // Limpa os recursos alocados
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
    al_destroy_timer(timer);

    return 0;
}
