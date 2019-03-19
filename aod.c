#include <stdlib.h>
#include <math.h>
#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>
#include <time.h>

// THE ADVENTURES OF DESMOND THE DUCK - MINI GAME
// Mitchell Fox
// n9968296 - CAB202

// Game Definitons
#define DELAY 10 /* milliseconds */
#define SCREEN_WIDTH (screen_width())
#define SCREEN_HEIGHT (screen_height())
#define DISPLAY_HEIGHT (7)
#define DISPLAY_WIDTH (77)
#define HERO_WIDTH (3)
#define HERO_HEIGHT (4)
#define HERO_START_X (screen_width / 2)
#define HERO_START_Y (12)
#define TREASURE_WIDTH (7)
#define TREASURE_HEIGHT (2)
#define MAX_BLOCK_WIDTH (10)
#define MAX_BLOCK_HEIGHT (2)
#define HORIZONTAL_SPACING (2)
#define NUMBER_OF_ROWS (screen_height() / (MAX_BLOCK_HEIGHT + (HERO_HEIGHT + 2)))
#define NUMBER_OF_COLUMNS (screen_width() / (MAX_BLOCK_WIDTH + 2))
#define MENU_WIDTH (80)

// Game States
bool game_over;
bool update_screen = true;
bool paused = false;
bool on_platform = false;
bool free_unsafe_coordinates = false;
bool sprites_collide( sprite_id x, sprite_id y);
bool begin_game = false;

// Game Variables
timer_id game_timer;
int milliseconds = 0;
int seconds = 0;
int minutes = 0;
int lives = 10;
int score = 000;
int key_code;
double dx = 0;
double dy = 0;
double velocity = 0.0;
double gravity = 0.008;

// Platforms
sprite_id initial_safe_platforms[1000];
sprite_id random_safe_platforms[1000];
sprite_id unsafe_platforms[1000];
int total_platforms = sizeof(initial_safe_platforms) + sizeof(random_safe_platforms);
int total_initial_safe_platforms = 12;
int total_random_safe_platforms = sizeof(random_safe_platforms);

// Initial Platforms in Each Coolumn
int init_random_y;
int init_random_y_values[12] = {1,4,2,3,1,4,3,2,1,4,1,3};

// Random Platforms
int random_x_num;
int random_y_num;
int safe_y_coordinates[20] = {};
int safe_x_coordinates[20] = {};

// Random Unsafe Platforms
int unsafe_random_x_num;
int unsafe_random_y_num;
int unsafe_y_coordinates[10] = {};
int unsafe_x_coordinates[10] = {};

// Game Characters
sprite_id desmond;
sprite_id treasure;
sprite_id safeblock;
sprite_id random_safe_block;
sprite_id unsafe_block;
sprite_id start_block;

// Create Game Characters Images
char *hero_image_left =
            /**/ " @>"
            /**/ "/|L"
            /**/ " | "
            /**/ "/ L";

char *hero_image_opposite =
            /**/ "<@"
            /**/ "/||"
            /**/ " | "
            /**/ "/ |";

char *hero_image_right =
            /**/ " @>"
            /**/ "/|L"
            /**/ " | "
            /**/ "/ L";

char *hero_image_fall =
            /**/ " ! "
            /**/ " @>"
            /**/ "~|~"
            /**/ "Z Z";

char *hero_image_respawn =
            /**/ " @>"
            /**/ "$|$"
            /**/ "$|$"
            /**/ "$|$";

char *safe_block_image =
            /**/ "=========="
            /**/ "==========";

char *unsafe_block_image =
            /**/ "xxxxxxxxxx"
            /**/ "xxxxxxxxxx";

char *treasure_image_1 =
            /**/ "[# $ #]"
            /**/ "[$ # $]";

char *treasure_image_2 =
            /**/ "[$ # $]"
            /**/ "[# $ #]";

char * game_over_image =
/**/	"THANK YOU FOR PLAYING THE ADVENTURES OF DESMOND DUCK @>"
/**/	"                Press any key_code to exit...          ";

char * welcome_image =
/**/".-----.-..-..--.    .--..---..-..-..--..-..-.-----.-..-.---. .--. .--. "  
/**/"`-. .-: :; : .--'  : .; : .  : :: : .--: `: `-. .-: :: : .; : .--: .--'"  
/**/"  : : :    : `;    :    : :: : :: : `; : .` : : : : :: :   .: `; `. `. "   
/**/"  : : : :: : :__   : :: : :; : `' : :__: :. : : : : :; : :.`: :__ _`, :"  
/**/"  :_; :_;:_`.__.'  :_;:_:___.'`.,'`.__.:_;:_; :_; `.__.:_;:_`.__.`.__.'"  
/**/"           .--..---.   .---. .--. .--..-..-..--..-..-.---.             "                        
/**/"          : ,. : .--'  : .  : .--: .--: `' : ,. : `: : .  :            "                        
/**/"          : :: : `;    : :: : `; `. `.: .. : :: : .` : :: :            "                        
/**/"          : :; : :     : :; : :__ _`, : :; : :; : :. : :; :            "                        
/**/"          `.__.:_;     :___.`.__.`.__.:_;:_`.__.:_;:_:___.'            " 
/**/"                                                                       "  
/**/"                       B Y    M I T C H  F O X                         ";  
	

