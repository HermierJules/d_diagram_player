#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef uint64_t board;
typedef bool map[64];
typedef char constraint[8];

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
		current = popcount(mask_column(i, b));
		if(current > p->columns[i])
			return true;
	}
	return false;
}

bool test_columns_underload(problem *p, board b)
{
    int current;
	for(int i = 0; i < 8; i++){
		current=popcount(mask_line(i,b));
		if(current < p->rows[i])
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




void solve(problem *p, int row, board b)
{
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
        //if (test_columns_underload(p, b))
          //  return;

        // Full board with monsters
        board fb = b | p->monsters;

        //if (test_treasure_rooms(p, &fb))
          //  return;
        // Every treasure room is now filled in fb

        if (test_empty_squares(p, fb))
            return;

        printf("Solution %lx\n", b); 
        print_board(p, b);

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

int main(void)
{
    for(int i = 0; i < 256; i++){
        enum_popcount_byte[i] = popcount(i);
    
    }
    problem *problems[] = {
        &p1, &p2, &p3, 
        &p4, &p5, &p6, 
        &p7, &p8, &p9
    };

    for (int i = 0; i < 9; i++)
    {
        printf("Solving problem %d\n", i+1);
        solve(problems[i], 0, 0);
    }
}
