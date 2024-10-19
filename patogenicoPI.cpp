#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h> // Para criar um circulo
#include <time.h>
#include <stdio.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <math.h>

int tela = 1;
int width = 800;
int height = 600;

#define gameOver 0
#define telaInicial 1
#define seletorFase 2
#define ataqueMosquito 3
#define fagocitose 4
#define viremia 5


/*Função que gera coordenadas de X aleatorias com base no tamanho do display,
  preenchendo um array. Onde a primeira e a última posição já tem coordenadas
  pré estabelecidas*/
void geracoordenadasX(int* coordenadaX, int tamanho, int x1, int x2) {
    coordenadaX[0] = x1;
    coordenadaX[tamanho - 1] = x2;
    for (int i = 1; i < tamanho - 1; i++) {
        coordenadaX[i] = 20 + rand() % (width - 40);
    }
}

/*Função que gera coordenadas de Y aleatorias com base no tamanho do display,
  preenchendo um array. Onde a primeira e a última posição já tem coordenadas
  pré estabelecidas*/
void geracoordenadasY(int* coordenadaY, int tamanho, int y1, int y2) {
    coordenadaY[0] = y1;
    coordenadaY[tamanho - 1] = y2;
    for (int i = 1; i < tamanho - 1; i++) {
        coordenadaY[i] = 20 + rand() % (height - 40);
    }
}

//Função que gera retas colineares de acordo com as coordenadas 
void gerarLinhas(int* coordenadaX, int* coordenadaY, int tamanho, int espessuraLinha) {
    for (int i = 0; i < tamanho - 1; i++) {
        al_draw_line(coordenadaX[i], coordenadaY[i], coordenadaX[i + 1], coordenadaY[i + 1], al_map_rgb(119, 25, 15), espessuraLinha);
    };
}

// Função para verificar se o círculo está sobre uma linha com espessura
bool foraDaLinha(float x, float y, float x1, float y1, float x2, float y2, float espessura) {
    // Calcular a distância perpendicular do ponto à reta
    float A = y2 - y1;
    float B = x1 - x2;
    float C = x2 * y1 - x1 * y2;
    float distancia = fabs(A * x + B * y + C) / sqrt(A * A + B * B);

    return distancia > espessura / 2;
}

