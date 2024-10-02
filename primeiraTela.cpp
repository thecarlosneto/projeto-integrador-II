#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h> // Para criar um circulo
#include <cmath> // Para utilizar o sqrt
#include <allegro5/allegro_image.h>

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

    //////////////////////////////////////////////////////////////////////////////////////
  
    // Variável representando a imagem
    ALLEGRO_BITMAP* imagem = NULL;

    // Inicializa o add-on para utilização de imagens
    al_init_image_addon();

    // Carrega a imagem
    imagem = al_load_bitmap("img/telaTest.png");

    // Desenha a imagem na tela
        al_draw_bitmap(imagem, 0, 0, 0);

        printf("testeeeeeeeeeeee");

        // Atualiza a tela
        al_flip_display();

        // Segura a execução por 10 segundos
        al_rest(10.0);

    //////////////////////////////////////////////////////////////////////////////////////
    
    // Limpa os recursos alocados
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);
    al_destroy_timer(timer);

    return 0;
}