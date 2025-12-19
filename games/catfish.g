//-----------------------------------------------------------------------------
// Catfish Bouncer - Raylib minigame by Syn9
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

// window res
const XRES = 800
const YRES = 600
const XRES_HALF = XRES / 2
const YRES_HALF = YRES / 2

// entity colors
const BG_COLOR = :RAYWHITE
const CAT_COLOR = :ORANGE
const TXT_COLOR = :LIGHTGRAY
const YUM_COLOR = :SKYBLUE
const BALL_COLOR = :SKYBLUE
const PADDLE_COLOR = :GRAY

// game floats
const START_SPEED = 300.0
const INC_SPEED = 20.0
const FEED_RADIUS = 50.0
const YUM_MAX = 100.0
// game ints
const BALL_RADIUS = 20
const PADDLE_W = 50
const PADDLE_H = 10


//-----------------------------------------------------------------------------
// Global Game Data
//-----------------------------------------------------------------------------
// clock
var t_prev = 0.0
var t_base = 0.0
var t_delta = 0.0

// ball
var ball_x = 0.0
var ball_y = 0.0
var ball_dx = 0.0
var ball_dy = 0.0
var ball_speed = 0.0

// mouse
var mx = 0
var my = 0
var prev_mx = 0
var prev_my = 0
var dmx = 0.0
var dmy = 0.0

// state
var yumY = 0.0
var fish = 0
var newBall = false


//-----------------------------------------------------------------------------
// Main Loop
//-----------------------------------------------------------------------------

ray::InitWindow(XRES, YRES, "Catfish Bouncer")
ray::SetTargetFPS(60)

t_prev = ray::GetTime()
t_base = ray::GetTime()

loop {
    update_game_state()
    draw_game_board()
    
    if ray::WindowShouldClose() { break; }
}

ray::CloseWindow()




//-----------------------------------------------------------------------------
// Function Definitions
//-----------------------------------------------------------------------------

def draw_game_board() {
    ray::BeginDrawing()
    ray::ClearBackground(BG_COLOR)
    
    // testing text output
    ray::DrawText("Catfish Bouncer!", 20, 20, 20, TXT_COLOR)
    ray::DrawText("fps: " + (1 / t_delta) as string, 20, 40, 20, TXT_COLOR)
    ray::DrawText("clock: " + (ray::GetTime() - t_base) as string, 20, 60, 20, TXT_COLOR)
    
    // draw cat
    ray::DrawCircle(XRES_HALF, YRES_HALF, 30, CAT_COLOR)
    var w = 15.0
    var h = 50.0
    ray::DrawTriangle(XRES_HALF, YRES_HALF, XRES_HALF - w, YRES_HALF - h, XRES_HALF - w - w, YRES_HALF, CAT_COLOR)
    ray::DrawTriangle(XRES_HALF, YRES_HALF, XRES_HALF + w + w, YRES_HALF, XRES_HALF + w, YRES_HALF - h, CAT_COLOR)
    
    // draw fish
    for i in 0..fish {
        ray::DrawCircle(XRES_HALF + ( i - ( fish - 1.00 ) / 2 ) * 12, YRES_HALF + 50, 5, YUM_COLOR)
    }
    
    // draw ball
    if !newBall {
        ray::DrawCircle(ball_x, ball_y, BALL_RADIUS, BALL_COLOR)
    }
    
    // draw yum text
    yumY = yumY - t_delta
    if (yumY > 0) {
        ray::DrawText("YUM!", XRES_HALF - 50, YRES_HALF - 50 - YUM_MAX * ( 1 - yumY ), 40, YUM_COLOR)
    }
    
    // draw paddles
    ray::DrawRectangle(mx - PADDLE_W, 0, PADDLE_W * 2, PADDLE_H, PADDLE_COLOR)
    ray::DrawRectangle(mx - PADDLE_W, YRES - PADDLE_H, PADDLE_W * 2, PADDLE_H, PADDLE_COLOR)
    ray::DrawRectangle(0, my - PADDLE_W, PADDLE_H, PADDLE_W * 2, PADDLE_COLOR)
    ray::DrawRectangle(XRES - PADDLE_H, my - PADDLE_W, PADDLE_H, PADDLE_W * 2, PADDLE_COLOR)
    
    ray::EndDrawing()
}


// game logic
def update_game_state() {
    // mouse delta for moving paddles and applying friction
    prev_mx = mx
    prev_my = my
    mx = ray::GetMouseX()
    my = ray::GetMouseY()
    dmx = dmx * 0.80 + ( mx - prev_mx ) * ( 1 - 0.80 )
    dmy = dmy * 0.80 + ( my - prev_my ) * ( 1 - 0.80 )
    
    // time delta for animation
    var t_clock = ray::GetTime()
    t_delta = t_clock - t_prev
	if t_delta < 0.000001 { t_delta = 0.000001 }
    t_prev = t_clock
    
    
    // is ball outside of game board?
    if ( yumY < 0 && newBall ) || PADDLE_H > ball_x || XRES - PADDLE_H < ball_x || PADDLE_H > ball_y || YRES - PADDLE_H < ball_y {
        // initialize position
		ball_x = 100.0 + rand() * ( XRES_HALF - 200 )
		if rand() < 0.50 {
            ball_x = XRES_HALF + ball_x
        }
		ball_y = 100.0 + rand() * ( YRES - 200 )

        // initialize velocity
		ball_speed = START_SPEED
        var angle = rand() * 3.141593 * 2.00
        ball_dx = ball_speed * cos(angle)
        ball_dy = ball_speed * sin(angle)
        newBall = false
    }
    
    // update ball state
    if (!newBall) {
        // integrate ball location
        ball_x = ball_x + ball_dx * t_delta
        ball_y = ball_y + ball_dy * t_delta
	
        // top/bottom bounce
        if ball_x > mx - PADDLE_W && ball_x < mx + PADDLE_W {
            if ball_y - BALL_RADIUS < PADDLE_H || ball_y + BALL_RADIUS > YRES - PADDLE_H {
                ball_dy = - ball_dy
                ball_y = PADDLE_H + BALL_RADIUS
                if (ball_dy < 0) {
                    ball_y = YRES - PADDLE_H - BALL_RADIUS
                }
                ball_dx = ball_dx + dmx * ball_speed / 20
                ball_speed = ball_speed + INC_SPEED
            }
        }
	
        // left/right bounce
        if ball_y > my - PADDLE_W && ball_y < my + PADDLE_W {
            if ball_x - BALL_RADIUS < PADDLE_H || ball_x + BALL_RADIUS > XRES - PADDLE_H {
                ball_dx = - ball_dx
                ball_x = PADDLE_H + BALL_RADIUS
                if (ball_dx < 0) {
                    ball_x = XRES - PADDLE_H - BALL_RADIUS
                }
                ball_dy = ball_dy + dmy * ball_speed / 20
                ball_speed = ball_speed + INC_SPEED
            }
        }
	
        // check for feed
        if yumY < 0 && ball_x > XRES_HALF - FEED_RADIUS &&
            ball_x < XRES_HALF + FEED_RADIUS && ball_y > YRES_HALF - FEED_RADIUS &&
            ball_y < YRES_HALF + FEED_RADIUS {
            yumY = 1.00
            newBall = true
            fish = fish + 1
        }
    }
}