#define PI 3.14159265358979323846
//função que cria linhas onduladas por meio do calculo da Função Seno
void linhasOnduladas(float x1, float y1, float x2, float y2, int qtdOndas) {
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

// Funções de movimento da mão
float movimento_linear(float x, float amplitude) {
    return height / 2;
}

float movimento_senoidal(float x, float amplitude) {
    return (height / 2) + amplitude * sin((x / width) * (2 * PI)) * 0.5; // Reduz a amplitude para não sair da tela
}

float movimento_cosenoidal(float x, float amplitude) {
    return (height / 2) + amplitude * cos((x / width) * (2 * PI)) * 0.5; // Reduz a amplitude para não sair da tela
}

float movimento_inverso_senoidal(float x, float amplitude) {
    return (height / 2) - amplitude * sin((x / width) * (2 * PI)) * 0.5;
}

float movimento_inverso_cosenoidal(float x, float amplitude) {
    return (height / 2) - amplitude * cos((x / width) * (2 * PI)) * 0.5;
}

float (*padroesMovimento[5])(float, float) = {
    movimento_linear,
    movimento_senoidal,
    movimento_cosenoidal,
    movimento_inverso_senoidal,
    movimento_inverso_cosenoidal
};


int main() {
    srand(time(NULL)); //inicializa a semente para números aleatórios

    // Inicializa o Allegro
    al_init();
    al_init_primitives_addon(); // Inicializa o addon para primitivas gráficas


    // Inicialização dos addons
    al_init_font_addon();       // Para usar fontes simples
    al_init_ttf_addon();        // Para fontes TTF
    al_init_image_addon();      // Inicializa o addon para imagens
    al_install_keyboard();      // Inicializa o teclado

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
    ALLEGRO_BITMAP* virusViremia = al_load_bitmap("img/virus.png");
    ALLEGRO_BITMAP* mosquito = al_load_bitmap("img/mosquito.png");


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

    // - - - - - - - VARIÁVEIS PARA VIREMIA - - - - - - -
    //Coordenadas pré estabelecidas 
    int x1 = 50, y1 = 535;
    int x2 = 740, y2 = 50;

    // Posição inicial do círculo
    int circle_x = x1 + 10;
    int circle_y = y1 + 10;

    bool startJogo = false;

    int tamanho = 4;

    int espessuraLinha = 15.0;
    bool dentroDaLinha = false;
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

    // Variáveis para o movimento circular
    float angle = 0; //variável de ângulo será incrementada a cada frame para simular o movimento circular
    float radius = 50;
    // - - - - - - -FIM DAS VARIÁVEIS PARA VIREMIA - - - - - - -

    // - - - - - - - VARIÁVEIS PARA MOSQUITO - - - - - - -

    int mosquito_x = 600; // Posição fixa em X
    int mosquito_y = 0; // Declaração de Y

    float mao_x = 0; // Posição fixa em X
    float mao_y = rand() % (height - 20) + 10; // Posição inicial aleatória em Y
    float velocidade_x = (rand() % 6 + 5);  // Define uma velocidade aleatória inicial entre 5 e 10
    float amplitude; // Amplitude da parábola
    float offset; // Deslocamento Y inicial
    float tempo = 0; // Tempo para calcular a posição

    // Inicializar a amplitude e o padrão de movimento
    amplitude = rand() % (height / 2) + (height / 2); // Amplitude
    int indice_padroes = 0; // Índice do padrão de movimento

    // - - - - - - -FIM DAS VARIÁVEIS PARA MOSQUITO - - - - - - -
    
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
        case telaInicial:

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

        case seletorFase:

            al_draw_bitmap(background2, 0, 0, 0);
            // Desenhar o texto na tela usando a fonte embutida
           
            if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
                if (ev.mouse.button == 1) { // Botão esquerdo do mouse

                    if (ev.mouse.y>240 && ev.mouse.y < 340) {

                        if (ev.mouse.x > 150 && ev.mouse.x < 230) {

                            tela = ataqueMosquito;
                        }

                        if (ev.mouse.x > 345 && ev.mouse.x < 437) {

                            tela = fagocitose;
                        }

                        if (ev.mouse.x > 544 && ev.mouse.x < 627) {

                            tela = viremia;
                        }
                    }

                    printf("Clique detectado na coordenada (%d, %d)\n", ev.mouse.x, ev.mouse.y);
                }
            }

            // Atualiza o display para mostrar o texto
            al_flip_display();
            break;

        case viremia:
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

                    startJogo = true;
                };
                // Reiniciar a cada iteração
                dentroDaLinha = false;

                //Fase viremia começou
                if (startJogo == true) {
                    circle_x = mState.x;
                    circle_y = mState.y;
                    //Verifica se o círculo está em cima de alguma linha ou dos quadrados brancos
                    for (int i = 0; i < tamanho - 1; i++) {
                        if (!foraDaLinha(circle_x, circle_y, coordenadaX[i], coordenadaY[i], coordenadaX[i + 1],
                            coordenadaY[i + 1], espessuraLinha)
                            || circle_x >= x1 && circle_x <= x1 + 20 && circle_y >= y1 && circle_y <= y1 + 20
                            || circle_x >= x2 && circle_x <= x2 + 20 && circle_y >= y2 && circle_y <= y2 + 20) {
                            dentroDaLinha = true;
                            break; // Se o cursor estiver sobre uma linha, não precisa verificar as demais
                        }
                    }

                    //Fora da linha
                    if (!dentroDaLinha) {
                        //deixar comentado por enquanto => tela = gameOver;
                        al_draw_text(font, al_map_rgb(255, 255, 255), 100, 200, ALLEGRO_ALIGN_CENTER, "GAME OVER");
                    }

                }
                //Desenha as linhas chamando a função
                gerarLinhas(coordenadaX, coordenadaY, tamanho, espessuraLinha);

                //Desenha os quadrados brancos
                al_draw_filled_rectangle(x1, y1, x1 + 20, y1 + 20, al_map_rgb(255, 255, 255));
                al_draw_filled_rectangle(x2, y2, x2 + 20, y2 + 20, al_map_rgb(255, 255, 255));

                // Calcula a posição da imagem no círculo
                float x = circle_x + radius * cos(angle);
                float y = circle_y + radius * sin(angle);

                //Desenha as linhas onduladas a partir da Função seno
                linhasOnduladas(circle_x, circle_y, x, y, 40);

                // Desenha a imagem
                al_draw_bitmap(virusViremia, x - al_get_bitmap_width(virusViremia) / 2, y - al_get_bitmap_height(virusViremia) / 2, 0);

                // Desenha o círculo na nova posição
                al_draw_filled_circle(circle_x, circle_y, 10, al_map_rgb(255, 0, 0));

                // Incrementa o ângulo
                angle += 0.03;

                // Atualiza a tela
                al_flip_display();
            }
       
            break;


        case ataqueMosquito:
            // Desenha a imagem de fundo
            al_draw_bitmap(background2, 0, 0, 0);

            // Checa o estado do teclado
            ALLEGRO_KEYBOARD_STATE kState;
            al_get_keyboard_state(&kState);

            // Atualiza a posição do mosquito com base nas teclas A e D
            if (al_key_down(&kState, ALLEGRO_KEY_A)) {
                mosquito_y -= 5; // Move para cima
            }
            if (al_key_down(&kState, ALLEGRO_KEY_D)) {
                mosquito_y += 5; // Move para baixo
            }

            // Limita a posição do mosquito dentro da tela
            if (mosquito_y < 0) {
                mosquito_y = 0;
            }
            if (mosquito_y > height - al_get_bitmap_height(mosquito)) {
                mosquito_y = height - al_get_bitmap_height(mosquito);
            }

            // Atualiza a posição da mão
            mao_x += velocidade_x;

            // Verifica se a mão saiu da tela
            if (mao_x > width) {
                // Resetando a mão para o início
                mao_x = 0;

                // Alterna para o próximo padrão
                indice_padroes = (indice_padroes + 1) % 5;

                // Atualiza a amplitude para o novo padrão
                amplitude = rand() % (height / 2) + (height / 2);

                // Nova posição Y aleatória
                mao_y = rand() % (height - 20) + 10;

                // Atribui uma nova velocidade aleatória à mão entre 5 e 10
                velocidade_x = (rand() % 6 + 5); // Gera um número aleatório entre 5 e 10
            }

            // Calcula a posição Y da mão usando o padrão atual
            mao_y = padroesMovimento[indice_padroes](mao_x, amplitude);

            // Verifica colisão com o mosquito
            if (mao_x + 10 >= mosquito_x && mao_x - 10 <= mosquito_x + al_get_bitmap_width(mosquito) &&
                mao_y + 10 >= mosquito_y && mao_y - 10 <= mosquito_y + al_get_bitmap_height(mosquito)) {
                mosquito_x -= 200; // Lógica de colisão
            }

            // Desenha a mão
            al_draw_filled_circle(mao_x, mao_y, 10, al_map_rgb(255, 255, 0)); // Mão

            // Desenha o mosquito
            al_draw_bitmap(mosquito, mosquito_x, mosquito_y, 0);

            // Atualiza o display
            al_flip_display();
            break;

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
