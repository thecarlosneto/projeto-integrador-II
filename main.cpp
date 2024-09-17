#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h> 

int main() {
    // inicia biblioteca Allegro
    if (!al_init()) {
        return -1;
    }

    // Inicia para desenhar formas
    if (!al_init_primitives_addon()) {
        return -1;
    }

    // tela 800x600 pixels
    ALLEGRO_DISPLAY* display = al_create_display(800, 600);
    if (!display) {
        return -1;
    }

    // cor do fundo
    al_clear_to_color(al_map_rgb(0, 0, 0));

    // cor - círculo 
    ALLEGRO_COLOR circle_color = al_map_rgb(255, 0, 0);

    // Desenha um círculo na tela
    // padrão => al_draw_filled_circle(x, y, radius, color)
    al_draw_filled_circle(400, 300, 100, circle_color);

    // Atualiza a tela para mostrar as mudanças
    al_flip_display();

    // Mantém a janela aberta por 5 segundos
    al_rest(5.0);

    // Destroi a janela
    al_destroy_display(display);

    return 0;
}
