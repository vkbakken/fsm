# fsm
FSM Framework

go to "Default" directory then execute command: ```make all && ./fsm```to build and execute FSM test app.

The app is a simple PIN unlock form with correct sequence:
1. press "Enter" key to begin enter PIN code.
2. Enter the correct PIN code is "20122019*" to go to passed state.
3. press "Esc" key at anytime to quit the app.

The traffic light is a app to integrate testing the workq and FSM work together
go to "Integrate" directory then execute command: ```make all && ./traffic-light```to build and execute integrate test app.