#include "drawer.h"
#include "map.h"
#include "globals.h"



Player player;


/**
 * Given the game inputs, determine what kind of update needs to happen.
 * Possbile return values are defined below.
 */
#define NO_ACTION 0
#define ACTION_BUTTON 1
#define MENU_BUTTON 2
#define GO_LEFT 3
#define GO_RIGHT 4
#define GO_UP 5
#define GO_DOWN 6




int get_action(GameInputs inputs)
{



    if (!PauseSensors){

        if (inputs.b3){
            GameMode = (GameMode + 1) % 2;
        }

        if (inputs.b1){
            int x, y;
            MapItem* item = getAllDirections(player.x,player.y, &x, &y);
            if (item){
                PauseSensors = 1;
                while(!button1.read()){}
                if (item->interact){
                    item->interact(item,x,y, player);
                }
                PauseSensors = 0;

            }


        }


        if (inputs.ay > 0.5){
            return GO_RIGHT;
        } 
        if (inputs.ay < -0.5){
            return GO_LEFT;
        }

        if (inputs.ax > 0.5){
            return GO_UP;
        }
        if (inputs.ax < -0.5){
            return GO_DOWN;
        }
    }

    return NO_ACTION;
}

void playerItemCheck(){

    MapItem* item = get_here(player.x, player.y);
    // pc.printf("Player pos (%d, %d)\r\n",u,v);

    if (item != NULL && item->walkable){
        draw_nothing(PLAYER_POS_X,PLAYER_POS_Y);
        draw_player(PLAYER_POS_X, PLAYER_POS_Y, player.has_key);
        if (item->interact){
            item->interact(item, 0, 0, player);
        }
    }

}

/**
 * Update the game state based on the user action. For example, if the user
 * requests GO_UP, then this function should determine if that is possible by
 * consulting the map, and update the Player position accordingly.
 * 
 * Return values are defined below. FULL_DRAW indicates that for this frame,
 * draw_game should not optimize drawing and should draw every tile, even if
 * the player has not moved.
 */
#define NO_RESULT 0
#define GAME_OVER 1
#define FULL_DRAW 2
int update_game(int action)
{

    draw_player(PLAYER_POS_X, PLAYER_POS_Y, player.has_key);

    if (!PauseSensors){
        // Save player previous location before updating
        player.px = player.x;
        player.py = player.y;
        playerItemCheck();
    }
    // Do different things based on the each action.
    // You can define functions like "go_up()" that get called for each case.
    switch(action)
    {
        case GO_UP:
            if (get_north(player.x, player.y) == NULL || GameMode || get_north(player.x, player.y)->walkable ){
                player.y += 1;
            }    
            break;
        case GO_LEFT:   
            if (get_west(player.x, player.y) == NULL || GameMode || get_west(player.x, player.y)->walkable){
                player.x -= 1;
            }    
            break;         
        case GO_DOWN:   
            if (get_south(player.x, player.y) == NULL || GameMode || get_south(player.x, player.y)->walkable){
                player.y -= 1;
            }    
            break;
        case GO_RIGHT:  
            if (get_east(player.x, player.y) == NULL || GameMode || get_east(player.x, player.y)->walkable){
                player.x += 1;
            }    
            break;   
        case ACTION_BUTTON: 
            break;
        case MENU_BUTTON: 
            break;
        default:        
            break;
    }




    return NO_RESULT;
}

/**
 * Entry point for frame drawing. This should be called once per iteration of
 * the game loop. This draws all tiles on the screen, followed by the status 
 * bars. Unless init is nonzero, this function will optimize drawing by only 
 * drawing tiles that have changed from the previous frame.
 */
void draw_game(int init)
{
    // Draw game border first
    if(init) draw_border();
    
    // Iterate over all visible map tiles
    for (int i = -5; i <= 5; i++) // Iterate over columns of tiles
    {
        for (int j = -4; j <= 4; j++) // Iterate over one column of tiles
        {
            // Here, we have a given (i,j)
            
            // Compute the current map (x,y) of this tile
            int x = i + player.x;
            int y = j + player.y;
            
            // Compute the previous map (px, py) of this tile
            int px = i + player.px;
            int py = j + player.py;
                        
            // Compute u,v coordinates for drawing
            int u = (i+5)*11 + 3;
            int v = (j+4)*11 + 15;
            
            // Figure out what to draw
            DrawFunc draw = NULL;
            if (init && i == 0 && j == 0) // Only draw the player on init
            {
                draw_player(u, v, player.has_key);
                continue;
            }
            else if (x >= 0 && y >= 0 && x < map_width() && y < map_height()) // Current (i,j) in the map
            {
                MapItem* curr_item = get_here(x, y);
                MapItem* prev_item = get_here(px, py);
                if (init || curr_item != prev_item) // Only draw if they're different
                {
                    if (curr_item) // There's something here! Draw it
                    {
                        draw = curr_item->draw;
                    }
                    else // There used to be something, but now there isn't
                    {

                        draw = draw_nothing;

                    }
                }
            }
            else if (init) // If doing a full draw, but we're out of bounds, draw the walls.
            {
                draw = draw_wall;
            }

            // Actually draw the tile
            if (draw) {
                // pc.printf("(%d,%d)", u,v);
                draw(u, v);
                // Draw status bars    

            }
        }
    }
    
        if (!PauseSensors){
            char buffer [50];
            sprintf (buffer, "X:%i, Y: %d     ",player.x,player.y);
            // Draw status bars    
            draw_upper_status(buffer);
        }
    }








