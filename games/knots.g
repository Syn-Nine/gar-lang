//-----------------------------------------------------------------------------
// Knots minigame by Syn9
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------

const XRES = 640
const YRES = 480
const XRES_HALF = XRES / 2
const YRES_HALF = YRES / 2
const SCALE = 2
const NUM_TILES = 28


//-----------------------------------------------------------------------------
// Game State
//-----------------------------------------------------------------------------
var clickdelay = false
var selidx = -1
var rotating = false
var moveidx1 = -1
var moveidx2 = -1
var winlock = false
var solvedalpha = 0
var solvedtimer = 0
var quit = false

var tile_x = new(fill(0, NUM_TILES))
var tile_y = new(fill(0, NUM_TILES))
var tile_an = new(fill(0, NUM_TILES))
var tile_tx = new(fill(0, NUM_TILES))
var tile_ty = new(fill(0, NUM_TILES))
var tile_tan = new(fill(0, NUM_TILES))
var tilemap = new(fill(0, NUM_TILES))


//-----------------------------------------------------------------------------
// Resources
//-----------------------------------------------------------------------------
ray::InitWindow(XRES * SCALE, YRES * SCALE, "Knots")
ray::SetTargetFPS(60)

var tiles = new(fill(0, NUM_TILES))
var active = new(fill(0, NUM_TILES))

for i in 0..NUM_TILES {
    tiles[i] = ray::LoadTexture("assets/hex-" + (i + 1) as string + ".png")
    active[i] = ray::LoadTexture("assets/active/hex-" + (i + 1) as string + ".png")
}

var hex_edge = ray::LoadTexture("assets/hex-edge.png")
var overlay = ray::LoadTexture("assets/overlay.png")


//-----------------------------------------------------------------------------
// Main Loop
//-----------------------------------------------------------------------------
initialize()

loop {
    render()
    update()
    check_events()
    if ray::WindowShouldClose() || quit { break }
}

ray::CloseWindow()


//-----------------------------------------------------------------------------
def initialize() {

    var map = [
        0, 0, 0, 1, 0, 1, 0, 0, 0,
        0, 0, 1, 0, 1, 0, 1, 0, 0,
        0, 1, 0, 1, 0, 1, 0, 1, 0,
        0, 0, 1, 0, 1, 0, 1, 0, 0,
        0, 1, 0, 1, 0, 1, 0, 1, 0,
        0, 0, 1, 0, 1, 0, 1, 0, 0,
        0, 1, 0, 1, 0, 1, 0, 1, 0,
        0, 0, 1, 0, 1, 0, 1, 0, 0,
        0, 0, 0, 1, 0, 1, 0, 0, 0,
    ]

    var mx = 9
    var my = (len(map) / mx) as int
    var dx = 74
    var dy = 42
    var cidx = 0

    for y in 0..my {
        for x in 0..mx {

            var idx = y * mx + x
            if 0 == map[idx] { continue }
            
            var ix = x - 4
            var iy = y - 4

            var tx = XRES_HALF + ix * dx
            var ty = YRES_HALF + iy * dy
            tile_tx[cidx] = tx
            tile_ty[cidx] = ty
            tilemap[cidx] = cidx            
            cidx = cidx + 1
        }   
    }

    // random rotate
    for i in 0..NUM_TILES {
        var an = randi(0, 5)
        an = 0
        tile_tan[i] = an
        tile_an[i] = an * 60
    }

    // random shuffle
    for _ in 0..1000 {
        var a = randi(0, 27)
        var b = randi(0, 27)
        if a == b { continue }

        var tx = tile_tx[a]
        var ty = tile_ty[a]
        tile_tx[a] = tile_tx[b]
        tile_ty[a] = tile_ty[b]
        tile_tx[b] = tx
        tile_ty[b] = ty

        tile_x[a] = tile_tx[a]
        tile_y[a] = tile_ty[a]
        tile_x[b] = tile_tx[b]
        tile_y[b] = tile_ty[b]

        var tidx = tilemap[a]
        tilemap[a] = tilemap[b]
        tilemap[b] = tidx
    }

}


//-----------------------------------------------------------------------------
def check_win() {
    
    for i in 0..NUM_TILES {
        if tile_tan[i] % 6 != 0 || tilemap[i] != i {
            return
        }
    }

    winlock = true
    solvedtimer = 60
}


