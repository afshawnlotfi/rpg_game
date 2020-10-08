#include "map.h"
#include "globals.h"
#include "graphics.h"

#define HashTableSize 10



/**
 * The Map structure. This holds a HashTable for all the MapItems, along with
 * values for the width and height of the Map.
 */
struct Map {
    HashTable* items;
    int w, h;
};

/**
 * Storage area for the maps.
 * This is a global variable, but can only be access from this file because it
 * is static.
 */
static Map map;
static int active_map;
// 0 normal, 1 omnipotent

/**
 * The first step in HashTable access for the map is turning the two-dimensional
 * key information (x, y) into a one-dimensional unsigned integer.
 * This function should uniquely map (x,y) onto the space of unsigned integers.
 */
static unsigned XY_KEY(int X, int Y) {
    //Pairing function creates unique key 
    //pi(x,y) = 0.5*(x+y)(x+y+1) + y
    return (0.5*(X+Y)*(X+Y+1) + Y);
}

/**
 * This is the hash function actually passed into createHashTable. It takes an
 * unsigned key (the output of XY_KEY) and turns it into a hash value (some
 * small non-negative integer).
 */
unsigned map_hash(unsigned key)
{   
    return key % HashTableSize;
}

void maps_init()
{

    map.items =  createHashTable(map_hash,HashTableSize);
    map.w = 20;
    map.h = 15;
}

Map* get_active_map()
{
    // There's only one map
    return &map;
}

Map* set_active_map(int m)
{
    active_map = m;
    return &map;
}

void print_map()
{
    // As you add more types, you'll need to add more items to this array.
    char lookup[] = {'W', 'P'};
    for(int y = 0; y < map_height(); y++)
    {
        for (int x = 0; x < map_width(); x++)
        {
            MapItem* item = get_here(x,y);
            if (item) pc.printf("%c", lookup[item->type]);
            else pc.printf(" ");
        }
        pc.printf("\r\n");
    }
}

int map_width()
{
    if (map.w != NULL){
        return map.w;   
    }else{
        return 0;
    }
}

int map_height()
{
    if (map.h != NULL){
        return map.h;   
    }else{
        return 0;
    }
}

int map_area()
{
    if (map.w != NULL && map.h != NULL){
        return map.w * map.h;   
    }else{
        return 0;
    }
}

MapItem* get_north(int x, int y)
{
    return get_here(x,y+1);
}

MapItem* get_south(int x, int y)
{
    return get_here(x,y-1);
}

MapItem* get_east(int x, int y)
{
    return get_here(x+1,y);

}

MapItem* get_west(int x, int y)
{
    return get_here(x-1,y);
}


MapItem* getAllDirections(int x, int y, int * xOffset, int * yOffset){
    
    MapItem* item = get_north(x,y);
    *xOffset = 0;
    *yOffset = 1;
    if (item) {return item;}
    item = get_south(x,y);
    *xOffset = 0;
    *yOffset = -1;
    if (item) {return item;}
    item = get_east(x,y);
    *xOffset = 1;
    *yOffset = 0;
    if (item) {return item;}
    item = get_west(x,y);
    *xOffset = -1;
    *yOffset = 0;
    return item;
}

MapItem* get_here(int x, int y)
{
    unsigned key = XY_KEY(x,y);
    MapItem* item = (MapItem*)(getValue(map.items,key));
    return item;
}





MapItem* map_erase(int x, int y)
{
    unsigned key = XY_KEY(x,y);
    MapItem* item = (MapItem*)(getValue(map.items,key));
    deleteItem(get_active_map()->items,key);
    return item;

}

void updateItemPosition(int currX, int currY, int newX, int newY){
    unsigned currKey = XY_KEY(currX,currY);
    void* item = removeItem(get_active_map()->items,currKey);
    unsigned newKey =  XY_KEY(newX,newY);
    insertItem(get_active_map()->items,newKey, item);
}

void map_destroy()
{
    destroyHashTable(get_active_map()->items);
    maps_init();
}

void add_wall(int x, int y, int dir, int len)
{
    add_wall(x, y, dir,len, draw_wall);
}

void add_wall(int x, int y, int dir, int len, DrawFunc func)
{

    for(int i = 0; i < len; i++)
    {
        MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
        w1->type = WALL;
        w1->draw = func;
        w1->walkable = false;
        unsigned key = (dir == HORIZONTAL) ? XY_KEY(x+i, y) : XY_KEY(x, y+i);
        deleteItem(get_active_map()->items, key);
        insertItem(get_active_map()->items, key, w1);
    }

}


void add_plant(int x, int y)
{

    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = PLANT;
    w1->draw = draw_plant;
    w1->walkable = true;
    w1->interact = plant_interact;

    int key = XY_KEY(x, y);
    deleteItem(get_active_map()->items, key);
    void* val = insertItem(get_active_map()->items, key, w1);
}



void add_holocron(int x, int y)
{

    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = HOLOCRON;
    w1->draw = draw_holocron;
    w1->interact = holocron_interact;
    w1->walkable = true;

    int key = XY_KEY(x, y);
    deleteItem(get_active_map()->items, key);
    void* val = insertItem(get_active_map()->items, key, w1);
}


void add_palpatine(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = PALPATINE;
    w1->draw = draw_palpatine;
    w1->walkable = false;
    w1->interact = palpatine_interact;

    // add_item_property(w1,HOVER_TEXT,(void *)"Holocron collected");
    int key = XY_KEY(x, y);
    deleteItem(get_active_map()->items, key);
    void* val = insertItem(get_active_map()->items, key, w1);
}

void add_trooper(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = TROOPER;
    w1->draw = draw_trooper;
    w1->walkable = false;
    w1->interact = trooper_interact;

    int key = XY_KEY(x, y);
    deleteItem(get_active_map()->items, key);
    void* val = insertItem(get_active_map()->items, key, w1);
}


void add_door(int x, int y, int id)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = DOOR;
    w1->draw = draw_door;
    w1->walkable = false;
    w1->interact = door_interact;
    w1->data = (void *)id;
    int key = XY_KEY(x, y);
    deleteItem(get_active_map()->items, key);
    void* val = insertItem(get_active_map()->items, key, w1);
}


void add_jedi(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = JEDI;
    w1->draw = draw_jedi;
    w1->walkable = false;
    w1->interact = jedi_interact;
    w1->data = (void *)100;

    int key = XY_KEY(x, y);
    deleteItem(get_active_map()->items, key);
    void* val = insertItem(get_active_map()->items, key, w1);
}

void add_library(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = LIBRARY;
    w1->draw = draw_library;
    w1->walkable = false;
    w1->interact = library_interact;
    w1->data = (void *)100;

    int key = XY_KEY(x, y);
    deleteItem(get_active_map()->items, key);
    void* val = insertItem(get_active_map()->items, key, w1);
}

void add_key(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = KEY;
    w1->draw = draw_key;
    w1->walkable = true;
    w1->interact = key_interact;
    int key = XY_KEY(x, y);
    deleteItem(get_active_map()->items, key);
    void* val = insertItem(get_active_map()->items, key, w1);
}


void add_sith_elder(int x, int y)
{
    MapItem* w1 = (MapItem*) malloc(sizeof(MapItem));
    w1->type = SITHELDER;
    w1->draw = draw_sith_elder;
    w1->walkable = false;
    w1->interact = sith_elder_interact;
    int key = XY_KEY(x, y);
    deleteItem(get_active_map()->items, key);
    void* val = insertItem(get_active_map()->items, key, w1);
}
