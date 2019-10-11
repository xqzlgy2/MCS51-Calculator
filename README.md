# MCS51-Calculator
Three kinds of simple calculators based on MCS51.
cal.c: Support float number, don't support brackets.
cal2.c: Support brackets, but don't support float number.
cal3.c: Support both brackets and float number. (implemented by a stack plus state machine)
The implementation is based on state machine as following.

<img src="https://github.com/xqzlgy2/MCS51-Calculator/blob/master/statemachine.png"  width="420" height="600">
