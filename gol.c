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
        uint32_t color: 24;
        uint32_t rsvd: 5;
        uint32_t flip_on: 1;
        uint32_t flip_off: 1;
        uint32_t alive: 1;
    };
    uint32_t cell;
} cell;

cell *grid;
int grid_x, grid_y;
bool paused = false; 
rng_t rng = {};

#define SCALE 10
#define BG_COLOR 0x0

#define CELL_AT(x,y) grid[(y * grid_x) + x]

void check_neighbors(){
    for (int x = 0; x < grid_x; x++){
        for (int y = 0; y < grid_y; y++){
            int neighbors = 0;
            uint16_t r = rng_next8(&rng)/3;
            uint16_t g = rng_next8(&rng)/3;
            uint16_t b = rng_next8(&rng)/3;
            for (int nx = -1; nx <= 1; nx++){
                for (int ny = -1; ny <= 1; ny++){
                    if ((nx + x) < 0 || (ny + y) < 0 || (nx + x) > grid_x || (ny + y) > grid_y || (ny == 0 && nx == 0)) continue;
                    if (grid[((y + ny) * grid_x) + (x + nx)].alive){
                        neighbors++;
                        r += (grid[((y + ny) * grid_x) + (x + nx)].color >> 16) & 0xFF;
                        g += (grid[((y + ny) * grid_x) + (x + nx)].color >> 8) & 0xFF;
                        b += (grid[((y + ny) * grid_x) + (x + nx)].color >> 0) & 0xFF;
                    }
                }
            }
            r /= neighbors;
            g /= neighbors;
            b /= neighbors;

            r &= 0xFF;
            g &= 0xFF;
            b &= 0xFF;

            if (CELL_AT(x, y).alive && (neighbors < 2 || neighbors > 3)){
                CELL_AT(x, y).flip_off = 1;
            }
            else if (!CELL_AT(x, y).alive && neighbors == 3){
                CELL_AT(x, y).color = (r << 16) | (g << 8) | (b << 0);
                CELL_AT(x, y).flip_on = 1;
            }
        }
    }
}

int main(int argc, char* argv[]){
    draw_ctx ctx = {};
    request_draw_ctx(&ctx);
    grid_x = (int)min(100, max(10, ctx.width/SCALE));
    grid_y = (int)min(100, max(10, ctx.height/SCALE));
    grid = (cell*)malloc(grid_x*grid_y*sizeof(cell));
    rng_seed(&rng, get_time());
    for (uint32_t x = 0; x < grid_x; x++)
        for (uint32_t y = 0; y < grid_y; y++){
            if (rng_next8(&rng) < 30){
                CELL_AT(x, y).flip_on = 1;
                CELL_AT(x, y).color = (x << 16) | (y << 8) | min(rng_next8(&rng), 100)/2;
            }
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
            if (kp.keys[0] == KEY_ESC) halt(0);
        }
        if (!paused || step){
            ctx.full_redraw = true;
            for (int x = 0; x < grid_x; x++){
                for (int y = 0; y < grid_y; y++){
                    cell cell = CELL_AT(x, y);
                    if (cell.flip_on){
                        fb_fill_rect(&ctx, x * SCALE, y * SCALE, SCALE, SCALE, (0xFF << 24) | cell.color);
                        CELL_AT(x, y).flip_on = 0;
                        CELL_AT(x, y).alive = 1;
                    }
                    if (cell.flip_off){
                        fb_fill_rect(&ctx, x * SCALE, y * SCALE, SCALE, SCALE, BG_COLOR);
                        CELL_AT(x, y).flip_off = 0;
                        CELL_AT(x, y).alive = 0;
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