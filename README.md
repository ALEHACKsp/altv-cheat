# altv-cheat

Multihack for alt:V Multiplayer.

## Features

* JavaScript executor
* Script dumper
* Hardware ID spoofer
* Server-side currentWeapon block

## Commands

* **exec [file name]** - Loads a file for executing code (put files in C:/AltCheat), you can try the **sample_modmenu.js** I provided
* **find [string]** - Default value: **import**, it'll look for the specified string in the original scripts when executing or removing (see **remove** below)
* **remove [string]** - Loads a file for executing code (put files in C:/AltCheat)
* **dump** - Dumps client-side scripts of current server (C:/AltCheat/dumper)
* **serial [number]** - Sets the hwid hashes to the specified input, changes the SC Club nickname to a random string
* **weapon** - Blocks server-side getter for **currentWeapon** Player class property (use after spawning!)
