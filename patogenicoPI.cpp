#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h> // Para criar um circulo
#include <time.h>
#include <stdio.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <math.h>

int tela = 1;
int telaAnterior = 0;
int width = 800;
int height = 600;
int teste = 0;
int cron = 20;
int cronP = 20; // para o cronometro do viremia 
char cron_str[10];
int tempo = 0;

#define gameOver 0
#define telaLoading 1
#define telaInicial 2
#define seletorFase 3
#define ataqueMosquito 4
#define fagocitose 5
#define viremia 6
#define venceuViremia 7
#define perdeu 8


void desenhar_caixa_dialogo(int caixaX, int caixaY, int caixaLargura, int caixaAltura, ALLEGRO_FONT* font, const char* texto,int* tempo) {
    // Variável estática para persistir o valor do tempo entre as chamadas
  //  static int tempo = 0;

    (*tempo) += 1;  // Incrementa o tempo a cada chamada
    printf("%d\n", *tempo);

    // Desenha a caixa de diálogo preenchida
    al_draw_filled_rectangle(caixaX, caixaY, caixaX + caixaLargura, caixaY + caixaAltura, al_map_rgb(50, 50, 50));

    // Desenha a borda da caixa de diálogo
    al_draw_rectangle(caixaX, caixaY, caixaX + caixaLargura, caixaY + caixaAltura, al_map_rgb(255, 255, 255), 2);

    // Desenha o texto dentro da caixa de diálogo (com offset de 20 e 25 para centralização)
    al_draw_text(font, al_map_rgb(255, 255, 255), caixaX + 20, caixaY + (caixaAltura / 2) - (al_get_font_line_height(font) / 2), 0, texto);

    // Atualiza a tela
    al_flip_display();

    // Quando o tempo for maior ou igual a 5, sai da função
    if (*tempo >= 2) {
        printf("Tempo atingido, saindo da função.\n");
        return;
    }
}

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

    // Inicia o sistema de entrada do mouse e teclado
    al_install_mouse();
    al_install_keyboard();

    // 60 fps
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);

    ALLEGRO_TIMER* countdown_timer = al_create_timer(1.0);

    // Criação de uma janela de 640x480 pixels
    ALLEGRO_DISPLAY* display = al_create_display(width, height);

    // Carregar uma imagem (substitua "sua_imagem.png" pelo caminho da sua imagem)
    ALLEGRO_BITMAP* loading = al_load_bitmap("img/telaLoading.png");
    ALLEGRO_BITMAP* background = al_load_bitmap("img/telaTeste.png");
    ALLEGRO_BITMAP* background2 = al_load_bitmap("img/tela2Teste.png");
    ALLEGRO_BITMAP* backgroundViremia = al_load_bitmap("img/backgroundViremia.png");
    ALLEGRO_BITMAP* virusViremia = al_load_bitmap("img/virus.png");
    ALLEGRO_BITMAP* telaPerdeu = al_load_bitmap("img/telaPerdeu.png");

    //carrega uma fonte que foi baixada
    ALLEGRO_FONT* fonte_20 = al_load_font("img/fonte.TTF", 20, 0);


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
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_timer_event_source(countdown_timer));


    // Inicia o timer para o cronômetro em viremia
    al_start_timer(countdown_timer);

    // Limpa a tela com uma cor de fundo
    al_clear_to_color(al_map_rgb(0, 0, 0));
    al_flip_display();

    // Usar a fonte embutida
    ALLEGRO_FONT* font = al_create_builtin_font();

    // Inicia o temporizador
    al_start_timer(timer);

    // - - - - - - - VARIÁVEIS PARA A TELA LOADING - - - - - - -
    int contadorDeFrames = 0;
    const int frameSegundo = 60;
    const int intervaloSegundo = 2;

    int opacidadeTexto = 255;

    const int RECT_WIDTH = 8;
    const int RECT_HEIGHT = 10;
    const int DISTANCE = 2; // Distância entre os retângulos
    ALLEGRO_COLOR BLUE = al_map_rgb(0, 0, 255); // Cor azul definida

    typedef struct {
        float x, y;
        int w, h;
    } Retangulo;

    // Lista de retângulos
    Retangulo retangulos[100];
    int num_retangulos = 1;

    // Primeiro retângulo
    retangulos[0].x = 380;
    retangulos[0].y = 295;
    retangulos[0].w = RECT_WIDTH;
    retangulos[0].h = RECT_HEIGHT;
    //  - - - - - - - FIM DAS VARIÁVEIS PARA A TELA LOADING - - - - - - -

    // - - - - - - - VARIÁVEIS PARA VIREMIA - - - - - - -
    //Coordenadas pré estabelecidas 
    int x1 = 50, y1 = 535;
    int x2 = 740, y2 = 50;

    // Posição inicial do círculo
    int circle_x = x1 + 10;
    int circle_y = y1 + 10;

    int xChegada;
    int yChegada;

    bool startJogo = false;
    bool mudouDeNivel = false;
    int nivelViremia = 1;

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
        case telaLoading:

            telaAnterior = tela;

            // Desenha a imagem de fundo na tela (na posição (0, 0))
            al_draw_bitmap(loading, 0, 0, 0);

            if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
                if (ev.keyboard.keycode == ALLEGRO_KEY_SPACE) {
                    //Tecla espaço pressionada
                    tela = telaInicial;
                }
            }

            if (ev.type == ALLEGRO_EVENT_TIMER) {
                contadorDeFrames++;
                //Verifica o timer a cada meio segundo
                if (contadorDeFrames % (frameSegundo / intervaloSegundo) == 0) {
                    // Criar um novo retângulo
                    if (num_retangulos <= 7) {
                        /*Calcula a posição x do novo retângulo,
                        baseada na posição x do último,
                        adicionando a largura e a distância entre eles.*/
                        retangulos[num_retangulos].x = retangulos[num_retangulos - 1].x + RECT_WIDTH + DISTANCE;
                        retangulos[num_retangulos].y = retangulos[0].y; // Manter a mesma posição
                        retangulos[num_retangulos].w = RECT_WIDTH;
                        retangulos[num_retangulos].h = RECT_HEIGHT;
                        num_retangulos++;
                    }
                    opacidadeTexto = opacidadeTexto * (-1);
                }
            }

            // Desenha todos os retângulos armazenados no array retangulos
            for (int i = 0; i < num_retangulos; i++) {
                al_draw_filled_rectangle(retangulos[i].x, retangulos[i].y,
                    retangulos[i].x + retangulos[i].w,
                    retangulos[i].y + retangulos[i].h,
                    BLUE);
            }
            if (num_retangulos >= 8) {
                // Desenha o texto alterando a cor entre preto e branco a cada 2 segundos
                al_draw_textf(font, al_map_rgb(opacidadeTexto, opacidadeTexto, opacidadeTexto), 400, 565, ALLEGRO_ALIGN_CENTER, "APERTE A TECLA 'ESPAÇO' PARA INICIAR");
            }

            al_flip_display();

            break;
        case telaInicial:

            telaAnterior = tela;

            // Desenha a imagem de fundo na tela (na posição (0, 0))
            al_draw_bitmap(background, 0, 0, 0);

            if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
                if (ev.mouse.button == 1) { // Botão esquerdo do mouse

                    if (ev.mouse.x > 570 && ev.mouse.x < 630 && ev.mouse.y>280 && ev.mouse.y < 320) {
                        tela = seletorFase;
                    }

                    printf("Clique detectado na coordenada (%d, %d)\n", ev.mouse.x, ev.mouse.y);
                }
            }
            // Desenhar o texto na tela usando a fonte embutida
            al_draw_text(font, al_map_rgb(0, 0, 0), 600, 300, ALLEGRO_ALIGN_CENTER, "jogar");

            // Atualiza o display para mostrar o texto
            al_flip_display();

            break;

        case seletorFase:

            telaAnterior = tela;

            al_draw_bitmap(background2, 0, 0, 0);
            // Desenhar o texto na tela usando a fonte embutida

            if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
                if (ev.mouse.button == 1) { // Botão esquerdo do mouse

                    if (ev.mouse.y > 240 && ev.mouse.y < 340) {

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

                    tempo = 0;

                    printf("Clique detectado na coordenada (%d, %d)\n", ev.mouse.x, ev.mouse.y);
                }
            }

            // Atualiza o display para mostrar o texto
            al_flip_display();
            break;

        case viremia:

            telaAnterior = tela;
            // Desenha a imagem de fundo
            al_draw_bitmap(backgroundViremia, 0, 0, 0);

            while (tempo < 3) {
                desenhar_caixa_dialogo(50, 50, 400, 60, font, "Aguarde 5 segundos...", &tempo);

                cron = cronP;

                // Atualiza a tela e faz outras operações, se necessário
                al_flip_display();

                // Espera um segundo para dar tempo para o tempo ser incrementado
                al_rest(1.0);
            }

            // Verifica se o evento é do temporizador
            if (ev.type == ALLEGRO_EVENT_TIMER) {

                if (ev.type == ALLEGRO_EVENT_TIMER && ev.timer.source == countdown_timer) {


                    if (cron > 0) {
                        cron = cron - 1;  // Decrementa o cronômetro
                    }
                }
               
                
              


                if (cron == 0) {
                    al_draw_text(font, al_map_rgb(255, 255, 255), 100, 200, ALLEGRO_ALIGN_CENTER, "GAME OVER");
                    cron = cronP;
                    circle_x = x1 + 10;
                    circle_y = y1 + 10;
                }


                // Estado do mouse
                ALLEGRO_MOUSE_STATE mState;
                al_get_mouse_state(&mState);

                // Verifica se o mouse está sobre o círculo
                if (mState.x >= circle_x - 10 && mState.x <= circle_x + 10 &&
                    mState.y >= circle_y - 10 && mState.y <= circle_y + 10) {

                    startJogo = true;
                }
                else {
                    startJogo = false;
                }
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
                        cron = cronP;

                        tela = perdeu;
                        circle_x = x1 + 10;
                        circle_y = y1 + 10;
                    }
                    if (nivelViremia == 1 || nivelViremia == 3) {
                        xChegada = x2;
                        yChegada = y2;
                    }
                    else if (nivelViremia == 2) {
                        xChegada = x1;
                        yChegada = y1;
                    }
                    if (nivelViremia < 3 && (circle_x >= xChegada && circle_x <= xChegada + 20 && circle_y >= yChegada && circle_y <= yChegada + 20)) {
                        mudouDeNivel = true;
                    }
                    if (mudouDeNivel == true) {
                        geracoordenadasX(coordenadaX, tamanho, x1, x2);
                        geracoordenadasY(coordenadaY, tamanho, y1, y2);
                        //Diminui a espessura da linha
                        espessuraLinha -= espessuraLinha * 0.2;
                        nivelViremia++;

                        // cronometro vai receber os segundos dnv
                        cron = cronP;

                        //Reseta mudouDeNivel
                        mudouDeNivel = false;
                    }
                    if (nivelViremia == 3 && (circle_x >= x2 && circle_x <= x2 + 20 && circle_y >= y2 && circle_y <= y2 + 20)) {
                        tela = venceuViremia;
                    }
                }
                // Desenhar o texto na tela usando a fonte embutida
                al_draw_textf(font, al_map_rgb(255, 255, 255), 700, 10, ALLEGRO_ALIGN_CENTER, "Nível %d/3", nivelViremia);

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

                al_wait_for_event(event_queue, &ev);




                sprintf_s(cron_str, "%d", cron);

                al_draw_text(font, al_map_rgb(255, 255, 255), 100, 100, ALLEGRO_ALIGN_CENTER, cron_str);

                al_draw_text(font, al_map_rgb(255, 255, 255), 50, 50, ALLEGRO_ALIGN_CENTER, "VOLTAR");

                if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
                    if (ev.mouse.button == 1) { // Botão esquerdo do mouse

                        if (ev.mouse.x > 25 && ev.mouse.x < 75 && ev.mouse.y>45 && ev.mouse.y < 60) {
                            tela = 2;
                        }

                        printf("Clique detectado na coordenada (%d, %d)\n", ev.mouse.x, ev.mouse.y);
                    }
                }

                // Atualiza a tela
                al_flip_display();
            }

            break;

         case perdeu:


             al_draw_bitmap(telaPerdeu, 0, 0, 0);

             if (ev.type == ALLEGRO_EVENT_TIMER) {

                 if (ev.type == ALLEGRO_EVENT_TIMER && ev.timer.source == countdown_timer) {


                     if (tempo>=0) {
                         tempo = tempo + 1;  // Decrementa o cronômetro
                         printf("%d\n", tempo);
                     }
                 }
             }

             if (tempo % 2 == 1) {
                
                 al_draw_text(fonte_20, al_map_rgb(255, 255, 255), 370, 550,-20, "S/N");
             }else{
                 al_draw_text(fonte_20, al_map_rgb(255, 0, 0), 370, 550, 0, "S/N");
             }


             if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {

                
                 if (ev.keyboard.keycode == ALLEGRO_KEY_S) {
                     tela = telaAnterior;
                    
                 }
                 else if (ev.keyboard.keycode == ALLEGRO_KEY_N) {
                     tela = telaInicial;
                 }
             }

             al_flip_display();



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
    al_destroy_bitmap(telaPerdeu);
    al_destroy_font(fonte_20);

    // Libera a memória alocada
    free(coordenadaX);
    free(coordenadaY);

    return 0; // Encerrar o programa corretamente
}
