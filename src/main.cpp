// Project includes
#include "globals.h"
#include "hardware.h"
#include "map.h"
#include "graphics.h"
#include "drawer.h"


/**
 * Program entry point! This is where it all begins.
 * This function orchestrates all the parts of the game. Most of your
 * implementation should be elsewhere - this holds the game loop, and should
 * read like a road map for the rest of the code.
 */
int main()
{

    // First things first: initialize hardware
    ASSERT_P(hardware_init() == ERROR_NONE, "Hardware init failed!");
    
    // Initialize the maps
    maps_init();
    init_third_map();
    // Initialize game state
    set_active_map(0);


    // Initial drawing
    draw_game(true);
    // palpatine_walkin();

    double ax = 0, ay = 0, az = 0;
    int b1 = 0, b2 = 0, b3 = 0;
    // Main game loop
    while(1)
    {
        // Timer to measure game update speed
        Timer t; t.start();
        
            if (button1.is_connected() && button2.is_connected() && button3.is_connected()){
                b1 = (button1.read() + 1) % 2;
                b2 = (button2.read() + 1) % 2;
                b3 = (button3.read() + 1) % 2;
            }


            if(!acc.isXYZReady()) 
            {
                wait(0.01);
            }
            else
            {
                acc.readXYZGravity(&ax,&ay,&az); //notice this is passed by reference use pointers
                GameInputs gameInputs = {.b1 = b1, .b2 = b2, .b3 = b3, .ax = ax, .ay = ay, .az = az  };
                // pc.printf("(%f,%f,%f)", ax,ay,az);

                int action = get_action(gameInputs);

                update_game(action);
                draw_game(false);
            } 
      


        // Actuall do the game update:
        // 1. Read inputs        
        // 2. Determine action (get_action)        
        // 3. Update game (update_game)
        // 3b. Check for game over
        // 4. Draw frame (draw_game)
        // draw_game(true);


        // 5. Frame delay
        t.stop();
        int dt = t.read_ms();
        if (dt < 100) wait_ms(100 - dt);
    }
}
