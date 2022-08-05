#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <SFML/Audio.h>
#include <SFML/Graphics.h>

typedef bool map[64];
typedef uint64_t board;
typedef char constraint[8];
struct problem {
board monsters;
board treasures;
constraint rows;
constraint columns;
};
typedef struct problem problem;
int enum_popcount_byte[256];

board map_to_board(map m){
	uint64_t pow = 1;
	board b = 0x8000000000000000;
	for(int i = 0; i < 64; i ++)
	{
		b = b + pow * m[i];
		pow = pow * 2;
	}
	return b;
}

void addbin(int n, int pos, bool m[64]){
	int b = pos;
	while(n != 0){
		if(n%2){
			m[b] = 1;
		}
		b = b +1;
		n = n / 2;
	}
}	
		
void board_to_map(uint64_t b, map m){
	int pos = 0;
	int digit;
	while(b!= 0){
		digit = b % 16;
		b = b/16;
		addbin(digit, pos, m);
		pos = pos + 4;
	}
}

int popcount(uint64_t x){
uint64_t m1 = 0x55; // 0b01010101
uint64_t m2 = 0x33; // 0b00110011
uint64_t m3 = 0x0f; // 0b00001111
x = (x&m1) + ((x&(m1<<1))>>1);
x = (x&m2) + ((x&(m2<<2))>>2);
x = (x&m3) + ((x&(m3<<4))>>4);
return x;	
}

void draw_board(problem* p, board b, sfSprite* wall, sfSprite* monster, sfSprite* treasure, sfSprite* empty, sfRenderWindow* window){
	sfVector2f pos;
	pos.x = 0;
	pos.y = 0;
    for(int i = 0; i < 64; i++)
    {
		
        if (((p->monsters >> i) & 1) != 0)
			sfRenderWindow_drawSprite(window, monster, NULL);
        else if (((p->treasures >> i) & 1) != 0)
			sfRenderWindow_drawSprite(window, treasure, NULL);
        else if (((b >> i) & 1) != 0)
            sfRenderWindow_drawSprite(window, monster, NULL);
		else
			sfRenderWindow_drawSprite(window, empty, NULL);
		if(i%8 == 0) {
			pos.y = pos.y + 16*6;
			pos.x = 0;
		}
		else pos.x = pos.x + 16*6;
	}
}
	
	

int main(){
    //16*16 tiles scaled by 6
	sfVideoMode mode = {768, 768, 220};
	sfRenderWindow* window;
	sfTexture* texture_wall;
	sfTexture* texture_treasure;
	sfTexture* texture_monster;
	sfTexture* texture_empty;
	sfSprite* wall;
	sfSprite* empty;
	sfSprite* treasure;
	sfSprite* monster;
	sfVector2f scale;
	scale.x = 6;
	scale.y = 6;
	//loading the textures
		texture_wall = sfTexture_createFromFile("./ressources/wall.png", NULL);
		texture_treasure = sfTexture_createFromFile("./ressources/treasure.png", NULL);	
	texture_monster = sfTexture_createFromFile("./ressources/monster.png", NULL);
	texture_empty = sfTexture_createFromFile("./ressources/empty.png", NULL);
	//creating the sprites
	sfSprite_


	//destroying everything
	return 0;
}
