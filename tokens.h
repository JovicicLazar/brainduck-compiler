#pragma once

enum Tokens {
    MoveRight,         //  ">" move to the right 
    MoveLeft,          //  "<" move to the left  
    Increment,         //  "+" increment the current field  
    Decrement,         //  "-" decrement the current field 
    Print,             //  "." print element from the current field 
    Input,             //  "," input to the current field 
    OpenBracket,       //  "[" start of a loop 
    CloseBracket,      //  "]" end of a loof (if the current element of a field is 0 then the loop is over)
    Comment,           // anything else aside of these 8 commands will be considered as a comment
};