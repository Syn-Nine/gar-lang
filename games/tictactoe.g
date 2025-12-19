// Tic-Tac-Toe Game

const SZ = 9
var board = new([:EMPTY, :EMPTY, :EMPTY, :EMPTY, :EMPTY, :EMPTY, :EMPTY, :EMPTY, :EMPTY])

println("Let's play Tic-Tac-Toe!")
println("-----------------------------------------------------")

loop {
    draw_board()
    
    get_player_input()    
    if check_win(:PLAYER) { break }
    
    get_computer_input()
    if check_win(:COMPUTER) { break }
}
draw_board()


/////////////////////////////////////////////////////////////////////////////////
def check_win(who) {
    var ret = false
    var idx = 0
	
	// check rows
    for row in 0..3 {
        idx = row * 3
        if who == board[idx] && who == board[idx + 1] && who == board[idx + 2] {
            ret = true
        }
    }

    // check columns
    for col in 0..3 {
        idx = col
        if who == board[idx] && who == board[idx + 3] && who == board[idx + 6] {
            ret = true
        }
    }

    // check diagonals
    if who == board[4] {
        if who == board[0] && who == board[8] {
            ret = true
        } else if who == board[2] && who == board[6] {
            ret = true
        }
    }

    if ret {
        if :PLAYER == who  {
            println("Player Wins!")
        } else {
            println("Computer Wins!")
        }
    } else {
        // check for draw
        for i in 0..SZ {
            if :EMPTY == board[i] { break }
            if 8 == i {
                ret = true
                println("DRAW!")
            }
        }
    }
        
    ret
}


/////////////////////////////////////////////////////////////////////////////////
def draw_board() {
    println("/-----------\ ")
    print("| ") draw_cell(0) print(" | ") draw_cell(1) print(" | ") draw_cell(2) println(" | ")
    println("|---|---|---|")
    print("| ") draw_cell(3) print(" | ") draw_cell(4) print(" | ") draw_cell(5) println(" | ")
    println("|---|---|---|")
    print("| ") draw_cell(6) print(" | ") draw_cell(7) print(" | ") draw_cell(8) println(" | ")
    
    println("\-----------/")
}


/////////////////////////////////////////////////////////////////////////////////
def draw_cell(idx) {
	if :EMPTY == board[idx] { print(idx + 1) }
	else if :PLAYER == board[idx] { print("X") }
	else if :COMPUTER == board[idx] { print("O") }
}


/////////////////////////////////////////////////////////////////////////////////
def get_player_input() {
	loop {
        println("Enter the # for your choice (X)")
        var temp = input() as int
        // check for validity
        if temp > 0 && temp < 10 {
            if :EMPTY == board[temp - 1] {
                board[temp - 1] = :PLAYER
                break
            }
        }        
        println("Invalid choice.")
    }
}


/////////////////////////////////////////////////////////////////////////////////
def get_computer_input() {
    loop {
        var choice = randi(0, SZ - 1)
        if :EMPTY == board[choice] {
            board[choice] = :COMPUTER
            break
        }
    }
}
