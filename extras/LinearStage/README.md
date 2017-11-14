# LinearStage

Authors:

    Peter Polidoro <polidorop@janelia.hhmi.org>

License:

    BSD

## Linear Drive Setup

VIX250AH-DRIVE

### Wiring

#### RS232 Cables

##### USB (USB-A) to RS232 (D-Sub, 9 Pin Male) Cable

Digikey Part Number: 768-1074-ND
Description: CABLE USB RS232 PREMIUM 1M DB9

##### DB9 Adapter Cable

Connect from USB RS232 Cable to VIX X3

| DB9 Female Sockets | DB9 Male Pins |
|--------------------|---------------|
| 2                  | 5             |
| 3                  | 4             |
| 5                  | 3             |

#### Step/Dir/Enable Cable

Connect from step_dir_controller to VIX X4

| IDC 10 Pin Female Sockets | DB15 Female Sockets |
|---------------------------|---------------------|
| 1                         | 12                  |
| 2                         | 7                   |
| 5                         | 13                  |
| 6                         | 8                   |
| 8                         | 3                   |
| 9                         | 11                  |

#### Power Supply

Parker XLPSU

| Power Supply | VIX X1 |
|--------------|--------|
| +DC(80V)     | +HV    |
| -DC          | -HV    |
| +24V         | +24    |
| GND          | -24    |
| EARTH GND    | PE     |

Wire power cord to N, L, and EARTH GROUND


#### Motor/Encoder Cable

Parker Cable: 006-2683-01

or

Parker Cable: 006-2690-01

| VIX X1 | Motor Leads |
|--------|-------------|
| 5      | GREEN       |
| 4      | WHITE       |
| 3      | BROWN       |
| 2      | YELLOW      |

Plug Encoder Connector (DB15 Male Pins) to VIX X2

Remove Parker cable DB15 Female Sockets limits connector if necessary.

Digikey: 277-1163-ND
TERM BLOCK PLUG 4POS STR 3.81MM
FEMALE SOCKETS

| Parker Cable Limits| 277-1163-ND |
|--------------------|-------------|
| GREEN              | 1           |
| WHITE              | 2           |
| BROWN              | 3           |

Digikey: 277-5781-ND
TERM BLOCK PLUG 4POS STR 3.81MM
MALE PINS

Digikey: 277-5811-ND
TERM BLOCK PLUG 4POS 90DEG 3.5MM
FEMALE SOCKETS

| 277-5781-ND | 277-5811-ND |
|-------------|-------------|
| 1           | 1           |
| 2           | 2,4         |
| 3           | 3           |

### EASI-V Software

[VIX Download](http://www.parker.com/portal/site/PARKER/menuitem.de7b26ee6a659c147cf26710237ad1ca/?vgnextoid=fcc9b5bbec622110VgnVCM10000032a71dacRCRD&vgnextdiv=&vgnextcatid=3216398&vgnextcat=VIX+DOWNLOADS&Wtky=&vgnextfmt=EN)

```shell
mkdir -p ~/parker/easi
sudo apt-get install wine
wine explorer
cd ~/.wine/dosdevices
ln -s /dev/ttyUSB0 com4
wine SETUP.EXE
```

### Download Settings

```shell
sudo apt-get install cutecom
cutecom
```

Device: /dev/ttyUSB0
Baud rate: 9600
Data bits: 8
Stop bits: 1
Parity: None

Open <./settings.txt>

Enter each line in Input field and press Enter.

At the end:

```shell
1SV
1Z
```

### step_dir_controller

```json
{
  "id":"getPropertyValues",
  "result":{
    "serialNumber":0,
    "ledsEnabled":true,
    "channelCount":2,
    "stepsPerPositionUnits":[
      1,
      1
    ],
    "velocityMax":[
      999999,
      999999
    ],
    "velocityMin":[
      20000,
      20000
    ],
    "accelerationMax":[
      1000000,
      1000000
    ],
    "enablePolarity":[
      "HIGH",
      "HIGH"
    ],
    "stepPolarityInverted":false,
    "dirPolarityInverted":false,
    "switchActivePolarity":"HIGH",
    "leftSwitchStopEnabled":[
      true,
      true
    ],
    "rightSwitchesEnabled":true,
    "rightSwitchStopEnabled":[
      true,
      true
    ],
    "switchSoftStopEnabled":[
      false,
      false
    ],
    "homeVelocity":[
      -50000,
      -50000
    ],
    "stagePositionMin":[
      0,
      0
    ],
    "stagePositionMax":[
      100000,
      100000
    ]
  }
}
```
