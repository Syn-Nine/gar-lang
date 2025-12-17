// Rock-Paper-Scissors Game
var p_score, c_score = 0, 0
var tries = 3
var max = tries

var ROCK, PAPER, SCISSORS, INVALID = 0, 1, 2, 3
var TIE, PLAYER_WIN, COMPUTER_WIN = 0, 1, 2

println("Let's play Rock-Paper-Scissors!")

while tries > 0 {
    print("Best out of " + max as string + ", " + tries as string + " tries remaining. ")
    println("What is your guess? [r]ock, [p]aper, or [s]cissors?")
    
    var guess = INVALID
    loop {
        var s = input()
        if "r" == s      { guess = ROCK;     println("Player: Rock");     break }
        else if "p" == s { guess = PAPER;    println("Player: Paper");    break }
        else if "s" == s { guess = SCISSORS; println("Player: Scissors"); break }
        println("input invalid.")
    }
    
    var comp = randi(0, 2)
    var choice = "Rock"
    if PAPER == comp            { choice = "Paper"    }
    else if SCISSORS == comp    { choice = "Scissors" }
    println("Computer: " + choice)
    
    var result = TIE
    if (ROCK     == comp && SCISSORS == guess) || 
       (SCISSORS == comp && PAPER    == guess) ||
       (PAPER    == comp && ROCK     == guess) {
           result = COMPUTER_WIN
       }
    else if (guess != comp) {
           result = PLAYER_WIN
       }

    if TIE == result {
        println("Tie!")
        
    } else if COMPUTER_WIN == result {
        println("Computer Score!")
        c_score = c_score + 1
        tries = tries - 1
        
    } else if PLAYER_WIN == result {
        println("Player Score!")
        p_score = p_score + 1
        tries = tries - 1
    }

    println("Score: Player: " + p_score as string + ", Computer: " + c_score as string)
}

if p_score == c_score {
    println("GAME TIED!")
} else if p_score > c_score {
    println("PLAYER WINS GAME!")
} else {
    println("COMPUTER WINS GAME!")
}
