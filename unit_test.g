// basic types
// assignment
{
    const z = 9
    if 9 != z { println("Test Failed, " + FILELINE) }
    
    var a = 3
    var b = -5
    var c = -5.5
    var d = 2.2
    var e = true
    var f = "test"
    var g = :TEST
    
    if a != 3 { println("Test Failed, " + FILELINE) }
    if 3 != a{ println("Test Failed, " + FILELINE) }
    if b != -5 { println("Test Failed, " + FILELINE) }
    if -5 != b { println("Test Failed, " + FILELINE) }
    if abs(-5.5 - c) > 0.000001 { println("Test Failed, " + FILELINE) }
    if 0.000001 < abs(-5.5 - c) { println("Test Failed, " + FILELINE) }
    if abs(2.2 - d) > 0.000001 { println("Test Failed, " + FILELINE) }
    if 0.000001 < abs(2.2 - d) { println("Test Failed, " + FILELINE) }
    if !e { println("Test Failed, " + FILELINE) }
    if e != true { println("Test Failed, " + FILELINE) }
    if true != e { println("Test Failed, " + FILELINE) }
    if f != "test" { println("Test Failed, " + FILELINE) }
    if "test" != f { println("Test Failed, " + FILELINE) }
    if g != :TEST { println("Test Failed, " + FILELINE) }
    if :TEST != g { println("Test Failed, " + FILELINE) }
}

// casting
{
    var a = 5.7 as float
    var b = 5.7 as int
    var c = 4 as int
    var d = 4 as float
    var e = true as string
    var f = false as string
    var g = 5.55 as string
    var h = 22 as string
    
    var i = e + "--" + f + "--" + g + "--" + h
    
    var j = true as int as float as string
    
    if abs(5.7 - a) > 0.000001 { println("Test Failed, " + FILELINE) }
    if b != 5 { println("Test Failed, " + FILELINE) }
    if c != 4 { println("Test Failed, " + FILELINE) }
    if abs(d - 4) > 0.000001 { println("Test Failed, " + FILELINE) }
    if e != "true" { println("Test Failed, " + FILELINE) }
    if f != "false" { println("Test Failed, " + FILELINE) }
    if g != "5.550000" { println("Test Failed, " + FILELINE) }
    if h != "22" { println("Test Failed, " + FILELINE) }
    
    if i != "true--false--5.550000--22" { println("Test Failed, " + FILELINE) }
    if j != "1.000000" { println("Test Failed, " + FILELINE) }
    
    // copy string to heap
    var k = new(i)
    if k != i { println("Test Failed, " + FILELINE) }
    k = new("testtest")
    if 8 != len(k) { println("Test Failed, " + FILELINE) }
}

// redefinition
{
    var c = 5
    var c = 9
    if 9 != c { println("Test Failed, " + FILELINE) }
    
    var age = 22.2
    var age = age as string
    var age = age as int
    if 22 != age { println("Test Failed, " + FILELINE) }
    
    // compound assignment
    var a, b = 1, 2
    if a != 1 { println("Test Failed, " + FILELINE) }
    if b != 2 { println("Test Failed, " + FILELINE) }
}

// conditional logic
{
    // simple conditions
    if 2 < 1 { println("Test Failed, " + FILELINE) }
    if 2 <= 1 { println("Test Failed, " + FILELINE) }
    if 1 > 2 { println("Test Failed, " + FILELINE) }
    if 1 >= 2 { println("Test Failed, " + FILELINE) }
    if 1 == 2 { println("Test Failed, " + FILELINE) }
    if 1 != 1 { println("Test Failed, " + FILELINE) }
    
    // if, else
    var x = 0
    if 1 != 1 {
        x = 1
    }
    else if 2 != 2 {
        x = 2
    }
    else {
        x = 3
    }
    if 3 != x { println("Test Failed, " + FILELINE) }
    
    // and or
    var x = 20
    var y = 0
    if x >= 10 && x <= 30 {
        y = 1
        if x > 10 || x > 30 {
            y = 2
            if (false != true && 5 > 7) || x < 10 {
                y = 3
                println("Test Failed, " + FILELINE)
            }
        }
    }
    if 2 != y { println("Test Failed, " + FILELINE) }
}

// looping
{
    var x = 0
    loop {
        x = x + 1
        break
    }
    if 1 != x { println("Test Failed, " + FILELINE) }
    
    var x = 0
    for i in 0..5 {
        x = x + i
    }
    if 10 != x { println("Test Failed, " + FILELINE) }
    
    var x = 2
    var y = 4
    var z = 0
    for i in x..=(x + y) { // same as for i in 2..=6
        z = z + i
    }
    if 20 != z { println("Test Failed, " + FILELINE) }
}