void palpatine_walkin(){
    int objX = 6;
    int objY = 1;

    add_palpatine(objX,objY);
    for (int i = 1; i < 5; i++){
        updateItemPosition(objX,objY,objX + 1, objY);
        objX = objX + 1;
        draw_game(true);
    }

}





/**
 * Initialize the main world map. Add walls around the edges, interior chambers,
 * and holograms in the background so you can see motion.
 */
void init_main_map()
{

    player.x = 10;
    player.y = 4;
    player.health = 100;
    draw_lower_status(player.health,0);

    // "Random" hologram
    Map* map = set_active_map(0);
    // for(int i = map_width() + 3; i < map_area(); i += 39)
    // {
    //     add_holocron(i % map_width(), i / map_width());
    // }



    for(int j = 1; j < 5; j++)
    {
        for(int i = 1; i < 10; i++)
        {
            add_trooper(j+4,i+2);

        }
    }

    for(int j = 1; j < 5; j++)
    {
        for(int i = 1; i < 10; i++)
        {
            add_trooper(j+11,i+2);

        }
    }
        
    pc.printf("Adding walls!\r\n");
    add_wall(0,              0,              HORIZONTAL, map_width());
    add_wall(0,              map_height()-1, HORIZONTAL, map_width());
    add_wall(0,              0,              VERTICAL,   map_height());
    add_wall(map_width()-1,  0,              VERTICAL,   map_height());
    pc.printf("Walls done!\r\n");
    add_door(9,13, JEDITEMPLEDOOR);
    add_door(10,13, JEDITEMPLEDOOR);
    add_door(11,13, JEDITEMPLEDOOR);


    add_door(18,7, SITHTEMPLEDOOR);
    add_door(18,8, SITHTEMPLEDOOR);
    add_door(18,9, SITHTEMPLEDOOR);

    print_map();
}


/**
 * Initialize the second world map. Add walls around the edges, interior chambers,
 * and holograms in the background so you can see motion.
 */
void init_second_map()
{

    draw_lower_status(player.health,holocrons_collected);
    // "Random" hologram
    Map* map = set_active_map(0);
    // for(int i = map_width() + 3; i < map_area(); i += 39)
    // {
    //     add_holocron(i % map_width(), i / map_width());
    // }

    add_trooper(12,12);
    add_trooper(13,12);
    add_trooper(14,12);
    add_trooper(15,12);

    add_trooper(8,12);
    add_trooper(7,12);
    add_trooper(6,12);
    add_trooper(5,12);

    add_jedi(10,5);
    add_jedi(8,5);
    add_jedi(12,5);
    add_jedi(10,7);

    for (int i = 1; i< map_height()-4; i++){
        add_library(i+4,2);

    }

    // pc.printf("Adding walls!\r\n");
    add_wall(0,              0,              HORIZONTAL, map_width(), draw_library);
    add_wall(0,              map_height()-1, HORIZONTAL, map_width(), draw_library);
    add_wall(0,              0,              VERTICAL,   map_height(), draw_library);
    add_wall(map_width()-1,  0,              VERTICAL,   map_height(), draw_library);
    pc.printf("Walls done!\r\n");

    print_map();
}



/**
 * Initialize the third world map. Add walls around the edges, interior chambers,
 * and holograms in the background so you can see motion.
 */
void init_third_map()
{

    draw_lower_status(player.health,holocrons_collected);
    Map* map = set_active_map(0);
    player.x = 14;
    player.y = 8;
    // for(int i = map_width() + 3; i < map_area(); i += 39)
    // {
    //     add_holocron(i % map_width(), i / map_width());
    // }

    add_sith_elder(10,10);
    add_sith_elder(10,9);
    add_sith_elder(10,8);
    add_sith_elder(10,7);

    // pc.printf("Adding walls!\r\n");
    add_wall(0,              0,              HORIZONTAL, map_width(), draw_library);
    add_wall(0,              map_height()-1, HORIZONTAL, map_width(), draw_library);
    add_wall(0,              0,              VERTICAL,   map_height(), draw_library);
    add_wall(map_width()-1,  0,              VERTICAL,   map_height(), draw_library);
    pc.printf("Walls done!\r\n");

    print_map();
}








