#include "speech.h"

#include "globals.h"
#include "hardware.h"
#include "graphics.h"
#define MAX_SPEECH_LENGTH 17




/**
 * Draw the speech bubble background.
 */
static void draw_speech_bubble();

/**
 * Erase the speech bubble.
 */
static void erase_speech_bubble();

/**
 * Draw a single line of the speech bubble.
 * @param line The text to display
 * @param which If TOP, the first line; if BOTTOM, the second line.
 */
#define TOP    0
#define BOTTOM 1
static void draw_speech_line(const char* line, int which);

/**
 * Delay until it is time to scroll.
 */
static void speech_bubble_wait();

void draw_speech_bubble()
{
    uLCD.filled_rectangle(0,100,127,127, WHITE);
}


void erase_speech_bubble()
{
    uLCD.filled_rectangle(0,100,127,127, BLACK);
    draw_border();

}

void draw_speech_line(const char* line, int which)
{
    uLCD.text_mode(TRANSPARENT);
    uLCD.text_string((char*)line, 1, 13 + which, FONT_7X8, BLACK);
    uLCD.text_mode(OPAQUE);

   
}

void speech_bubble_wait()
{
    //Wait for button to be pushed again
    while(button1.read()){}
    while(!button1.read()){}
    wait(0.3);


}

void speech(const char* line1, const char* line2)
{
    draw_speech_bubble();
    draw_speech_line(line1, TOP);
    draw_speech_line(line2, BOTTOM);
    speech_bubble_wait();
    erase_speech_bubble();
}

void clearArrays(char * a, char *b){
    for (int j = 0; j < MAX_SPEECH_LENGTH; j++){
        a[j] = '\0';
        b[j] = '\0';

    }
}


void long_speech(char* lines)
{
    draw_speech_bubble();


    char arr1[MAX_SPEECH_LENGTH] = {};
    char arr2[MAX_SPEECH_LENGTH] = {};
    int count = 0;
    int isFirst = 1;
    int offset = 0;
    while(*lines != '\0'){

        if (((count + 1) % MAX_SPEECH_LENGTH) == 0){
            if (isFirst){
                draw_speech_line(arr1, TOP);
            }else{
                draw_speech_line(arr2, BOTTOM);
                speech_bubble_wait();
                erase_speech_bubble();
                draw_speech_bubble();
                clearArrays(arr1,arr2);

            }
            offset = count;
            isFirst = (isFirst + 1) % 2;

        }
        if (isFirst){
            arr1[count - offset] = *lines;
        }else{
            arr2[count - offset] = *lines;

        }

        lines++;
        count++;

    }
    
    if (isFirst){
        draw_speech_line(arr1, TOP);
    }else{
        draw_speech_line(arr2, BOTTOM);
    }

    speech_bubble_wait();
    //TODO Get array free working for speech
    // free(arr1);
    // free(arr2);
    erase_speech_bubble();

}


