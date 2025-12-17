// Guess-The-Number Game
var tries = 6
var secret = randi(1, 20)

while tries > 0 {
    println("Enter a guess from 1 to 20, " + tries as string + " tries remaining")

    var guess = input() as int
    if 0 == guess { continue; }
    
    if guess < secret {
        println("Too Small")
    }
    else if guess > secret {
        println("Too Large")
    }
    else {
        println("You Win!")
        tries = 0
    }
    
    tries = tries - 1
    if 0 == tries {
        println("Game Over")
    }
}