// Keep Game Time
void update_game_timer (void) {
		milliseconds++;

		if (milliseconds >= 100) {
			seconds++;
			milliseconds = 0;
		}
		if (seconds >= 60) {
			minutes++;
			seconds = 0;
		}	
}

// Pause Game
int read_char() {

    int key_code = paused ? wait_char() : get_char();

    if (key_code == 'p')
    {
        paused = !paused;
    }

    return key_code;
}

// Generate Numbers for Platforms
void platform() {

    srand (time(NULL));
    // 10 Random Y Values for Initial 1 Block in each column (Between 1-4)
    for (int i = 0; i <= (NUMBER_OF_COLUMNS - 1); i++) {
        init_random_y = rand() % 4;
        *(init_random_y_values + i) = init_random_y;
    }
    // SAFE - X Create a Random Number
    for (int i = 0; i <= 19; i++) {
        random_x_num = rand() % (NUMBER_OF_COLUMNS - 1);
        *(safe_x_coordinates + i) = random_x_num;
    }
    // SAFE - Y Create a Random Number
    for (int j = 0; j <= 19; j++) {
        random_y_num = rand() % 4;
        *(safe_y_coordinates + j) = random_y_num;
    }
    // UNSAFE - X Create a Random Number
    for (int i = 0; i < 10; i++) {
        unsafe_random_x_num = rand() % (NUMBER_OF_COLUMNS - 1);
        *(unsafe_x_coordinates + i) = unsafe_random_x_num;
    }
    // UNSAFE - Y Create a Random Number
    for (int j = 0; j < 10; j++) {
        unsafe_random_y_num = rand() % 3;
        *(unsafe_y_coordinates + j) = (unsafe_random_y_num);
    }
}

void draw_platform() {

    // Starting Block
    start_block = sprite_create(0, (1 * (MAX_BLOCK_HEIGHT + (HERO_HEIGHT + 2 + 3))), MAX_BLOCK_WIDTH, MAX_BLOCK_HEIGHT, safe_block_image);
    sprite_draw(start_block);

    // Draw at Least 1 Platform in Each Column
    for (int i = 0; i <= (NUMBER_OF_COLUMNS - 1); i++) {
        // Itterate X to = 1, 2, 3, etc to Place 1 Platform in each Column
        int x = i * (MAX_BLOCK_WIDTH + 2);
        //+ MAX_BLOCK_HEIGHT + 2
        int y = (init_random_y_values[i] * ((MAX_BLOCK_HEIGHT + (HERO_HEIGHT + 2)) ));
        // Draw Each Safe Block || Add Display Height and Player Height to Give first Platform
        initial_safe_platforms[i] = sprite_create(x, (y + DISPLAY_HEIGHT + HERO_HEIGHT), MAX_BLOCK_WIDTH, MAX_BLOCK_HEIGHT, safe_block_image);
    }
    // 20 Random Safe Platforms
    for (int i = 0; i <= 19; i++) {  
        int x = safe_x_coordinates[i] * (MAX_BLOCK_WIDTH + 2);
        int y = (safe_y_coordinates[i] * (MAX_BLOCK_HEIGHT + (HERO_HEIGHT + 2)) + DISPLAY_HEIGHT + MAX_BLOCK_HEIGHT + 2);
        random_safe_platforms[i] = sprite_create(x, y, MAX_BLOCK_WIDTH, MAX_BLOCK_HEIGHT, safe_block_image);
    }
    // 5 Random Unsafe Blocks
    for (int i = 0; i < 10; i++) {
        int x = unsafe_x_coordinates[i] * (MAX_BLOCK_WIDTH + 2);
        int y = (unsafe_y_coordinates[i + 1] * (MAX_BLOCK_HEIGHT + (HERO_HEIGHT + 2)) + DISPLAY_HEIGHT + MAX_BLOCK_HEIGHT + 2);
        unsafe_platforms[i] = sprite_create(x, y, MAX_BLOCK_WIDTH, MAX_BLOCK_HEIGHT, unsafe_block_image);
        sprite_draw(unsafe_platforms[i]);   
    }
}


