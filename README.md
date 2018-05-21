# Design with data - The scentmap


## Repository structure
```
.
└── scentmap_web (web application to generate the personalised scentmap)
|	└── static
|	└── template
|	└── scentmap.py
└── arduino
| └── dwd_arduino.ino
└── README.md
```

## Team Members
Sally,
Vicky,
Hong,
Yaguang,
Liquan

## Depedencies
scentmap_web:
  Python 3.5.4
  Flask
  requests
 
Arduino:
  Adafruit_NeoPixel
  TimedAction https://playground.arduino.cc/Code/TimedAction
  NOTE: This library has an issue on newer versions of Arduino. After downloading the library you MUST go into the library directory and edit TimedAction.h. Within, overwrite WProgram.h with Arduino.h
  

To install dependencies, use following command:
```
pip install Flask

pip install requests
```