//-----------------------------------------------------------------------------
def check_events() {

    if clickdelay { return }
    if winlock {
        if solvedalpha == 60 && ray::IsMouseButtonReleased(:MOUSE_BUTTON_LEFT) || ray::IsMouseButtonReleased(:MOUSE_BUTTON_RIGHT) {
            quit = true
        }
        return
    }

    if ray::IsMouseButtonReleased(:MOUSE_BUTTON_LEFT) || ray::IsMouseButtonReleased(:MOUSE_BUTTON_RIGHT) {
        var previdx = selidx
        var prevrot = rotating
        selidx = -1
        rotating = false
        
        var mx = ray::GetMouseX() / SCALE
        var my = ray::GetMouseY() / SCALE
        
        for i in 0..NUM_TILES {
            var cx = tile_tx[i]
            var cy = tile_ty[i]
            var delta = sqrt((cx - mx) * (cx - mx) + (cy - my) * (cy - my))
            if delta < 42 {
                selidx = i
                
                if previdx == selidx {
                    tile_tan[i] = tile_tan[i] + 1
                    rotating = true
                }
                else if -1 != previdx && !prevrot {
                    var tidx = tilemap[previdx]
                    tilemap[previdx] = tilemap[selidx]
                    tilemap[selidx] = tidx

                    var tx = tile_tx[previdx]
                    var ty = tile_ty[previdx]
                    tile_tx[previdx] = tile_tx[selidx]
                    tile_ty[previdx] = tile_ty[selidx]
                    tile_tx[selidx] = tx
                    tile_ty[selidx] = ty

                    rotating = false
                    moveidx1 = previdx
                    moveidx2 = selidx
                    selidx = -1
                }

                break
            }            
        }
        check_win()
    }
}


//-----------------------------------------------------------------------------
def render() {
    ray::BeginDrawing()
    ray::ClearBackground(:BLACK)
        var edge_alpha = 1.0 - solvedalpha / 60.0

        for i in 0..NUM_TILES {
            draw_tile(i, false, edge_alpha)
        }

        if -1 != selidx { draw_tile(selidx, true, edge_alpha)    }
        if -1 != moveidx1 { draw_tile(moveidx1, true, edge_alpha) }
        if -1 != moveidx2 { draw_tile(moveidx2, true, edge_alpha) }
        
        ray::DrawTexturePro(overlay, 0, 0, XRES, YRES, 0, 0, XRES * SCALE, YRES * SCALE, 0, 0, 0, :WHITE)
    ray::EndDrawing()
}


//-----------------------------------------------------------------------------
def draw_tile(idx, highlight, edge_alpha) {
    var x = tile_x[idx]
    var y = tile_y[idx]
    var an = tile_an[idx]
    if highlight {
        ray::DrawTexturePro(active[idx], 0, 0, 100, 100, x * SCALE, y * SCALE, 100 * SCALE, 100 * SCALE, 50 * SCALE, 50 * SCALE, an, :WHITE)
    } else {
        ray::DrawTexturePro(tiles[idx], 0, 0, 100, 100, x * SCALE, y * SCALE, 100 * SCALE, 100 * SCALE, 50 * SCALE, 50 * SCALE, an, :WHITE)
    }
    ray::DrawTextureProRGBA(hex_edge, 0, 0, 100, 100, x * SCALE, y * SCALE, 100 * SCALE, 100 * SCALE, 50 * SCALE, 50 * SCALE, an, 255, 255, 255, 255 * edge_alpha)
}


//-----------------------------------------------------------------------------
def update() {

    clickdelay = false
    var rot_speed = 0.15
    var tx_speed = 0.15
    var moving = false
    var mindx = 3
    var mintx = 40

    for i in 0..NUM_TILES {
        var dx = tile_tx[i] - tile_x[i]
        if dx > mintx { dx = mintx }
        if dx < -mintx { dx = -mintx }

        var dy = tile_ty[i] - tile_y[i]
        if dy > mintx { dy = mintx }
        if dy < -mintx { dy = -mintx }

        var dan = tile_tan[i] * 60 - tile_an[i]
        var delta = sqrt(dx * dx + dy * dy)

        if mindx > delta {
            tile_x[i] = tile_tx[i]
            tile_y[i] = tile_ty[i]
        } else {
            tile_x[i] = tile_x[i] + dx * tx_speed
            tile_y[i] = tile_y[i] + dy * tx_speed
            clickdelay = true
            moving = true
        }

        if abs(dan) < 2 {
            tile_an[i] = tile_tan[i] * 60
        } else {
            tile_an[i] = tile_an[i] + dan * rot_speed
            clickdelay = true
        }
    }

    if !moving {
        moveidx1 = -1
        moveidx2 = -1
    }

    if winlock {
        solvedtimer = solvedtimer - 1
        if 0 > solvedtimer {
            solvedalpha = solvedalpha + 1
            if 60 < solvedalpha { solvedalpha = 60 }
        }
    }

}