void create_treasure()
{
    int h = screen_height() - 1, w = screen_width() - 1;
    int left = 1, top = 0, right = w, bottom = h - 2, lower_top = 3;
    treasure = sprite_create((SCREEN_WIDTH / 2), (SCREEN_HEIGHT - (TREASURE_HEIGHT + 2)), TREASURE_WIDTH, TREASURE_HEIGHT, treasure_image_1);
    desmond = sprite_create((left + 1), (top + DISPLAY_HEIGHT), HERO_WIDTH, HERO_HEIGHT, hero_image_right);
    sprite_turn_to(treasure, +0.2, 0);
}


void move_treasure(int key_code)
{
    sprite_step(treasure);
    // Get screen location of treasure
    int treasure_x = round(sprite_x(treasure));
    int treasure_y = round(sprite_y(treasure));
    // // Get the displacement vector of the treasure
    double treasure_dx = sprite_dx(treasure);
    double treasure_dy = sprite_dy(treasure);

    if (treasure_x == 0 || treasure_x == (SCREEN_WIDTH - TREASURE_WIDTH)) {

        treasure_dx = -treasure_dx;
        sprite_turn_to(treasure, treasure_dx, 0);

        
    }
}

void menu_setup() {
    // Display Screen
    int h = screen_height() - 1, w = screen_width() - 1;
    int left = 1, top = 0, right = w, bottom = h - 2, lower_top = 3;
    // Draw Top & Lower Borders
    draw_line(left, top, right, top, '_');
    draw_line(left, lower_top, right, lower_top, '_');
    // Place Student Number, Title, Score, Time & Lives & Centre
    draw_formatted(((SCREEN_WIDTH / 2) - (MENU_WIDTH / 2)), (top + 2), "TIME:%02i:%02i | N9968296 | THE ADVENTURES OF DESMOND DUCK | LIVES:%d | SCORE:%d", minutes, seconds, lives, score); 
}


bool sprites_collide(sprite_id sprite_one, sprite_id sprite_two) {

    // Sprite One
    int top1 = round(sprite_y(sprite_one));
    int bottom1 = top1 + sprite_height(sprite_one);
    int left1 = round(sprite_x(sprite_one));
    int right1 = left1 + sprite_width(sprite_one);
    // Sprite Two
    int top2 = round(sprite_y(sprite_two));
    int bottom2 = top2 + sprite_height(sprite_two);
    int left2 = round(sprite_x(sprite_two));
    int right2 = left2 + sprite_width(sprite_two);

    // Box Collision
    if (top1 > bottom2)
    {
        return false;
    }
    else if (top2 > bottom1)
    {
        return false;
    }
    else if (right1 < left2)
    {
        return false;
    }
    else if (right2 < left1)
    {
        return false;
    }
    else 
    {
        return true;
    }
}


// Collision for When Desmond contacts any Platform
sprite_id sprites_collide_any(sprite_id s, sprite_id sprite_platforms[], int number)
{
    sprite_id result = NULL;

    for (int i = 0; i < number; i++)
    {
        if(sprites_collide(s, sprite_platforms[i]))
        {
            result = sprite_platforms[i];
            score++;
            break;
        }
    }
    return result;
}

// Respawn Function
void restart() {

    int h = SCREEN_HEIGHT - 1, w = screen_width() - 1;
    int left = 1, top = 0, right = w, bottom = h - 2, lower_top = 3;

    // Respawn
    start_block = sprite_create(0, (1 * (MAX_BLOCK_HEIGHT + (HERO_HEIGHT + 2 + 3))), MAX_BLOCK_WIDTH, MAX_BLOCK_HEIGHT, safe_block_image);
    sprite_draw(start_block);
    platform();
    draw_platform();
    desmond = sprite_create((left + 1), (top + DISPLAY_HEIGHT), HERO_WIDTH, HERO_HEIGHT, hero_image_right);
    sprite_draw(desmond);
    sprite_draw(treasure);

    // Draw Platforms
    for (int i = 0; i < 10; i++){
        sprite_draw(initial_safe_platforms[i]);
    }
    for (int i = 0; i < 20; i++){
        sprite_draw(random_safe_platforms[i]);
    }
    for (int i = 0; i < 10; i++){
        sprite_draw(unsafe_platforms[i]);
    }
}


void hero_movement(int key_code, int term_width, int term_height) {

    // Get X & Y Location
    int hx = round(sprite_x(desmond));
    int hy = round(sprite_y(desmond));
    
    // Move Desmond left according to specification.
    if (key_code == 'a' && hx > 1) 
    {
    sprite_move(desmond, -1, 0);
    sprite_set_image(desmond, hero_image_right);
    }
    // Move Desmond right according to specification.
    if (key_code == 'd' && hx < screen_width() - HERO_WIDTH - 1) 
    {
    sprite_move(desmond, +1, 0);
    sprite_set_image(desmond, hero_image_right);
    }
    // Desmond Jumps according to specification.
    if (key_code == 'w' && hy > 1) {
    
    sprite_move(desmond, 0, -2);

    }
    if (hy > SCREEN_HEIGHT - HERO_HEIGHT) {

        lives = lives - 2;
        restart();
    }
}

