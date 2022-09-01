#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <SFML/Audio.h>
#include <SFML/Graphics.h>

typedef uint64_t board;
typedef bool map[64];
typedef char constraint[8];
	sfVideoMode mode = {864, 864, 220};
	sfRenderWindow* window;
	sfTexture* texture_wall;
	sfTexture* texture_treasure;
	sfTexture* texture_monster;
	sfTexture* texture_empty;
	sfSprite* wall;
	sfSprite* empty_sprite;
	sfFont* font;
	sfSprite* treasure;
	sfSprite* monster_sprite;
	sfVector2f scale;
	sfEvent event;
	sfTime wait;
struct problem {
    board monsters;
    board treasures;
    constraint rows;
    constraint columns;
};
typedef struct problem problem;

problem p1 = {
	.monsters = 0x8400010000000020,
	.treasures = 0x20000,
	.rows = { 7,3,4,1,7,1,6,3 },
	.columns = { 4,4,2,6,2,3,4,7 }
};

problem p2 = {
	.monsters = 0xa00008000000000,
	.treasures = 0x2800,
	.rows = { 5,2,2,3,6,0,6,1 },
	.columns = { 5,3,4,3,5,1,3,1 }
};

problem p3 = {
	.monsters = 0x8000008001000081,
	.treasures = 0x4000000000000,
	.rows = { 6,0,4,1,5,3,3,4 },
	.columns = { 4,4,1,3,6,3,4,1 }
};

problem p4 = {
	.monsters = 0x8000800000000100,
	.treasures = 0x2000,
	.rows = { 2,3,4,3,7,0,4,0 },
	.columns = { 1,4,1,5,1,3,2,6 }
};

problem p5 = {
	.monsters = 0x20001,
	.treasures = 0x80000,
	.rows = { 0,7,3,3,3,5,2,5 },
	.columns = { 7,2,6,1,3,2,6,1 }
};

problem p6 = {
	.monsters = 0x802080208020,
	.treasures = 0x40000,
	.rows = { 6,3,3,3,5,3,4,3 },
	.columns = { 5,3,2,4,6,4,1,5 }
};

problem p7 = {
	.monsters = 0x80010008,
	.treasures = 0x80000000000000,
	.rows = { 1,4,2,6,2,3,3,1 },
	.columns = { 1,5,2,2,6,1,4,1 }
};

problem p8 = {
	.monsters = 0x4200801000800042,
	.treasures = 0x0,
	.rows = { 3,6,0,5,4,0,6,3 },
	.columns = { 4,2,4,2,3,4,2,6 }
};

problem p9 = {
	.monsters = 0x40080,
	.treasures = 0x200000000000000,
	.rows = { 4,1,4,2,6,2,3,2 },
	.columns = { 5,2,1,2,5,4,2,3 }
};

