#include <stdio.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>



bool isColliding(int x1, int y1, int width1, int height1, int x2, int y2, int width2, int height2) {
    if ((x2 < x1 && x1 < x2 + width2) && (y2 < y1 && y1 < y2 + height2))
        return true;
    return false;
}

int main()
{
    al_init();
    al_install_keyboard();
    al_init_primitives_addon();

    int dispX = 800;
    int dispY = 600;
    int cRaio = 20;

    ALLEGRO_MOUSE_STATE mState;

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    ALLEGRO_DISPLAY* disp = al_create_display(dispX, dispY);
    ALLEGRO_FONT* font = al_create_builtin_font();

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    bool redraw = true;
    ALLEGRO_EVENT event;
    al_install_mouse();

    al_start_timer(timer);
    while (1)
    {
        al_wait_for_event(queue, &event);

        al_get_mouse_state(&mState);

        if (event.type == ALLEGRO_EVENT_TIMER)
            redraw = true;
        else if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            break;
        else if (event.type == ALLEGRO_EVENT_KEY_DOWN){}


        if (redraw && al_is_event_queue_empty(queue))
        {
            al_clear_to_color(al_map_rgb(40, 40, 40));
            al_draw_text(font, al_map_rgb(255, 255, 255), 0, 100, 0, "estou montando arapucas e engenhocas");
            al_draw_filled_triangle(35, 350, 85, 375, 35, 400, al_map_rgb_f(1, 0, 1));
            al_draw_filled_circle(150, 150, 20, al_map_rgb_f(0.5, 0.5, 1));
            if (isColliding(mState.x, mState.y, 1, 1, 150-cRaio, 150-cRaio, cRaio * 2, cRaio * 2))
                cRaio++;

            al_draw_filled_circle(mState.x, mState.y, cRaio, al_map_rgb_f(1, 1, 1));
            al_flip_display();
        
            redraw = false;
        }
    }

    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}