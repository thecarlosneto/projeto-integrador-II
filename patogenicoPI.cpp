#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h> // Para criar um circulo
#include <cmath> // Para utilizar o sqrt

bool isColliding(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2) {
    if ((x2 < x1 && x1 < x2 + width2) && (y2 < y1 && y1 < y2 + height2))
        return true;
    return false;
}

const float CIRCUFERENCIA = 50;
const float MOVE_VELO = 5.0; // Velocidade do movimento do círculo

int main() {
    // Inicia a biblioteca Allegro
    al_init();

    // Inicia a biblioteca de primitivas para desenhar formas
    al_init_primitives_addon();

    // Inicia o sistema de entrada do mouse
    al_install_mouse();

    // Cria uma tela de 800x600 pixels
    ALLEGRO_DISPLAY* display = al_create_display(800, 600);

    // 60 fps
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);

    // Cria uma fila de eventos
    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();

    // Registra a fila de eventos com as fontes de eventos (display, timer, mouse)
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_mouse_event_source());

    // Inicia o temporizador
    al_start_timer(timer);

    // Cor do círculo
    ALLEGRO_COLOR circle_color = al_map_rgb(255, 0, 0);

    // Posição inicial do círculo (centro da tela)
    float circle_x = 400;
    float circle_y = 300;

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
            al_clear_to_color(al_map_rgb(0, 0, 0));

            // Desenha o círculo na nova posição
            al_draw_filled_circle(circle_x, circle_y, CIRCUFERENCIA, circle_color);

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
