#include "interact.h"
#include "speech.h"
#include "map.h"
#include "graphics.h"



#include "globals.h"




void plant_interact(void * data, int xOffset, int yOffset,Player player){


}

void holocron_interact(void * data, int xOffset, int yOffset,Player player){
    
    int objX = player.x + xOffset;
    int objY = player.y + yOffset;
    map_erase(objX,objY);
    holocrons_collected++;

    // Draw status bars    
    draw_lower_status(player.health,holocrons_collected);


}

void palpatine_interact(void * data, int xOffset, int yOffset,Player player){
    if (!palpatine_initial_talk){
        long_speech("Lord Vader I need you to infiltrate the Jedi temple and collect the holocrons. Afterwards I will reward you my young apprentice");
        palpatine_initial_talk = 1;
    }


    if (holocrons_collected >= 10 && !palpatine_key_talk){
        long_speech("Here is the key to the forbidden Sith temple");
        add_key(player.x,player.y + 1);
        draw_game(true);
        palpatine_key_talk = 1;

    }else{
        long_speech("Well, go on");

    }




}

void trooper_interact(void * data, int xOffset, int yOffset,Player player){

    long_speech("Lord Vader Sir");

}

void door_interact(void * data, int xOffset, int yOffset,Player player){

    int doorType =  (int)(((MapItem*)data)->data);


    if (palpatine_initial_talk && !holocrons_collected && doorType == JEDITEMPLEDOOR){

        uLCD.cls();
        map_destroy();
        init_second_map();
        draw_game(true);
        
        long_speech("Take down the Jedi and destroy their library");



    }else{

        if (!player.has_key){
            if (doorType == JEDITEMPLEDOOR){
                long_speech("No key to unlock temple door");

            }else if (doorType == SITHTEMPLEDOOR){

                uLCD.cls();
                map_destroy();
                init_third_map();
                draw_game(true);
                long_speech("You now enter the ancient Sith temple.");
            }else{
                long_speech("No key to unlock temple door");
            }

        }
    }


}


void health_interact(void * data, int xOffset, int yOffset,Player player){

    int health =  (int)(((MapItem*)data)->data);
    int newHealth = health - 50;
    
    (((MapItem*)data)->data) = (void *)newHealth;

    if (newHealth <= 0 ){
        int objX = player.x + xOffset;
        int objY = player.y + yOffset;
        map_erase(objX,objY);
        draw_explosion(PLAYER_POS_X + 11*xOffset,PLAYER_POS_Y + 11*yOffset);
        add_holocron(objX,objY);
        draw_game(true);

    }    

    if (holocrons_collected >= 10){
        long_speech("Nice job return back to the ship for I have something to show you");
        uLCD.cls();
        map_destroy();
        init_main_map();
        draw_game(true);
        palpatine_walkin();

    }


}

void jedi_interact(void * data, int xOffset, int yOffset,Player player){
    health_interact(data,xOffset,yOffset,player);

}

void library_interact(void * data, int xOffset, int yOffset,Player player){
    health_interact(data,xOffset,yOffset,player);
}


void key_interact(void * data, int xOffset, int yOffset,Player player){
    player.has_key = true;
    map_erase(player.x + xOffset,player.y + yOffset);
    long_speech("Key has been received. Unlock the temple doors");

}

void sith_elder_interact(void * data, int xOffset, int yOffset,Player player){
    long_speech("Congratulations Lord Vader you have successfully defeated the Jedi. We will reward you with powers no Jedi will every be capable of having");
    draw_gameover();    
}
