#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h> // Para criar um circulo
#include <time.h>
#include <stdio.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>

int tela = 1;
int width = 800;
int height = 600;

void geracoordenadasX(int* coordenadaX, int tamanho, int x1, int x2) {
    coordenadaX[0] = x1;
    coordenadaX[tamanho - 1] = x2;
    for (int i = 1; i < tamanho - 1; i++) {
        coordenadaX[i] = 20 + rand() % (width - 40);
    }
}

void geracoordenadasY(int* coordenadaY, int tamanho, int y1, int y2) {
    coordenadaY[0] = y1;
    coordenadaY[tamanho - 1] = y2;
    for (int i = 1; i < tamanho - 1; i++) {
        coordenadaY[i] = 20 + rand() % (height - 40);
    }
}

void gerarLinhas(int* coordenadaX, int* coordenadaY, int tamanho) {
    for (int i = 0; i < tamanho - 1; i++) {
        al_draw_line(coordenadaX[i], coordenadaY[i], coordenadaX[i + 1], coordenadaY[i + 1], al_map_rgb(119, 25, 15), 10.0);
    };
}

int main() {
    srand(time(NULL)); //inicializa a semente para números aleatórios

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
    ALLEGRO_DISPLAY* display = al_create_display(width, height);

    // Carregar uma imagem (substitua "sua_imagem.png" pelo caminho da sua imagem)
    ALLEGRO_BITMAP* background = al_load_bitmap("img/telaTeste.png");
    ALLEGRO_BITMAP* background2 = al_load_bitmap("img/tela2Teste.png");
    ALLEGRO_BITMAP* backgroundViremia = al_load_bitmap("img/backgroundViremia.png");

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

    //Variáveis para o case Viremia
    int x1 = 50, y1 = 535;
    int x2 = 740, y2 = 50;

    float circle_x = x1 + 10; // Posição inicial do círculo
    float circle_y = y1 + 10;

    bool minigame3 = false;
    bool gameOver = false;

    int tamanho = 4;
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
    //fim das variáveis para o case Viremia

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

                    printf("Clique detectado na coordenada (%d, %d)\n", ev.mouse.x, ev.mouse.y);
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

                    printf("Clique detectado na coordenada (%d, %d)\n", ev.mouse.x, ev.mouse.y);
                }
            }

            // Atualiza o display para mostrar o texto
            al_flip_display();
            break;

        case 3:
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

                    minigame3 = true;
                }

                if (minigame3 == true) {
                    circle_x = mState.x; // Atualiza a posição do círculo para a posição do mouse
                    circle_y = mState.y;
                }

                gerarLinhas(coordenadaX, coordenadaY, tamanho);


                al_draw_filled_rectangle(x1, y1, x1 + 20, y1 + 20, al_map_rgb(255, 255, 255));
                al_draw_filled_rectangle(x2, y2, x2 + 20, y2 + 20, al_map_rgb(255, 255, 255));
                // Desenha o círculo na nova posição
                al_draw_filled_circle(circle_x, circle_y, 10, al_map_rgb(255, 0, 0));

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
    al_destroy_bitmap(backgroundViremia);

    // Libera a memória alocada
    free(coordenadaX);
    free(coordenadaY);

    return 0; // Encerrar o programa corretamente
}