// break and continue
{
    var x = 0
    for i in 0..7 {
        if i > 3 {
            break
        }
        x = x + i
    }
    if 6 != x { println("Test Failed, " + FILELINE) }

    var x = 0
    var i = 0
    loop {
        i = i + 1
        if i > 2 { break }
        x = x + i
    }
    if 3 != x { println("Test Failed, " + FILELINE) }
    
    var x = 0
    for i in 0..7 {
        if i < 4 {
            continue
        }
        x = x + i
    }
    if 15 != x { println("Test Failed, " + FILELINE) }

    var x = 0
    var i = 0
    var output = ""
    while i < 5 {
        i = i + 1
        output = output + "ba"   // every cycle add: ba
        if i % 2 == 0 {
            output = output + "-"
            continue     // skip the rest if i is an even number.
        } 
        output = output + "rf! " // every-other-cycle add: rf
        x = x + i
    }
    if "barf! ba-barf! ba-barf! " != output { println("Test Failed, " + FILELINE) }
    if 9 != x { println("Test Failed, " + FILELINE) }
    
    // break and continue combo test
    var a = 0
    loop {
        a = a + 1
        if a > 5 { break }
        continue
        a = a - 1
    }
    if 6 != a { println("Test Failed, " + FILELINE) }

}


// basic math
{
	var a = (2 * (1 + 3)) as int
	var b = (4 / 2 * 7) as int
	var c = ((1 + 4 * 6 - 2 * 8) / 3) as float
	var d = (-6 + 3 ^ 2 * 2) as int
	var e = (c * d / a + b) as float
    
    if abs(3.2 - (1 + 2.2)) > 0.000001 { println("Test Failed, " + FILELINE) }
	if abs(-2.6 - (5.4 - 8)) > 0.000001 { println("Test Failed, " + FILELINE) }
	if abs(6.6 - (2 * 3.3)) > 0.000001 { println("Test Failed, " + FILELINE) }
	if abs(5 - 10/2) > 0.000001 { println("Test Failed, " + FILELINE) }
	if 8 != a { println("Test Failed, " + FILELINE) }
	if 14 != b { println("Test Failed, " + FILELINE) }
	if abs(3 - c) > 0.000001 { println("Test Failed, " + FILELINE) }
	if d != 12 { println("Test Failed, " + FILELINE) }
	if abs(18.5 - e) > 0.000001 { println("Test Failed, " + FILELINE) }
	
	var a = (2 ^ 3) as int
	var b = (2 ^ 3) as float
	if a != 8 { println("Test Failed, " + FILELINE) }
	if abs(8 - b) > 0.000001 { println("Test Failed, " + FILELINE) }

}

// lists
{
    var x = [1, 2.0, :TEST, false, "test"]
    if 5 != len(x) { println("Test Failed, " + FILELINE) }
    if :TEST != x[2] { println("Test Failed, " + FILELINE) }
    x[0] = x[0] + 2
    if 3 != x[0] { println("Test Failed, " + FILELINE) }
    x[3] = !x[3]
    if !x[3] { println("Test Failed, " + FILELINE) }
    if x[3] != true { println("Test Failed, " + FILELINE) }
    x[4] = x[4] + "2"
    if "test2" != x[4] { println("Test Failed, " + FILELINE) }
    
    var y = [1, "test"]
    var x = [3.3, y]

    if 2 != len(x) { println("Test Failed, " + FILELINE) }
    if 2 != len(x[1]) { println("Test Failed, " + FILELINE) }
    if 1 != x[1][0] { println("Test Failed, " + FILELINE) }
    if 4 != len(x[1][1]) { println("Test Failed, " + FILELINE) }
    if abs(3.3 - x[0]) > 0.000001 { println("Test Failed, " + FILELINE) }

    var i = 1
    x[i][i] = "foo"
    if "foo" != x[1][1] { println("Test Failed, " + FILELINE) }
    
    var x = [1, 2, 3, 4, 5]
    var y = 0
    for i in 0..len(x) {
        y = y + x[i]
    }
    if 15 != y { println("Test Failed, " + FILELINE) }
    
    // copying list
    var z = x
    if 5 != len(z) { println("Test Failed, " + FILELINE) }
    
    // clearing list
    x = []
    if 0 != len(x) { println("Test Failed, " + FILELINE) }

    // filling list
    var x = fill(0, 10) // allocate in scratch
    x[2] = 1
    if x[1] != 0 { println("Test Failed, " + FILELINE) }
    if x[2] != 1 { println("Test Failed, " + FILELINE) }
    if 10 != len(x) { println("Test Failed, " + FILELINE) }
    if 1 != x[2] { println("Test Failed, " + FILELINE) }

    // copy list to heap
    var y = new(x)
    y[2] = 2
    if len(y) != len(x) { println("Test Failed, " + FILELINE) }
    if y[2] == x[2] { println("Test Failed, " + FILELINE) }
    
    var sum = 0
    for i in 0..len(x) {
        x[i] = i
        sum = sum + x[i]
    }
    if 45 != sum { println("Test Failed, " + FILELINE) }
}

// functions
def test1(v) {
    v[1]
}
if 2 != test1([1, 2]) { println("Test Failed, " + FILELINE) }

def test2(a, b) {
    a + b
}
if 3 != test2(1, 2) { println("Test Failed, " + FILELINE) }

