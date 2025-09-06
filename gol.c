#include "syscalls/syscalls.h"
#include "math/math.h"
#include "input_keycodes.h"

// John Conway's Game of Life 
// 1. A cell with fewer than 2 neighbors dies as if by underpopulation
// 2. A cell with more than 3 neighbors dies as if by overpopulation
// 3. An empty cell with 3 neighbors comes to life as if by reproduction
// 4. A cell with 2 or 3 neighbors lives on to the next generation

typedef union { 
    struct {
        uint8_t rsvd: 5;
        uint8_t flip_on: 1;
        uint8_t flip_off: 1;
        uint8_t alive: 1;
    };
    uint8_t cell;
} cell;

cell *grid;
int grid_x, grid_y;
bool paused = false; 

#define SCALE 10
#define BG_COLOR 0x0
#define CELL_COLOR 0xFFb4dd13

void check_neighbors(){
    for (int x = 0; x < grid_x; x++){
        for (int y = 0; y < grid_y; y++){
            int neighbors = 0;
            for (int nx = -1; nx <= 1; nx++){
                for (int ny = -1; ny <= 1; ny++){
                    if ((nx + x) < 0 || (ny + y) < 0 || (nx + x) > grid_x || (ny + y) > grid_y || (ny == 0 && nx == 0)) continue;
                    if (grid[((y + ny) * grid_x) + (x + nx)].alive) neighbors++;
                }
            }
            if (grid[(y * grid_x) + x].alive && (neighbors < 2 || neighbors > 3)){
                // printf("DEAD %ix%i", x, y);
                grid[(y * grid_x) + x].flip_off = 1;
            }
            else if (!grid[(y * grid_x) + x].alive && neighbors == 3){
                // printf("ALIVE %ix%i", x, y);
                grid[(y * grid_x) + x].flip_on = 1;
            }
        }
    }
    
}

#define CELL_AT(x,y) grid[(y * grid_x) + x]

int main(int argc, char* argv[]){
    draw_ctx ctx = {};
    request_draw_ctx(&ctx);
    grid_x = (int)min(100, max(10, ctx.width/SCALE));
    grid_y = (int)min(100, max(10, ctx.height/SCALE));
    grid = (cell*)malloc(grid_x*grid_y*sizeof(cell));
    rng_t rng = {};
    rng_seed(&rng, get_time());
    for (uint32_t x = 0; x < grid_x; x++)
        for (uint32_t y = 0; y < grid_y; y++){
            grid[(y * grid_x) + x].flip_on = rng_next8(&rng) < 30;
            grid[(y * grid_x) + x].alive = grid[(y * grid_x) + x].flip_on; 
        }
    
    fb_clear(&ctx, BG_COLOR);
    commit_draw_ctx(&ctx);
    int generation = 0;
    while (true){
        bool step = false;
        keypress kp = {};
        if (read_key(&kp)){
            if (kp.keys[0] == KEY_SPACE) paused = !paused;
            if (kp.keys[0] == KEY_ENTER || kp.keys[0] == KEY_KPENTER) step = true;
        }
        if (!paused || step){
            ctx.full_redraw = true;
            for (int x = 0; x < grid_x; x++){
                for (int y = 0; y < grid_y; y++){
                    if (grid[(y * grid_x) + x].flip_on){
                        fb_fill_rect(&ctx, x * SCALE, y * SCALE, SCALE, SCALE, CELL_COLOR);
                        grid[(y * grid_x) + x].flip_on = 0;
                        grid[(y * grid_x) + x].alive = 1;
                    }
                    if (grid[(y * grid_x) + x].flip_off){
                        fb_fill_rect(&ctx, x * SCALE, y * SCALE, SCALE, SCALE, BG_COLOR);
                        grid[(y * grid_x) + x].flip_off = 0;
                        grid[(y * grid_x) + x].alive = 0;
                    }
                }
            }
            commit_draw_ctx(&ctx);
            check_neighbors();
            // printf("Generation %i done",generation++);
        }
    }
    return 1;
}