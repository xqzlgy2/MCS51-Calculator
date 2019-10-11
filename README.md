# MCS51-Calculator
Three kinds of simple calculators based on MCS51.</br>
cal.c: Support float number, don't support brackets.</br>
cal2.c: Support brackets, but don't support float number.</br>
cal3.c: Support both brackets and float number. (implemented by a stack plus state machine)</br></br>
The implementation is based on state machine as following.

<img src="https://github.com/xqzlgy2/MCS51-Calculator/blob/master/statemachine.png"  width="420" height="600">