board map_to_board(map m)
{
    uint64_t pow = 1;
	board b = 0;
	for(int i = 0; i < 64; i ++)
	{
		b = b + pow * m[i];
		pow = (pow << 1);
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
	
void board_to_map(board b, map m)
{
    int pos = 0;
	int digit;
	while(b!= 0){
		digit = b % 16;
		b = b/16;
		addbin(digit, pos, m);
		pos = pos + 4;
	}
}

int popcount_slow(uint64_t x)
{
    int n = 0;
	while(x != 0){
		if(x%2) n = n + 1;
		x/2;
	}
	return n;
}

int popcount(uint64_t x)
{
    uint64_t m1 = 0x55; // 0b01010101
uint64_t m2 = 0x33; // 0b00110011
uint64_t m3 = 0x0f; // 0b00001111
x = (x&m1) + ((x&(m1<<1))>>1);
x = (x&m2) + ((x&(m2<<2))>>2);
x = (x&m3) + ((x&(m3<<4))>>4);
return x;	

}

void print_board(problem *p, board b)
{
    for (int i = 0; i < 64; i++)
    {
		
		if(i%8 == 0 && i!=0) 
			putchar('\n');
        if (((p->monsters >> i) & 1) != 0)
			putchar('M');
        else if (((p->treasures >> i) & 1) != 0)
			putchar('T');
        else if (((b >> i) & 1) != 0)
            putchar('#');
		else
			putchar('.');
	}
}


int enum_popcount_byte[256];

uint64_t bit(int pos)
{
    return (uint64_t) 1 << pos;
}
int is_empty(int n, board b){ //permet d'accéder à une des 64 cases
	 if ((b >> n) & 1) return 1;
	else return 0;
}
int is_not_empty(int n, board b){ //permet d'accéder à une des 64 cases
	 if ((b >> n) == 0) return 0;
	else return 1;
}
uint64_t mask_column(int i, board b)
{
    uint64_t mask = 0;
	uint64_t pow = 1 << 8;
	for(int j = 0; j < 8; j++){
		mask = mask + pow * is_empty(i + j*8, b);
		pow = pow/2;
	}
	return mask;
}
uint64_t mask_line(int i, board b){
	uint64_t mask = 0;
	uint64_t pow = 1 << 8;
	for(int j = 0; j < 8; j++){
		mask = mask + pow * is_empty(j + i*8, b);
		pow = pow/2;
	}
	return mask;
}
bool empty(problem *p, board b, int pos){
	if(((b >> pos) & 1) | ((p->monsters >> pos) & 1) | ((p->treasures >> pos) & 1)) return 0;
	else return 1;
}
bool test_columns_overload(problem *p, board b)
{
    int current;
	for(int i = 0; i < 8; i++){
		current = __builtin_popcount(mask_column(i, b));
		if(current > p->columns[i])
			return true;
	}
	return false;
}

bool test_columns_underload(problem *p, board b)
{
    int current;
	for(int i = 0; i < 8; i++){
		current=__builtin_popcount(mask_column(i,b));
		if(current < p->columns[i])
			return true;
	}
	return false;
}



//bool treasure(problem *p, int pos)


bool monster(problem *p, int pos)
{
    if((p->monsters >> pos) & 1) return 1;
	else return 0;
}

//uint64_t neighbours(int pos)

int count_non_wall(problem *p, board b, int pos)
{
    int n = 0;
	if(pos > 8) //above 
		if (!((b >> (pos-8)) & 1)) n=n+1;
	if(pos < 56) //under
		if(!((b >> (pos+8)) & 1)) n=n+1;
	if(pos%8 != 0) //to the right
		if(!((b >> (pos-1)) & 1)) n=n+1;
	if((pos+1)%8 != 0) //to the left
		if(!((b >> (pos+1)) & 1)) n=n+1;
	return n;
}

bool test_no_deadend_upto_row(problem *p, board b, int row)
{
    for(int i = 0; i < (row*8); i++){
		if(empty(p, b, i) && count_non_wall(p, b, i) < 2) return true;
	}
	return false;
}

bool test_trapped_monster(problem *p, board b)
{
    //renvoie false si rien à signaler
	for(int i = 0; i < 64; i++){
		if(is_empty(i,p->monsters) && count_non_wall(p,b,i) == 0 ) return true;
	}
	return false;
}

bool test_deadends(problem *p, board b)
{
    //renvoie false si rien à signaler
	for(int i = 0; i < 64; i++){
		if(is_empty(i,p->monsters))
			if(count_non_wall(p, b, i) != 1)
			return true;
		if(empty(p,b,i) && count_non_wall(p,b,i) < 2) 
			return true;
	}
	return false;
}

bool test_empty_squares(problem *p, board b)
{
    //renvoie false si rien à signaler
	for(int i = 0; i < 56; i++){
		if((i+1) % 8 != 0 && i!=0){
		if(empty(p,b,i) && empty(p,b,i+1) 
			&& empty(p,b,i+8) && empty(p,b,i+8+1))
			return true;
		}
	}
	return false;
}

bool test_treasure_rooms(problem *p, board *b){
    return true;
}





void draw_board(problem* p, board b, sfSprite* wall, sfSprite* monster_sprite, sfSprite* treasure, sfSprite* empty_sprite, sfFont* font,sfRenderWindow* window){
	char str[96];
	sfVector2f pos;
	sfVector2f postext;
	postext.x = 0;
	postext.y = 0;
	sfText* text;
	text = sfText_create();
	sfText_setColor(text, sfWhite);
	sfText_setFont(text,font);
	sfText_setCharacterSize(text,96);
	sprintf(str," %d%d%d%d%d%d%d%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d",p->columns[1], p->columns[2], p->columns[3], p->columns[4], p->columns[5], p->columns[6], p->columns[7],p->rows[0], p->rows[1], p->rows[2], p->rows[3], p->rows[4], p->rows[5], p->rows[6], p->rows[7], p->columns[0]);
	sfText_setPosition(text,postext);
	sfText_setString(text,str);
	sfRenderWindow_drawText(window,text,NULL);
	pos.x = 0;
	pos.y = 96;
    for(int i = 0; i < 64; i++)
    {
		if(i%8 == 0 && i!= 0) {
			pos.y = pos.y + 16*6;
			pos.x = 96;
		}
		else pos.x = pos.x + 16*6;
			sfSprite_setPosition(empty_sprite,pos);
			sfRenderWindow_drawSprite(window, empty_sprite, NULL);
		if (((b >> i) & 1) != 0){
			sfSprite_setPosition(wall,pos);
            sfRenderWindow_drawSprite(window, wall, NULL);
		}
        else if (((p->monsters >> i) & 1) != 0){
			sfSprite_setPosition(monster_sprite, pos);
			sfRenderWindow_drawSprite(window, monster_sprite, NULL);
        }
		else if (((p->treasures >> i) & 1) != 0){
			sfSprite_setPosition(treasure, pos);
			sfRenderWindow_drawSprite(window, treasure, NULL);
        }
		
	}
	sfText_destroy(text);
}
	
void solve(problem *p, int row, board b)
{
	sfRenderWindow_clear(window, sfBlack);
	draw_board(p, b, wall, monster_sprite, treasure, empty_sprite,font, window);
   	sfRenderWindow_display(window);
	sfSleep(wait);
   	 // Unsolvable from this board
    if (test_columns_overload(p, b))
        return;
    //printf("testcol\n");
    if (test_no_deadend_upto_row(p, b, row))
        return;
    //printf("nodead\n");
    if (test_trapped_monster(p, b))
        return;
    //printf("trapmon\n");
        // Board is filled
    if(row == 8)
    {
        printf("end");
        // Validate columns count
        if (test_columns_underload(p, b))
            return;

        // Full board with monsters
        board fb = b | p->monsters;

        //if (test_treasure_rooms(p, &fb))
          //  return;
        // Every treasure room is now filled in fb

        if (test_empty_squares(p, fb))
            return;

        printf("Solution %lx\n", b); 
        print_board(p, b);
		sleep(50);

        return;
    }

    for(int i = 0; i < 256; i++)
    {
        if (enum_popcount_byte[i] == p->rows[row])
        {
            uint64_t move = (uint64_t)i << (8*row);

            // ensure there's no overlap with walls/monsters/treasures
            if ((move & (p->monsters | p->treasures)) != 0)
                continue;

            solve(p, row+1, b + move);
        }
    }
}

int main(){
	wait = sfSeconds(.0);
for(int i = 0; i < 256; i++){
        enum_popcount_byte[i] = __builtin_popcount(i);
    
    }
    problem *problems[] = {
        &p1, &p2, &p3, 
        &p4, &p5, &p6, 
        &p7, &p8, &p9
    };

		scale.x = 6;
	scale.y = 6;
	//loading the textures
		texture_wall = sfTexture_createFromFile("./ressources/wall.png", NULL);
		texture_treasure = sfTexture_createFromFile("./ressources/treasure.png", NULL);	
	texture_monster = sfTexture_createFromFile("./ressources/monster.png", NULL);
	texture_empty = sfTexture_createFromFile("./ressources/empty.png", NULL);
	//creating the sprites
	wall = sfSprite_create();
	treasure =sfSprite_create();
	empty_sprite = sfSprite_create();
	monster_sprite =sfSprite_create();
	//setting the texture
	sfSprite_setTexture(wall,texture_wall,sfTrue);
	sfSprite_setTexture(treasure, texture_treasure, sfTrue);
	sfSprite_setTexture(empty_sprite, texture_empty, sfTrue);
	sfSprite_setTexture(monster_sprite,texture_monster, sfTrue);
	//setting the scale
	sfSprite_setScale(monster_sprite,scale);
	sfSprite_setScale(empty_sprite,scale);
	sfSprite_setScale(treasure,scale);
	sfSprite_setScale(wall,scale);
	//destroying everything
	//creating the font
	font = sfFont_createFromFile("./ressources/pixel.ttf");
	//creating the window
	window = sfRenderWindow_create(mode, "dungeons and diagrams", sfResize | sfClose, NULL);

    //16*16 tiles scaled by 6, 9*9 grid
	while(sfRenderWindow_isOpen(window)){
		while(sfRenderWindow_pollEvent(window, &event)){
			/*close window: exit*/
			if(event.type == sfEvtClosed)
				sfRenderWindow_close(window);
		}
		sfRenderWindow_clear(window, sfBlack);
		solve(problems[7],0,0);
	sfRenderWindow_display(window);
	}
	return 0;
}