void welcome() {
    // Position Title Centre
    sprite_id welcome_title = sprite_create(SCREEN_WIDTH / 2 - (71 / 2), ((SCREEN_HEIGHT / 2) - 12), 71, 12, welcome_image);
    sprite_draw(welcome_title);
}

void show_score() {
    // Display Score
    clear_screen();
    draw_formatted((SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2), "YOUR SCORE:%d     TIME:%02i:%02i   WELL DONE!", score, time);
    timer_pause(10);
}

// Set Up Game
void setup(void)
{
    for () {

        welcome();
        show_screen();
        timer_pause(10);
        clear_screen();
    }

    // Prepare Screen for Gameplay
    clear_screen();
    menu_setup();
    game_timer = create_timer(10);
    create_treasure();
    platform();
    draw_platform();
    sprite_draw(desmond);
    
    // Draw Platforms
    start_block = sprite_create(0, (1 * (MAX_BLOCK_HEIGHT + (HERO_HEIGHT + 2 + 3))), MAX_BLOCK_WIDTH, MAX_BLOCK_HEIGHT, safe_block_image);
    sprite_draw(start_block);
    for (int i = 0; i < 10; i++){
        sprite_draw(unsafe_platforms[i]);
    }
    for (int i = 0; i < 10; i++){
        sprite_draw(initial_safe_platforms[i]);
    }
    for (int i = 0; i < 20; i++){
        sprite_draw(random_safe_platforms[i]);
    }
}


// Start / Play One Turn of the Game.
void process()
{   
    if (lives > 0) {
    // Clear Screen
    clear_screen();

    // Set Up Display
    menu_setup();
    int w = screen_width();
    int h = screen_height();
    update_game_timer();
    
    // Desmond Collision & Movement
    int key_code = get_char();
    hero_movement(key_code, w, h);

    // With Starting Block
    if (sprites_collide(desmond, start_block)) {
        desmond -> dy = 0;
        hero_movement(key_code, w, h);
    }
    // With Platforms -> Unsafe
    else if ((sprites_collide_any(desmond, initial_safe_platforms, 12)) || (sprites_collide_any(desmond, random_safe_platforms, 20)))
    {
        desmond -> dy = 0;
        hero_movement(key_code, w, h);
    }
    else if ((sprites_collide_any(desmond, unsafe_platforms, 10))) {
        lives = lives - 1;
        restart();
    }

    // With Treasure
    else if ((sprites_collide(desmond, treasure)))
    {
        lives = lives + 2;
        // Respawn
        restart();
    } 
    else 
    { 
        // Apply Gravity
        desmond -> dy += gravity;
        sprite_step(desmond);
        sprite_set_image(desmond, hero_image_fall);
    }   

    // If User Quits Game with Command
    if (key_code == 'q')
    {
        clear_screen();
        sprite_id game_over_msg =  sprite_create((SCREEN_WIDTH / 2), (SCREEN_HEIGHT / 2), 55, 2, game_over_image);
        sprite_draw(game_over_msg);
        show_screen();
        game_over = true;
    }

    sprite_draw(desmond);
    sprite_draw(treasure);
    move_treasure(key_code);

    // Drawing Platforms
    for (int i = 0; i < 10; i++){
        sprite_draw(unsafe_platforms[i]);
    }
    for (int i = 0; i < 10; i++){
        sprite_draw(initial_safe_platforms[i]);
    }
    for (int i = 0; i < 20; i++){
        sprite_draw(random_safe_platforms[i]);
    }
    sprite_id safe_block = sprite_create(0, (1 * (MAX_BLOCK_HEIGHT + (HERO_HEIGHT + 2 + 3))), MAX_BLOCK_WIDTH, MAX_BLOCK_HEIGHT, safe_block_image);
    sprite_draw(safe_block);
    
    }
    else {
        // Game Over
        clear_screen();


        game_over = true;
    }
}

// Clean Up Game
void cleanup(void)
{
    // STATEMENTS
}

// Program Entry Point
int main(void)
{
    setup_screen();  

#if defined(AUTO_SAVE_SCREEN)
    auto_save_screen(true);
#endif

    setup();
    show_screen();

    while (!game_over)
    {
        process();

        if (update_screen)
        {
            show_screen();
        }

        timer_pause(DELAY);
    }
    // show_score();
    cleanup();

    return 0;
}
