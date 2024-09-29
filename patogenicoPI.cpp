#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h> // Para criar um circulo
#include <cmath> // Para utilizar o sqrt
#include <stdio.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>

int tela = 1;

bool isColliding(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2) {
    if ((x2 < x1 && x1 < x2 + width2) && (y2 < y1 && y1 < y2 + height2))
        return true;
    return false;
}



const float CIRCUFERENCIA = 50;
const float MOVE_VELO = 5.0; // Velocidade do movimento do círculo

// Posição inicial do círculo (centro da tela)
float circle_x = 400;
float circle_y = 300;

int main() {

    // Inicializa o Allegro
    al_init();
    al_init_primitives_addon(); // Inicializa o addon para primitivas gráficas


    // Inicialização dos addons
    al_init_font_addon();       // Para usar fontes simples
    al_init_ttf_addon();        // Para fontes TTF
    al_init_image_addon();      // Inicializa o addon para imagens

    // Inicia a biblioteca de primitivas para desenhar formas
    al_init_primitives_addon();

    // Inicia o sistema de entrada do mouse
    al_install_mouse();

    // 60 fps
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);

    // Criação de uma janela de 640x480 pixels
    ALLEGRO_DISPLAY* display = al_create_display(800, 600);

    // Carregar uma imagem (substitua "sua_imagem.png" pelo caminho da sua imagem)
    ALLEGRO_BITMAP* background = al_load_bitmap("img/telaTeste.png");
    ALLEGRO_BITMAP* background2 = al_load_bitmap("img/tela2Teste.png");

    if (!display) {
        printf("Erro ao criar a janela.\n");
        return -1;
    }

    // Criação da fila de eventos
    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
    if (!event_queue) {
        printf("Erro ao criar a fila de eventos.\n");
        al_destroy_display(display);
        return -1;
    }

    // Registra a janela para receber eventos
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_mouse_event_source());

    // Limpa a tela com uma cor de fundo
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_flip_display();

    // Usar a fonte embutida
    ALLEGRO_FONT* font = al_create_builtin_font();

    // Inicia o temporizador
    al_start_timer(timer);

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
        case 1:

            // Desenha a imagem de fundo na tela (na posição (0, 0))
            al_draw_bitmap(background, 0, 0, 0);

            if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
                if (ev.mouse.button == 1) { // Botão esquerdo do mouse

                    if (ev.mouse.x > 570 && ev.mouse.x < 630 && ev.mouse.y>280 && ev.mouse.y < 320) {
                        tela = 2;
                    }

                    printf("Clique detectado na posição (%d, %d)\n", ev.mouse.x, ev.mouse.y);
                }
            }
            // Desenhar o texto na tela usando a fonte embutida
            al_draw_text(font, al_map_rgb(0, 100, 0), 600, 300,ALLEGRO_ALIGN_CENTER, "jogar");

            // Atualiza o display para mostrar o texto
            al_flip_display();

            break;

        case 2:

            al_draw_bitmap(background2, 0, 0, 0);
            // Desenhar o texto na tela usando a fonte embutida
           
            if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
                if (ev.mouse.button == 1) { // Botão esquerdo do mouse

                    if (ev.mouse.y>240 && ev.mouse.y < 340) {

                        if (ev.mouse.x > 150 && ev.mouse.x < 230) {

                            tela = 3;
                        }

                        if (ev.mouse.x > 345 && ev.mouse.x < 437) {

                            tela = 3;
                        }

                        if (ev.mouse.x > 544 && ev.mouse.x < 627) {

                            tela = 3;
                        }
                    }

                    printf("Clique detectado na posição (%d, %d)\n", ev.mouse.x, ev.mouse.y);
                }
            }

            // Atualiza o display para mostrar o texto
            al_flip_display();
            break;

        case 3:


            al_clear_to_color(al_map_rgb(0, 0, 0));

            // Cor do círculo
            ALLEGRO_COLOR circle_color = al_map_rgb(255, 0, 0);


            


            // Verifica se o evento é do temporizador
            if (ev.type == ALLEGRO_EVENT_TIMER) {
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
       
            break;

            // Você pode ter quantos casos quiser

        default:
            // Código a ser executado se nenhum caso for correspondente
            break;

        }
    }

    // Limpeza dos recursos
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_bitmap(background); 
    al_destroy_bitmap(background2);

    return 0; // Encerrar o programa corretamente
}
